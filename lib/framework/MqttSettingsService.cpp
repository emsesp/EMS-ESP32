#include <MqttSettingsService.h>

/**
 * Retains a copy of the cstr provided in the pointer provided using dynamic allocation.
 *
 * Frees the pointer before allocation and leaves it as nullptr if cstr == nullptr.
 */
static char* retainCstr(const char* cstr, char** ptr) {
  // free up previously retained value if exists
  free(*ptr);
  *ptr = nullptr;

  // dynamically allocate and copy cstr (if non null)
  if (cstr != nullptr) {
    *ptr = (char*)malloc(strlen(cstr) + 1);
    strcpy(*ptr, cstr);
  }

  // return reference to pointer for convenience
  return *ptr;
}

MqttSettingsService::MqttSettingsService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager) :
    _httpEndpoint(MqttSettings::read, MqttSettings::update, this, server, MQTT_SETTINGS_SERVICE_PATH, securityManager),
    _fsPersistence(MqttSettings::read, MqttSettings::update, this, fs, MQTT_SETTINGS_FILE),
    _retainedHost(nullptr),
    _retainedClientId(nullptr),
    _retainedUsername(nullptr),
    _retainedPassword(nullptr),
    _reconfigureMqtt(false),
    _disconnectedAt(0),
    _disconnectReason(AsyncMqttClientDisconnectReason::TCP_DISCONNECTED),
    _mqttClient() {
#ifdef ESP32
  WiFi.onEvent(
      std::bind(&MqttSettingsService::onStationModeDisconnected, this, std::placeholders::_1, std::placeholders::_2),
      WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
  WiFi.onEvent(std::bind(&MqttSettingsService::onStationModeGotIP, this, std::placeholders::_1, std::placeholders::_2),
               WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
#elif defined(ESP8266)
  _onStationModeDisconnectedHandler = WiFi.onStationModeDisconnected(
      std::bind(&MqttSettingsService::onStationModeDisconnected, this, std::placeholders::_1));
  _onStationModeGotIPHandler =
      WiFi.onStationModeGotIP(std::bind(&MqttSettingsService::onStationModeGotIP, this, std::placeholders::_1));
#endif
  _mqttClient.onConnect(std::bind(&MqttSettingsService::onMqttConnect, this, std::placeholders::_1));
  _mqttClient.onDisconnect(std::bind(&MqttSettingsService::onMqttDisconnect, this, std::placeholders::_1));
  addUpdateHandler([&](const String& originId) { onConfigUpdated(); }, false);
}

MqttSettingsService::~MqttSettingsService() {
}

void MqttSettingsService::begin() {
  _fsPersistence.readFromFS();
}

void MqttSettingsService::loop() {
  if (_reconfigureMqtt || (_disconnectedAt && (unsigned long)(millis() - _disconnectedAt) >= MQTT_RECONNECTION_DELAY)) {
    // reconfigure MQTT client
    configureMqtt();

    // clear the reconnection flags
    _reconfigureMqtt = false;
    _disconnectedAt = 0;
  }
}

bool MqttSettingsService::isEnabled() {
  return _state.enabled;
}

bool MqttSettingsService::isConnected() {
  return _mqttClient.connected();
}

const char* MqttSettingsService::getClientId() {
  return _mqttClient.getClientId();
}

AsyncMqttClientDisconnectReason MqttSettingsService::getDisconnectReason() {
  return _disconnectReason;
}

AsyncMqttClient* MqttSettingsService::getMqttClient() {
  return &_mqttClient;
}

void MqttSettingsService::onMqttConnect(bool sessionPresent) {
  Serial.print(F("Connected to MQTT, "));
  if (sessionPresent) {
    Serial.println(F("with persistent session"));
  } else {
    Serial.println(F("without persistent session"));
  }
}

void MqttSettingsService::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.print(F("Disconnected from MQTT reason: "));
  Serial.println((uint8_t)reason);
  _disconnectReason = reason;
  _disconnectedAt = millis();
}

void MqttSettingsService::onConfigUpdated() {
  _reconfigureMqtt = true;
  _disconnectedAt = 0;
}

#ifdef ESP32
void MqttSettingsService::onStationModeGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  if (_state.enabled) {
    Serial.println(F("WiFi connection dropped, starting MQTT client."));
    onConfigUpdated();
  }
}

void MqttSettingsService::onStationModeDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  if (_state.enabled) {
    Serial.println(F("WiFi connection dropped, stopping MQTT client."));
    onConfigUpdated();
  }
}
#elif defined(ESP8266)
void MqttSettingsService::onStationModeGotIP(const WiFiEventStationModeGotIP& event) {
  if (_state.enabled) {
    Serial.println(F("WiFi connection dropped, starting MQTT client."));
    onConfigUpdated();
  }
}

void MqttSettingsService::onStationModeDisconnected(const WiFiEventStationModeDisconnected& event) {
  if (_state.enabled) {
    Serial.println(F("WiFi connection dropped, stopping MQTT client."));
    onConfigUpdated();
  }
}
#endif

void MqttSettingsService::configureMqtt() {
  // disconnect if currently connected
  _mqttClient.disconnect();

  // only connect if WiFi is connected and MQTT is enabled
  if (_state.enabled && WiFi.isConnected()) {
    Serial.println(F("Connecting to MQTT..."));
    _mqttClient.setServer(retainCstr(_state.host.c_str(), &_retainedHost), _state.port);
    if (_state.username.length() > 0) {
      _mqttClient.setCredentials(
          retainCstr(_state.username.c_str(), &_retainedUsername),
          retainCstr(_state.password.length() > 0 ? _state.password.c_str() : nullptr, &_retainedPassword));
    } else {
      _mqttClient.setCredentials(retainCstr(nullptr, &_retainedUsername), retainCstr(nullptr, &_retainedPassword));
    }
    _mqttClient.setClientId(retainCstr(_state.clientId.c_str(), &_retainedClientId));
    _mqttClient.setKeepAlive(_state.keepAlive);
    _mqttClient.setCleanSession(_state.cleanSession);
    _mqttClient.setMaxTopicLength(_state.maxTopicLength);
    _mqttClient.connect();
  }
}
