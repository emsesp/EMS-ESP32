const ENDPOINT_ROOT = "/rest/";

const FEATURES_ENDPOINT = ENDPOINT_ROOT + "features";
const NTP_STATUS_ENDPOINT = ENDPOINT_ROOT + "ntpStatus";
const NTP_SETTINGS_ENDPOINT = ENDPOINT_ROOT + "ntpSettings";
const TIME_ENDPOINT = ENDPOINT_ROOT + "time";
const AP_SETTINGS_ENDPOINT = ENDPOINT_ROOT + "apSettings";
const AP_STATUS_ENDPOINT = ENDPOINT_ROOT + "apStatus";
const SCAN_NETWORKS_ENDPOINT = ENDPOINT_ROOT + "scanNetworks";
const LIST_NETWORKS_ENDPOINT = ENDPOINT_ROOT + "listNetworks";
const NETWORK_SETTINGS_ENDPOINT = ENDPOINT_ROOT + "networkSettings";
const NETWORK_STATUS_ENDPOINT = ENDPOINT_ROOT + "networkStatus";
const OTA_SETTINGS_ENDPOINT = ENDPOINT_ROOT + "otaSettings";
const UPLOAD_FIRMWARE_ENDPOINT = ENDPOINT_ROOT + "uploadFirmware";
const MQTT_SETTINGS_ENDPOINT = ENDPOINT_ROOT + "mqttSettings";
const MQTT_STATUS_ENDPOINT = ENDPOINT_ROOT + "mqttStatus";
const SYSTEM_STATUS_ENDPOINT = ENDPOINT_ROOT + "systemStatus";
const SIGN_IN_ENDPOINT = ENDPOINT_ROOT + "signIn";
const VERIFY_AUTHORIZATION_ENDPOINT = ENDPOINT_ROOT + "verifyAuthorization";
const SECURITY_SETTINGS_ENDPOINT = ENDPOINT_ROOT + "securitySettings";
const RESTART_ENDPOINT = ENDPOINT_ROOT + "restart";
const FACTORY_RESET_ENDPOINT = ENDPOINT_ROOT + "factoryReset";

const express = require('express');
const path = require('path');
const app = express(),
    bodyParser = require("body-parser");
const port = process.env.PORT || 3080;

app.use(bodyParser.json());

app.use(express.static(path.join(__dirname, '../interface/build')));

app.get(FEATURES_ENDPOINT, (req, res) => {
    // const stuff = req.body.stuff;
    console.log('features')
    res.json({
        security: false,
        project: false,
        mqtt: false,
        ntp: false,
        ota: false,
        upload_firmware: false
    });

});

app.get(VERIFY_AUTHORIZATION_ENDPOINT, (req, res) => {
    console.log('verifyAuthentication')
    res.json({
        access_token: '1234'
    });

});

app.get(NETWORK_STATUS_ENDPOINT, (req, res) => {
    console.log('networkStatus')
    res.json({
        status: 3,
        local_ip: '10.10.10.2',
        mac_address: '00:11:22:33:44',
        rssi: 12,
        ssid: "myWifi",
        bssid: "adsfds",
        channel: 3,
        submnet_mask: "255.255.255.0"
    });

});

app.get(NETWORK_SETTINGS_ENDPOINT, (req, res) => {
    console.log('networkSettings')
    res.json({
        ssid: "myWifi",
        password: 'myPassword',
        hostname: 'ems-esp',
        static_ip_config: false
    });

});

app.listen(port);
console.log(`Server listening on port::${port}`);

