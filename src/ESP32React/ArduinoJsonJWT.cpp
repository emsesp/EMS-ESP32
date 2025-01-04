#include "ArduinoJsonJWT.h"

#include <array>

ArduinoJsonJWT::ArduinoJsonJWT(String secret)
    : _secret(std::move(secret)) {
}

void ArduinoJsonJWT::setSecret(String secret) {
    _secret = std::move(secret);
}

String ArduinoJsonJWT::getSecret() {
    return _secret;
}

String ArduinoJsonJWT::buildJWT(JsonObject payload) {
    // serialize, then encode payload
    String jwt;
    serializeJson(payload, jwt);
    jwt = encode(jwt.c_str(), static_cast<int>(jwt.length()));

    // add the header to payload
    jwt = getJWTHeader() + '.' + jwt;

    // add signature
    jwt += '.' + sign(jwt);

    return jwt;
}

void ArduinoJsonJWT::parseJWT(String jwt, JsonDocument & jsonDocument) {
    // clear json document before we begin, jsonDocument wil be null on failure
    jsonDocument.clear();

    const String &     jwt_header      = getJWTHeader();
    const unsigned int jwt_header_size = jwt_header.length();

    // must have the correct header and delimiter
    if (!jwt.startsWith(jwt_header) || jwt.indexOf('.') != static_cast<int>(jwt_header_size)) {
        return;
    }

    // check there is a signature delimieter
    const int signatureDelimiterIndex = jwt.lastIndexOf('.');
    if (signatureDelimiterIndex == static_cast<int>(jwt_header_size)) {
        return;
    }

    // check the signature is valid
    const String signature = jwt.substring(static_cast<unsigned int>(signatureDelimiterIndex) + 1);
    jwt                    = jwt.substring(0, static_cast<unsigned int>(signatureDelimiterIndex));
    if (sign(jwt) != signature) {
        return;
    }

    // decode payload
    jwt = jwt.substring(jwt_header_size + 1);
    jwt = decode(jwt);

    // parse payload, clearing json document after failure
    const DeserializationError error = deserializeJson(jsonDocument, jwt);
    if (error != DeserializationError::Ok || !jsonDocument.is<JsonObject>()) {
        jsonDocument.clear();
    }
}

/*
 * ESP32 uses mbedtls, with decent HMAC implementations supporting sha256, as well as others.
 * No need to pull in additional crypto libraries - lets use what we already have.
 */
String ArduinoJsonJWT::sign(String & payload) {
    std::array<unsigned char, 32> hmacResult{};
    {
        mbedtls_md_context_t ctx;
        mbedtls_md_type_t    md_type = MBEDTLS_MD_SHA256;
        mbedtls_md_init(&ctx);
        mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
        mbedtls_md_hmac_starts(&ctx, reinterpret_cast<const unsigned char *>(_secret.c_str()), _secret.length());
        mbedtls_md_hmac_update(&ctx, reinterpret_cast<const unsigned char *>(payload.c_str()), payload.length());
        mbedtls_md_hmac_finish(&ctx, hmacResult.data());
        mbedtls_md_free(&ctx);
    }
    return encode(reinterpret_cast<const char *>(hmacResult.data()), hmacResult.size());
}

String ArduinoJsonJWT::encode(const char * cstr, int inputLen) {
    // prepare encoder
    base64_encodestate _state;
    base64_init_encodestate(&_state);

    // prepare buffer of correct length
    const auto bufferLength = static_cast<std::size_t>(base64_encode_expected_len(inputLen)) + 1;
    auto *     buffer       = new char[bufferLength];

    // encode to buffer
    int len = base64_encode_block(cstr, inputLen, &buffer[0], &_state);
    len += base64_encode_blockend(&buffer[len], &_state);
    buffer[len] = '\0';

    // convert to arduino string, freeing buffer
    auto result = String(buffer);
    delete[] buffer;
    buffer = nullptr;

    // remove padding and convert to URL safe form
    while (result.length() > 0 && result.charAt(result.length() - 1) == '=') {
        result.remove(result.length() - 1);
    }
    result.replace('+', '-');
    result.replace('/', '_');

    // return as string
    return result;
}

String ArduinoJsonJWT::decode(String value) {
    // convert to standard base64
    value.replace('-', '+');
    value.replace('_', '/');

    // prepare buffer of correct length
    const auto bufferLength = static_cast<std::size_t>(base64_decode_expected_len(value.length()) + 1);
    auto *     buffer       = new char[bufferLength];

    // decode
    const int len = base64_decode_chars(value.c_str(), static_cast<int>(value.length()), &buffer[0]);
    buffer[len]   = '\0';

    // convert to arduino string, freeing buffer
    auto result = String(buffer);
    delete[] buffer;
    buffer = nullptr;

    // return as string
    return result;
}