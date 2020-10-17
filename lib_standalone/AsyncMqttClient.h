#ifndef ASYNCMQTTCLIENT_H_
#define ASYNCMQTTCLIENT_H_

#include "Arduino.h"
#include <functional>

enum class AsyncMqttClientDisconnectReason : int8_t {
    TCP_DISCONNECTED = 0,

    MQTT_UNACCEPTABLE_PROTOCOL_VERSION = 1,
    MQTT_IDENTIFIER_REJECTED           = 2,
    MQTT_SERVER_UNAVAILABLE            = 3,
    MQTT_MALFORMED_CREDENTIALS         = 4,
    MQTT_NOT_AUTHORIZED                = 5,

    ESP8266_NOT_ENOUGH_SPACE = 6,

    TLS_BAD_FINGERPRINT = 7
};

struct AsyncMqttClientMessageProperties {
    uint8_t qos;
    bool    dup;
    bool    retain;
};

namespace AsyncMqttClientInternals {

typedef std::function<void(bool sessionPresent)>                    OnConnectUserCallback;
typedef std::function<void(AsyncMqttClientDisconnectReason reason)> OnDisconnectUserCallback;
typedef std::function<void(uint16_t packetId, uint8_t qos)>         OnSubscribeUserCallback;
typedef std::function<void(uint16_t packetId)>                      OnUnsubscribeUserCallback;
typedef std::function<void(char * topic, char * payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)> OnMessageUserCallback;
typedef std::function<void(uint16_t packetId)> OnPublishUserCallback;
}; // namespace AsyncMqttClientInternals

class AsyncMqttClient {
  public:
    AsyncMqttClient();
    ~AsyncMqttClient();

    AsyncMqttClient & setKeepAlive(uint16_t keepAlive);
    AsyncMqttClient & setClientId(const char * clientId);
    AsyncMqttClient & setCleanSession(bool cleanSession);
    AsyncMqttClient & setMaxTopicLength(uint16_t maxTopicLength);
    AsyncMqttClient & setCredentials(const char * username, const char * password = nullptr);
    AsyncMqttClient & setWill(const char * topic, uint8_t qos, bool retain, const char * payload = nullptr, size_t length = 0) {
        return *this;
    }
    AsyncMqttClient & setServer(IPAddress ip, uint16_t port);
    AsyncMqttClient & setServer(const char * host, uint16_t port);

    AsyncMqttClient & onConnect(AsyncMqttClientInternals::OnConnectUserCallback callback) {
        return *this;
    }
    AsyncMqttClient & onDisconnect(AsyncMqttClientInternals::OnDisconnectUserCallback callback) {
        return *this;
    }
    AsyncMqttClient & onSubscribe(AsyncMqttClientInternals::OnSubscribeUserCallback callback) {
        return *this;
    }
    AsyncMqttClient & onUnsubscribe(AsyncMqttClientInternals::OnUnsubscribeUserCallback callback) {
        return *this;
    }
    AsyncMqttClient & onMessage(AsyncMqttClientInternals::OnMessageUserCallback callback) {
        return *this;
    }
    AsyncMqttClient & onPublish(AsyncMqttClientInternals::OnPublishUserCallback callback) {
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