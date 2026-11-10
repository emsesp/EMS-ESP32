#ifndef FirmwareSignature_h
#define FirmwareSignature_h

#include <cstddef>
#include <cstdint>
#include <cstring>

#ifdef EMSESP_STANDALONE
#include "PortableSha256.h"
#else
#include <mbedtls/md.h>
#endif

// Checks the ECDSA P-256 / SHA-256 signature that scripts/sign_firmware.py appends to a firmware image. A signed
// file is the image followed by a DER encoded signature, zero padded to SIGNATURE_SIZE bytes. This is the layout of
// tools/bin_signing.py in the Arduino core.
//
// The exact file size isn't known while it streams in (an upload's content length includes the multipart overhead),
// so write() keeps the last SIGNATURE_SIZE bytes back and only passes on bytes that are certainly image.
class FirmwareSignature {
  public:
    static constexpr size_t SIGNATURE_SIZE  = 512;
    static constexpr size_t PUBLIC_KEY_SIZE = 64; // uncompressed P-256 point, X then Y, big endian
    static constexpr size_t HASH_SIZE       = 32;

    FirmwareSignature() = default;
    ~FirmwareSignature();
    FirmwareSignature(const FirmwareSignature &)             = delete;
    FirmwareSignature & operator=(const FirmwareSignature &) = delete;

    void begin();

    // out(data, len) gets the image bytes, it returns false on a write error
    template <typename F>
    bool write(const uint8_t * data, size_t len, F && out) {
        const size_t total = tail_len_ + len;
        if (total > SIGNATURE_SIZE) {
            size_t       release   = total - SIGNATURE_SIZE;
            const size_t from_tail = release < tail_len_ ? release : tail_len_;
            if (from_tail) {
                hash(tail_, from_tail);
                if (!out(tail_, from_tail)) {
                    return false;
                }
                memmove(tail_, tail_ + from_tail, tail_len_ - from_tail);
                tail_len_ -= from_tail;
                release -= from_tail;
            }
            if (release) {
                hash(data, release);
                if (!out(data, release)) {
                    return false;
                }
                data += release;
                len -= release;
            }
        }
        memcpy(tail_ + tail_len_, data, len);
        tail_len_ += len;
        return true;
    }

    // after the last write, checks the held back bytes are a signature of the image by one of the keys
    bool verify(const uint8_t (*keys)[PUBLIC_KEY_SIZE], size_t num_keys);
    bool verify(); // with the EMS-ESP keys from firmware_keys.h

    // the held back bytes, they're the end of the image if the file isn't signed
    const uint8_t * tail() const {
        return tail_;
    }
    size_t tail_size() const {
        return tail_len_;
    }

    // r and s from a DER encoded signature followed by zero padding
    static bool parse_signature(const uint8_t * data, size_t len, uint8_t (&rs)[64]);

  private:
    uint8_t tail_[SIGNATURE_SIZE]{};
    size_t  tail_len_ = 0;
    bool    hashing_  = false;

#ifdef EMSESP_STANDALONE
    PortableSha256 sha_;
#else
    mbedtls_md_context_t sha_{};
    bool                 sha_setup_ = false;
#endif

    void hash(const uint8_t * data, size_t len);
    bool finish(uint8_t (&digest)[HASH_SIZE]);
};

#endif
