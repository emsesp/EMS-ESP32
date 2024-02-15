#ifndef ArduinoJsonJWT_H
#define ArduinoJsonJWT_H

#include <Arduino.h>
#include <ArduinoJson.h>

#include <libb64/cdecode.h>
#include <libb64/cencode.h>
#include <mbedtls/md.h>

class ArduinoJsonJWT {
  public:
    explicit ArduinoJsonJWT(String secret);

    void   setSecret(String secret);
    String getSecret();

    String buildJWT(JsonObject payload);
    void   parseJWT(String jwt, JsonDocument & jsonDocument);

  private:
    String _secret;

    String sign(String & value);

    static String encode(const char * cstr, int len);
    static String decode(String value);

    static const String & getJWTHeader() {
        static const String JWT_HEADER = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9";
        return JWT_HEADER;
    }
};

#endif