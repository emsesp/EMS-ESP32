/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#include "espMqttClient.h"

#if defined(ARDUINO_ARCH_ESP32)
espMqttClient::espMqttClient(espMqttClientTypes::UseInternalTask useInternalTask)
    : MqttClientSetup(useInternalTask)
    , _client() {
    _transport = &_client;
}

espMqttClient::espMqttClient(uint8_t priority, uint8_t core, uint32_t stackSize)
    : MqttClientSetup(espMqttClientTypes::UseInternalTask::YES, priority, core, stackSize)
    , _client() {
    _transport = &_client;
}

espMqttClientSecure::espMqttClientSecure(espMqttClientTypes::UseInternalTask useInternalTask)
    : MqttClientSetup(useInternalTask)
    , _client() {
    _transport = &_client;
}

espMqttClientSecure::espMqttClientSecure(uint8_t priority, uint8_t core, uint32_t stackSize)
    : MqttClientSetup(espMqttClientTypes::UseInternalTask::YES, priority, core, stackSize)
    , _client() {
    _transport = &_client;
}

espMqttClientSecure & espMqttClientSecure::setInsecure() {
    _client.client.setInsecure();
    return *this;
}

espMqttClientSecure & espMqttClientSecure::setCACert(const char * rootCA) {
    _client.client.setCACert(rootCA);
    return *this;
}

espMqttClientSecure & espMqttClientSecure::setCertificate(const char * clientCa) {
    _client.client.setCertificate(clientCa);
    return *this;
}

espMqttClientSecure & espMqttClientSecure::setPrivateKey(const char * privateKey) {
    _client.client.setPrivateKey(privateKey);
    return *this;
}

espMqttClientSecure & espMqttClientSecure::setPreSharedKey(const char * pskIdent, const char * psKey) {
    return *this;
}

#endif

#if defined(__linux__) || defined(__APPLE__)
espMqttClient::espMqttClient()
    : MqttClientSetup(espMqttClientTypes::UseInternalTask::NO)
    , _client() {
    _transport = &_client;
}
#endif
