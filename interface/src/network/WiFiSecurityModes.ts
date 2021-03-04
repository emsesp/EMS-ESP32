import { WiFiNetwork, WiFiEncryptionType } from "./types";

export const isNetworkOpen = ({ encryption_type }: WiFiNetwork) => encryption_type === WiFiEncryptionType.WIFI_AUTH_OPEN;

export const networkSecurityMode = ({ encryption_type }: WiFiNetwork) => {
  switch (encryption_type) {
    case WiFiEncryptionType.WIFI_AUTH_WEP:
      return "WEP";
    case WiFiEncryptionType.WIFI_AUTH_WPA_PSK:
      return "WPA";
    case WiFiEncryptionType.WIFI_AUTH_WPA2_PSK:
      return "WPA2";
    case WiFiEncryptionType.WIFI_AUTH_WPA_WPA2_PSK:
      return "WPA/WPA2";
    case WiFiEncryptionType.WIFI_AUTH_WPA2_ENTERPRISE:
      return "WPA2 Enterprise";
    case WiFiEncryptionType.WIFI_AUTH_OPEN:
      return "None";
    default:
      return "Unknown";
  }
}
