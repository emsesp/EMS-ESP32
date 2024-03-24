export enum MqttDisconnectReason {
  USER_OK = 0,
  MQTT_UNACCEPTABLE_PROTOCOL_VERSION = 1,
  MQTT_IDENTIFIER_REJECTED = 2,
  MQTT_SERVER_UNAVAILABLE = 3,
  MQTT_MALFORMED_CREDENTIALS = 4,
  MQTT_NOT_AUTHORIZED = 5,
  TLS_BAD_FINGERPRINT = 6,
  TCP_DISCONNECTED = 7
}

export interface MqttStatusType {
  enabled: boolean;
  connected: boolean;
  client_id: string;
  disconnect_reason: MqttDisconnectReason;
  mqtt_fails: number;
  mqtt_queued: number;
  connect_count: number;
}

export interface MqttSettingsType {
  enabled: boolean;
  host: string;
  port: number;
  base: string;
  rootCA?: string;
  enableTLS?: boolean;
  username: string;
  password: string;
  client_id: string;
  keep_alive: number;
  clean_session: boolean;
  entity_format: number;
  publish_time_boiler: number;
  publish_time_thermostat: number;
  publish_time_solar: number;
  publish_time_mixer: number;
  publish_time_water: number;
  publish_time_other: number;
  publish_time_sensor: number;
  publish_time_heartbeat: number;
  mqtt_qos: number;
  mqtt_retain: boolean;
  ha_enabled: boolean;
  nested_format: number;
  send_response: boolean;
  publish_single: boolean;
  publish_single2cmd: boolean;
  discovery_prefix: string;
  discovery_type: number;
}
