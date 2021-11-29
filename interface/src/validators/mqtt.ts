import Schema from "async-validator";
import { IP_OR_HOSTNAME_VALIDATOR } from './shared';

export const MQTT_SETTINGS_VALIDATOR = new Schema({
  host: [
    { required: true, message: "Host is required" },
    IP_OR_HOSTNAME_VALIDATOR
  ],
  port: [
    { required: true, message: "Port is required" },
    { type: "number", min: 0, max: 65535, message: "Port must be between 0 and 65535" }
  ],
  keep_alive: [
    { required: true, message: "Keep alive is required" },
    { type: "number", min: 1, max: 86400, message: "Keep alive must be between 1 and 86400" }
  ],
  max_topic_length: [
    { required: true, message: "Max topic length is required" },
    { type: "number", min: 16, max: 1024, message: "Max topic length must be between 16 and 1024" }
  ]
});
