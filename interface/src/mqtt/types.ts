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
  mqtt_fails: number;
}

export interface MqttSettings {
  enabled: boolean;
  host: string;
  port: number;
  base: string;
  username: string;
  password: string;
  client_id: string;
  keep_alive: number;
  clean_session: boolean;
  max_topic_length: number;
  publish_time_boiler: number;
  publish_time_thermostat: number;
  publish_time_solar: number;
  publish_time_mixer: number;
  publish_time_other: number;
  publish_time_sensor: number;
  dallas_format: number;
  bool_format: number;
  mqtt_qos: number;
  mqtt_retain: boolean;
  ha_enabled: boolean;
  ha_climate_format: number;
  nested_format: boolean;
}
