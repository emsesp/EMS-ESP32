export enum MqttDisconnectReason {
  TCP_DISCONNECTED = 0,
  MQTT_UNACCEPTABLE_PROTOCOL_VERSION = 1,
  MQTT_IDENTIFIER_REJECTED = 2,
  MQTT_SERVER_UNAVAILABLE = 3,
  MQTT_MALFORMED_CREDENTIALS = 4,
  MQTT_NOT_AUTHORIZED = 5,
  ESP8266_NOT_ENOUGH_SPACE = 6,
  TLS_BAD_FINGERPRINT = 7
}

export interface MqttStatus {
  enabled: boolean;
  connected: boolean;
  client_id: string;
  disconnect_reason: MqttDisconnectReason;
}

export interface MqttSettings {
  enabled: boolean;
  host: string;
  port: number;
  username: string;
  password: string;
  client_id: string;
  keep_alive: number;
  clean_session: boolean;
  max_topic_length: number;
}
