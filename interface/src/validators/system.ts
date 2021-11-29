import Schema from "async-validator";

export const OTA_SETTINGS_VALIDATOR = new Schema({
  port: [
    { required: true, message: "Port is required" },
    { type: "number", min: 1025, max: 65535, message: "Port must be between 1025 and 65535" }
  ],
  password: [
    { required: true, message: "Password is required" },
    { type: "string", min: 1, max: 64, message: "Password must be between 1 and 64 characters" }
  ]
});
