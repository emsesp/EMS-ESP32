import { Theme } from "@material-ui/core";
import { MqttStatus, MqttDisconnectReason } from "./types";

export const mqttStatusHighlight = ({ enabled, connected }: MqttStatus, theme: Theme) => {
  if (!enabled) {
    return theme.palette.info.main;
  }
  if (connected) {
    return theme.palette.success.main;
  }
  return theme.palette.error.main;
}

export const mqttStatus = ({ enabled, connected }: MqttStatus) => {
  if (!enabled) {
    return "Not enabled";
  }
  if (connected) {
    return "Connected";
  }
  return "Disconnected";
}

export const disconnectReason = ({ disconnect_reason }: MqttStatus) => {
  switch (disconnect_reason) {
    case MqttDisconnectReason.TCP_DISCONNECTED:
      return "TCP disconnected";
    case MqttDisconnectReason.MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
      return "Unacceptable protocol version";
    case MqttDisconnectReason.MQTT_IDENTIFIER_REJECTED:
      return "Client ID rejected";
    case MqttDisconnectReason.MQTT_SERVER_UNAVAILABLE:
      return "Server unavailable";
    case MqttDisconnectReason.MQTT_MALFORMED_CREDENTIALS:
      return "Malformed credentials";
    case MqttDisconnectReason.MQTT_NOT_AUTHORIZED:
      return "Not authorized";
    case MqttDisconnectReason.ESP8266_NOT_ENOUGH_SPACE:
      return "Device out of memory";
    case MqttDisconnectReason.TLS_BAD_FINGERPRINT:
      return "Server fingerprint invalid";
    default:
      return "Unknown"
  }
}
