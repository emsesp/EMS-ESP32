// Firmware signature checks, see lib/FirmwareSignature

#include <FirmwareSignature.h>
#include <firmware_keys.h>
#include <micro-ecc/uECC.h>

#include <vector>

namespace fwsig {

// signatures of image() by a test key, made with the Python cryptography package
// keys: [0] an unrelated key, [1] the test key
static constexpr uint8_t KEYS[2][64] = {
    {0x8b, 0xdd, 0x52, 0xf7, 0x6e, 0xc3, 0x03, 0x24, 0xd5, 0x4c, 0xa2, 0x09, 0x73, 0x79, 0x23, 0x8f, 0x91, 0x37, 0x2f, 0x70, 0x82, 0xc8,
     0x84, 0x10, 0x57, 0x7c, 0xdd, 0xf0, 0x70, 0x3b, 0xfe, 0xa7, 0x95, 0x75, 0xb0, 0x8d, 0x3b, 0xb9, 0xbd, 0xaa, 0xbe, 0x71, 0x46, 0x79,
     0x92, 0xfd, 0x5b, 0x0f, 0xfb, 0x9b, 0xf9, 0xe8, 0x0d, 0x21, 0xb9, 0xac, 0x65, 0x9c, 0xb8, 0xc4, 0xd6, 0xe4, 0xb9, 0xf8},
    {0xc4, 0xbd, 0xed, 0xce, 0xf4, 0x89, 0xeb, 0x28, 0x76, 0x75, 0xab, 0xd9, 0x4b, 0x8f, 0x25, 0xf3, 0x6b, 0xd4, 0xaa, 0x2e, 0xdd, 0xaa,
     0xdd, 0x2a, 0xa5, 0xa2, 0x9e, 0x69, 0x42, 0x5f, 0x5b, 0x1d, 0xf0, 0x48, 0xab, 0x8e, 0x7b, 0xe0, 0xc5, 0x13, 0x9a, 0x3b, 0xd1, 0x03,
     0xd8, 0xc3, 0x45, 0x0d, 0x78, 0xed, 0xbf, 0x2c, 0xce, 0x30, 0x32, 0xb9, 0x5a, 0x0c, 0x77, 0x0b, 0xb2, 0xf6, 0x65, 0xe7}};

// r has a leading zero byte
static constexpr uint8_t SIG_R33[] = {0x30, 0x45, 0x02, 0x21, 0x00, 0x82, 0x65, 0x69, 0x71, 0xfc, 0x15, 0x76, 0x89, 0x60, 0xb4, 0x60, 0xf8, 0x51,
                                      0x8b, 0xef, 0xf3, 0x79, 0x3d, 0xb9, 0xb5, 0x50, 0xd5, 0x74, 0x21, 0x97, 0x75, 0x83, 0x3a, 0x75, 0xf9, 0x3d,
                                      0xba, 0x02, 0x20, 0x28, 0x6c, 0x6d, 0x00, 0x8e, 0x33, 0x52, 0x5a, 0x85, 0x29, 0xbb, 0xe2, 0xe1, 0x93, 0x90,
                                      0xe1, 0xfe, 0xd7, 0x27, 0x9a, 0x47, 0xb0, 0x8e, 0xc3, 0x56, 0x65, 0xf7, 0x2f, 0x6d, 0x9a, 0xdf, 0x56};
// s has a leading zero byte
static constexpr uint8_t SIG_R32[] = {0x30, 0x45, 0x02, 0x20, 0x48, 0x08, 0x4e, 0xee, 0xe9, 0xb3, 0x37, 0x45, 0x24, 0x29, 0x6f, 0x49, 0x6f, 0xab,
                                      0xbe, 0xc8, 0x88, 0xd9, 0x56, 0xf5, 0x57, 0x44, 0x6e, 0x6e, 0x30, 0x58, 0x5d, 0x43, 0x5b, 0xc5, 0xad, 0x15,
                                      0x02, 0x21, 0x00, 0xb4, 0x9f, 0xa0, 0x4e, 0x1e, 0x9e, 0x43, 0x69, 0xdf, 0x37, 0x69, 0x68, 0x39, 0xec, 0x63,
                                      0xac, 0x0d, 0x41, 0x31, 0xd8, 0xaf, 0xb0, 0x05, 0x5a, 0xd8, 0x3d, 0xf8, 0x91, 0x56, 0x4d, 0xa4, 0xf7};

static std::vector<uint8_t> image() {
    std::vector<uint8_t> data(3000);
    for (size_t i = 0; i < data.size(); i++) {
        data[i] = static_cast<uint8_t>(i * 31 + 7);
    }
    return data;
}

template <size_t N>
static std::vector<uint8_t> signed_file(const uint8_t (&sig)[N]) {
    std::vector<uint8_t> file = image();
    file.insert(file.end(), sig, sig + N);
    file.resize(file.size() + FirmwareSignature::SIGNATURE_SIZE - N, 0);
    return file;
}

struct Result {
    bool                 ok;
    std::vector<uint8_t> written;
    std::vector<uint8_t> tail;
};

static Result stream(const std::vector<uint8_t> & file, size_t chunk, const uint8_t (*keys)[64] = KEYS + 1, size_t num_keys = 1) {
    FirmwareSignature sig;
    Result            r;
    sig.begin();
    for (size_t pos = 0; pos < file.size(); pos += chunk) {
        const size_t len = std::min(chunk, file.size() - pos);
        sig.write(file.data() + pos, len, [&r](const uint8_t * d, size_t n) {
            r.written.insert(r.written.end(), d, d + n);
            return true;
        });
    }
    r.ok = sig.verify(keys, num_keys);
    r.tail.assign(sig.tail(), sig.tail() + sig.tail_size());
    return r;
}

} // namespace fwsig

// a signed image passes in any chunk size, and only the image is written
void security_test_signature_valid() {
    for (const size_t chunk : {1, 7, 100, 511, 512, 513, 1460, 5000}) {
        for (const auto & file : {fwsig::signed_file(fwsig::SIG_R33), fwsig::signed_file(fwsig::SIG_R32)}) {
            const auto r = fwsig::stream(file, chunk);
            TEST_ASSERT_TRUE(r.ok);
            TEST_ASSERT_TRUE(r.written == fwsig::image());
        }
    }
}

// any of the keys can sign, others can't
void security_test_signature_keys() {
    const auto file = fwsig::signed_file(fwsig::SIG_R33);
    TEST_ASSERT_TRUE(fwsig::stream(file, 256, fwsig::KEYS, 2).ok);
    TEST_ASSERT_FALSE(fwsig::stream(file, 256, fwsig::KEYS, 1).ok);
    TEST_ASSERT_FALSE(fwsig::stream(file, 256, fwsig::KEYS, 0).ok);
}

// a changed image, signature or padding fails
void security_test_signature_tampered() {
    const auto good = fwsig::signed_file(fwsig::SIG_R33);
    for (const size_t pos : {size_t(0), size_t(1500), size_t(2999), size_t(3010), size_t(3000 + 0x46), good.size() - 1}) {
        auto file = good;
        file[pos] ^= 0x01;
        TEST_ASSERT_FALSE(fwsig::stream(file, 300).ok);
    }

    auto longer = good;
    longer.insert(longer.begin(), 0x00); // one more byte of image
    TEST_ASSERT_FALSE(fwsig::stream(longer, 300).ok);
}

// without a signature nothing is lost: the written bytes and the tail together are the whole file
void security_test_signature_unsigned() {
    const auto file = fwsig::image();
    const auto r    = fwsig::stream(file, 333);
    TEST_ASSERT_FALSE(r.ok);
    TEST_ASSERT_EQUAL(FirmwareSignature::SIGNATURE_SIZE, r.tail.size());
    auto joined = r.written;
    joined.insert(joined.end(), r.tail.begin(), r.tail.end());
    TEST_ASSERT_TRUE(joined == file);

    const std::vector<uint8_t> tiny(100, 0xE9);
    const auto                 t = fwsig::stream(tiny, 10);
    TEST_ASSERT_FALSE(t.ok);
    TEST_ASSERT_EQUAL(0, t.written.size());
    TEST_ASSERT_TRUE(t.tail == tiny);
}

// malformed DER is rejected before any curve maths
void security_test_signature_parse() {
    uint8_t rs[64];
    uint8_t buf[FirmwareSignature::SIGNATURE_SIZE]{};
    memcpy(buf, fwsig::SIG_R33, sizeof(fwsig::SIG_R33));
    TEST_ASSERT_TRUE(FirmwareSignature::parse_signature(buf, sizeof(buf), rs));
    TEST_ASSERT_EQUAL_HEX8(0x82, rs[0]);   // the leading zero is dropped
    TEST_ASSERT_EQUAL_HEX8(0x28, rs[32]);  // s starts at byte 32
    TEST_ASSERT_EQUAL_HEX8(0x56, rs[63]);

    auto broken = [&](size_t pos, uint8_t value) {
        uint8_t copy[sizeof(buf)];
        memcpy(copy, buf, sizeof(buf));
        copy[pos] = value;
        return !FirmwareSignature::parse_signature(copy, sizeof(copy), rs);
    };
    TEST_ASSERT_TRUE(broken(0, 0x31));  // not a SEQUENCE
    TEST_ASSERT_TRUE(broken(1, 0x81));  // long form length
    TEST_ASSERT_TRUE(broken(1, 0x44));  // SEQUENCE shorter than its content
    TEST_ASSERT_TRUE(broken(1, 0x46));  // SEQUENCE longer than its content
    TEST_ASSERT_TRUE(broken(2, 0x04));  // r not an INTEGER
    TEST_ASSERT_TRUE(broken(3, 0x00));  // empty r
    TEST_ASSERT_TRUE(broken(3, 0x7F));  // r longer than the SEQUENCE
    TEST_ASSERT_TRUE(broken(4, 0x80));  // negative r
    TEST_ASSERT_TRUE(broken(71, 0x01)); // padding not zero
    TEST_ASSERT_FALSE(FirmwareSignature::parse_signature(buf, 7, rs));

    // a 33 byte r without a leading zero doesn't fit
    uint8_t wide[FirmwareSignature::SIGNATURE_SIZE]{};
    memcpy(wide, fwsig::SIG_R33, sizeof(fwsig::SIG_R33));
    wide[4] = 0x01;
    TEST_ASSERT_FALSE(FirmwareSignature::parse_signature(wide, sizeof(wide), rs));
}

// the built-in keys are points on the curve
void security_test_firmware_keys() {
    const size_t count = sizeof(FIRMWARE_PUBLIC_KEYS) / sizeof(FIRMWARE_PUBLIC_KEYS[0]);
    TEST_ASSERT_EQUAL(2, count); // the release key and the spare
    for (size_t i = 0; i < count; i++) {
        TEST_ASSERT_EQUAL(1, uECC_valid_public_key(FIRMWARE_PUBLIC_KEYS[i], uECC_secp256r1()));
    }
    TEST_ASSERT_FALSE(fwsig::stream(fwsig::signed_file(fwsig::SIG_R33), 512, FIRMWARE_PUBLIC_KEYS, count).ok);

    // a build signed with any other key is a custom build
    const auto        file = fwsig::signed_file(fwsig::SIG_R33);
    FirmwareSignature sig;
    sig.begin();
    sig.write(file.data(), file.size(), [](const uint8_t *, size_t) { return true; });
    TEST_ASSERT_FALSE(sig.verify());
}

void run_firmware_signature_tests() {
    RUN_TEST(security_test_signature_valid);
    RUN_TEST(security_test_signature_keys);
    RUN_TEST(security_test_signature_tampered);
    RUN_TEST(security_test_signature_unsigned);
    RUN_TEST(security_test_signature_parse);
    RUN_TEST(security_test_firmware_keys);
}
