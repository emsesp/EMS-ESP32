/*
Copyright (c) 2022 Bert Melis. All rights reserved.

Parts are based on the original work of Marvin Roger:
https://github.com/marvinroger/async-mqtt-client

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#include "TypeDefs.h"

namespace espMqttClientTypes {

const char* disconnectReasonToString(DisconnectReason reason) {
  switch (reason) {
    case DisconnectReason::USER_OK:                            return "No error";
    case DisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION: return "Unacceptable protocol version";
    case DisconnectReason::MQTT_IDENTIFIER_REJECTED:           return "Identified rejected";
    case DisconnectReason::MQTT_SERVER_UNAVAILABLE:            return "Server unavailable";
    case DisconnectReason::MQTT_MALFORMED_CREDENTIALS:         return "Malformed credentials";
    case DisconnectReason::MQTT_NOT_AUTHORIZED:                return "Not authorized";
    case DisconnectReason::TLS_BAD_FINGERPRINT:                return "Bad fingerprint";
    case DisconnectReason::TCP_DISCONNECTED:                   return "TCP disconnected";
    default:                                                   return "";
  }
}

const char* subscribeReturncodeToString(SubscribeReturncode returnCode) {
  switch (returnCode) {
    case SubscribeReturncode::QOS0: return "QoS 0";
    case SubscribeReturncode::QOS1: return "QoS 1";
    case SubscribeReturncode::QOS2: return "QoS 2";
    case SubscribeReturncode::FAIL: return "Failed";
    default:                        return "";
  }
}

const char* errorToString(Error error) {
  switch (error) {
    case Error::SUCCESS:             return "Success";
    case Error::OUT_OF_MEMORY:       return "Out of memory";
    case Error::MAX_RETRIES:         return "Maximum retries exceeded";
    case Error::MALFORMED_PARAMETER: return "Malformed parameters";
    case Error::MISC_ERROR:          return "Misc error";
    default:                         return "";
  }
}

}  // end namespace espMqttClientTypes
