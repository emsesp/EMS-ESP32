/*
Copyright (c) 2022 Bert Melis. All rights reserved.

API is based on the original work of Marvin Roger:
https://github.com/marvinroger/async-mqtt-client

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#include "Transport/ClientSync.h"
#include "Transport/ClientSecureSync.h"
#elif defined(__linux__)
#include "Transport/ClientPosix.h"
#endif

#include "MqttClientSetup.h"

#if defined(ARDUINO_ARCH_ESP8266)
class espMqttClient : public MqttClientSetup<espMqttClient> {
 public:
  espMqttClient();

 protected:
  espMqttClientInternals::ClientSync _client;
};

class espMqttClientSecure : public MqttClientSetup<espMqttClientSecure> {
 public:
  espMqttClientSecure();
  espMqttClientSecure& setInsecure();
  espMqttClientSecure& setFingerprint(const uint8_t fingerprint[20]);
  espMqttClientSecure& setTrustAnchors(const X509List *ta);
  espMqttClientSecure& setClientRSACert(const X509List *cert, const PrivateKey *sk);
  espMqttClientSecure& setClientECCert(const X509List *cert, const PrivateKey *sk, unsigned allowed_usages, unsigned cert_issuer_key_type);
  espMqttClientSecure& setCertStore(CertStoreBase *certStore);

 protected:
  espMqttClientInternals::ClientSecureSync _client;
};
#endif

#if defined(ARDUINO_ARCH_ESP32)
class espMqttClient : public MqttClientSetup<espMqttClient> {
 public:
  explicit espMqttClient(espMqttClientTypes::UseInternalTask useInternalTask);
  explicit espMqttClient(uint8_t priority = 1, uint8_t core = 1);

 protected:
  espMqttClientInternals::ClientSync _client;
};

class espMqttClientSecure : public MqttClientSetup<espMqttClientSecure> {
 public:
  explicit espMqttClientSecure(espMqttClientTypes::UseInternalTask useInternalTask);
  explicit espMqttClientSecure(uint8_t priority = 1, uint8_t core = 1);
  espMqttClientSecure& setInsecure();
  espMqttClientSecure& setCACert(const char* rootCA);
  espMqttClientSecure& setCertificate(const char* clientCa);
  espMqttClientSecure& setPrivateKey(const char* privateKey);
  espMqttClientSecure& setPreSharedKey(const char* pskIdent, const char* psKey);

 protected:
  espMqttClientInternals::ClientSecureSync _client;
};
#endif

#if defined(__linux__)
class espMqttClient : public MqttClientSetup<espMqttClient> {
 public:
  espMqttClient();

 protected:
  espMqttClientInternals::ClientPosix _client;
};
#endif
