#ifndef ESPMQTTCLIENT_H_
#define ESPMQTTCLIENT_H_

#include "Arduino.h"
#include <functional>

namespace espMqttClientTypes {

enum class DisconnectReason : uint8_t {
    USER_OK                            = 0,
    MQTT_UNACCEPTABLE_PROTOCOL_VERSION = 1,
    MQTT_IDENTIFIER_REJECTED           = 2,
    MQTT_SERVER_UNAVAILABLE            = 3,
    MQTT_MALFORMED_CREDENTIALS         = 4,
    MQTT_NOT_AUTHORIZED                = 5,
    TLS_BAD_FINGERPRINT                = 6,
    TCP_DISCONNECTED                   = 7
};
const char * disconnectReasonToString(DisconnectReason reason);

enum class SubscribeReturncode : uint8_t { QOS0 = 0x00, QOS1 = 0x01, QOS2 = 0x02, FAIL = 0X80 };
const char * subscribeReturncodeToString(SubscribeReturncode returnCode);

enum class Error : uint8_t { SUCCESS = 0, OUT_OF_MEMORY = 1, MAX_RETRIES = 2, MALFORMED_PARAMETER = 3, MISC_ERROR = 4 };
const char * errorToString(Error error);

struct MessageProperties {
    uint8_t  qos;
    bool     dup;
    bool     retain;
    uint16_t packetId;
};

typedef std::function<void(bool sessionPresent)>                                                    OnConnectCallback;
typedef std::function<void(DisconnectReason reason)>                                                OnDisconnectCallback;
typedef std::function<void(uint16_t packetId, const SubscribeReturncode * returncodes, size_t len)> OnSubscribeCallback;
typedef std::function<void(uint16_t packetId)>                                                      OnUnsubscribeCallback;
typedef std::function<void(const MessageProperties & properties, const char * topic, const uint8_t * payload, size_t len, size_t index, size_t total)> OnMessageCallback;
typedef std::function<void(uint16_t packetId)>                              OnPublishCallback;
typedef std::function<size_t(uint8_t * data, size_t maxSize, size_t index)> PayloadCallback;
typedef std::function<void(uint16_t packetId, Error error)>                 OnErrorCallback;

} // namespace espMqttClientTypes

class espMqttClient {
  public:
    espMqttClient();
    ~espMqttClient();

    espMqttClient & setKeepAlive(uint16_t keepAlive);
    espMqttClient & setClientId(const char * clientId);
    espMqttClient & setCleanSession(bool cleanSession);
    espMqttClient & setMaxTopicLength(uint16_t maxTopicLength);
    espMqttClient & setCredentials(const char * username, const char * password = nullptr);
    espMqttClient & setWill(const char * topic, uint8_t qos, bool retain, const char * payload = nullptr, size_t length = 0) {
        return *this;
    }
    espMqttClient & setServer(IPAddress ip, uint16_t port);
    espMqttClient & setServer(const char * host, uint16_t port);

    espMqttClient & onConnect(espMqttClientTypes::OnConnectCallback callback) {
        return *this;
    }
    espMqttClient & onDisconnect(espMqttClientTypes::OnDisconnectCallback callback) {
        return *this;
    }
    espMqttClient & onSubscribe(espMqttClientTypes::OnSubscribeCallback callback) {
        return *this;
    }
    espMqttClient & onUnsubscribe(espMqttClientTypes::OnUnsubscribeCallback callback) {
        return *this;
    }
    espMqttClient & onMessage(espMqttClientTypes::OnMessageCallback callback) {
        return *this;
    }
    espMqttClient & onPublish(espMqttClientTypes::OnPublishCallback callback) {
        return *this;
    }

    bool connected() const {
        return false;
    }
    void connect() {
    }
    void disconnect(bool force = false) {
    }
    uint16_t subscribe(const char * topic, uint8_t qos) {
        return 1;
    }
    uint16_t unsubscribe(const char * topic) {
        return 1;
    }
    uint16_t publish(const char * topic, uint8_t qos, bool retain, const char * payload = nullptr, size_t length = 0, bool dup = false, uint16_t message_id = 0) {
        return 1;
    }

    const char * getClientId() {
        return "12";
    }

    uint16_t getQueue() const {
        return 0;
    }

  private:
    bool         _connected;
    bool         _connectPacketNotEnoughSpace;
    bool         _disconnectOnPoll;
    bool         _tlsBadFingerprint;
    uint32_t     _lastClientActivity;
    uint32_t     _lastServerActivity;
    uint32_t     _lastPingRequestTime;
    char         _generatedClientId[18 + 1]; // esp8266-abc123 and esp32-abcdef123456
    IPAddress    _ip;
    const char * _host;
    bool         _useIp;
    uint16_t     _port;
    uint16_t     _keepAlive;
    bool         _cleanSession;
    const char * _clientId;
    const char * _username;
    const char * _password;
    const char * _willTopic;
    const char * _willPayload;
    uint16_t     _willPayloadLength;
    uint8_t      _willQos;
    bool         _willRetain;
};

#endif
