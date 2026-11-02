#include "FirmwareSignature.h"
#include "firmware_keys.h"

#include "micro-ecc/uECC.h"

FirmwareSignature::~FirmwareSignature() {
#ifndef EMSESP_STANDALONE
    if (sha_setup_) {
        mbedtls_md_free(&sha_);
    }
#endif
}

void FirmwareSignature::begin() {
    tail_len_ = 0;
#ifdef EMSESP_STANDALONE
    sha_     = PortableSha256();
    hashing_ = true;
#else
    if (!sha_setup_) {
        mbedtls_md_init(&sha_);
        sha_setup_ = mbedtls_md_setup(&sha_, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 0) == 0;
    }
    hashing_ = sha_setup_ && mbedtls_md_starts(&sha_) == 0;
#endif
}

void FirmwareSignature::hash(const uint8_t * data, size_t len) {
    if (!hashing_) {
        return;
    }
#ifdef EMSESP_STANDALONE
    sha_.update(data, len);
#else
    hashing_ = mbedtls_md_update(&sha_, data, len) == 0;
#endif
}

bool FirmwareSignature::finish(uint8_t (&digest)[HASH_SIZE]) {
    if (!hashing_) {
        return false;
    }
    hashing_ = false;
#ifdef EMSESP_STANDALONE
    sha_.finish(digest);
    return true;
#else
    return mbedtls_md_finish(&sha_, digest) == 0;
#endif
}

bool FirmwareSignature::parse_signature(const uint8_t * data, size_t len, uint8_t (&rs)[64]) {
    // SEQUENCE { INTEGER r, INTEGER s }. A P-256 signature is at most 72 bytes, so all lengths are one byte
    if (len < 8 || data[0] != 0x30 || data[1] > 0x7F) {
        return false;
    }
    const size_t end = 2 + data[1];
    if (end > len) {
        return false;
    }

    size_t pos = 2;
    for (int i = 0; i < 2; i++) {
        if (pos + 2 > end || data[pos] != 0x02) {
            return false;
        }
        size_t n = data[pos + 1];
        pos += 2;
        if (n == 0 || pos + n > end || (data[pos] & 0x80)) {
            return false; // empty or negative
        }
        while (n > 32 && data[pos] == 0) {
            pos++;
            n--;
        }
        if (n > 32) {
            return false;
        }
        uint8_t * out = rs + i * 32;
        memset(out, 0, 32 - n);
        memcpy(out + 32 - n, data + pos, n);
        pos += n;
    }

    if (pos != end) {
        return false;
    }
    for (size_t i = end; i < len; i++) {
        if (data[i] != 0) {
            return false;
        }
    }
    return true;
}

bool FirmwareSignature::verify(const uint8_t (*keys)[PUBLIC_KEY_SIZE], size_t num_keys) {
    uint8_t digest[HASH_SIZE];
    if (!finish(digest) || tail_len_ != SIGNATURE_SIZE) {
        return false;
    }

    uint8_t rs[64];
    if (!parse_signature(tail_, tail_len_, rs)) {
        return false;
    }

    for (size_t i = 0; i < num_keys; i++) {
        if (uECC_verify(keys[i], digest, sizeof(digest), rs, uECC_secp256r1())) {
            return true;
        }
    }
    return false;
}

bool FirmwareSignature::verify() {
    return verify(FIRMWARE_PUBLIC_KEYS, sizeof(FIRMWARE_PUBLIC_KEYS) / sizeof(FIRMWARE_PUBLIC_KEYS[0]));
}

bool FirmwareSignature::verify_emsesp() {
    static_assert(EMSESP_KEY_COUNT <= sizeof(FIRMWARE_PUBLIC_KEYS) / sizeof(FIRMWARE_PUBLIC_KEYS[0]), "EMSESP_KEY_COUNT is too big");
    return verify(FIRMWARE_PUBLIC_KEYS, EMSESP_KEY_COUNT);
}
