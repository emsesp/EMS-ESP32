console.log("[INFO] Starting MyESP WebSocket Emulation Server");

const WebSocket = require("ws");

console.log("[INFO] You can connect to ws://localhost or load URL .../src/websrc/temp/index.html");
console.log("[INFO] Password is 'neo'");

const wss = new WebSocket.Server({
    port: 80
});

wss.broadcast = function broadcast(data) {
    wss.clients.forEach(function each(client) {
        if (client.readyState === WebSocket.OPEN) {
            client.send(JSON.stringify(data));
        }
    });
};

var networks = {
    "command": "ssidlist",
    "list": [{
        "ssid": "Company's Network",
        "bssid": "4c:f4:39:a1:41",
        "rssi": "-84"
    },
    {
        "ssid": "Home Router",
        "bssid": "8a:e6:63:a8:15",
        "rssi": "-42"
    },
    {
        "ssid": "SSID Shown Here",
        "bssid": "8a:f5:86:c3:12",
        "rssi": "-77"
    },
    {
        "ssid": "Great Wall of WPA",
        "bssid": "9c:f1:90:c5:15",
        "rssi": "-80"
    },
    {
        "ssid": "Not Internet",
        "bssid": "8c:e4:57:c5:16",
        "rssi": "-87"
    }
    ]
}

var eventlog = {
    "command": "eventlist",
    "page": 1,
    "haspages": 1,
    "list": [
        "{\"type\":\"WARN\",\"src\":\"system\",\"desc\":\"test data\",\"data\":\"Record #1\",\"time\": 1563371160}",
        "{\"type\":\"WARN\",\"src\":\"system\",\"desc\":\"test data\",\"data\":\"Record #2\",\"time\":0}",
        "{\"type\":\"INFO\",\"src\":\"system\",\"desc\":\"System booted\",\"data\":\"\",\"time\":1568660479}",
        "{\"type\":\"WARN\",\"src\":\"system\",\"desc\":\"test data\",\"data\":\"Record #3\",\"time\":0}"
    ]
}

var configfile = {
    "command": "configfile",
    "network": {
        "ssid": "myssid",
        "wmode": 0,
        "password": "password"
    },
    "general": {
        "hostname": "myesp",
        "password": "admin",
        "serial": true,
        "log_events": true
    },
    "mqtt": {
        "enabled": false,
        "ip": "10.10.10.10",
        "port": 1883,
        "qos": 1,
        "keepalive": 60,
        "retain": true,
        "base": "base",
        "user": "user",
        "password": "password",
        "heartbeat": false
    },
    "ntp": {
        "server": "pool.ntp.org",
        "interval": "30",
        "enabled": false
    }
};

var custom_configfile = {
    "command": "custom_configfile",
    "settings": {
        "led": true,
        "led_gpio": 2,
        "dallas_gpio": 14,
        "dallas_parasite": false,
        "listen_mode": false,
        "shower_timer": true,
        "shower_alert": false,
        "publish_time": 120,
        "tx_mode": 1
    }
};

function sendEventLog() {
    wss.broadcast(eventlog);
    var res = {
        "command": "result",
        "resultof": "eventlist",
        "result": true
    };
    wss.broadcast(res);
}

function sendStatus() {
    var stats = {
        "command": "status",
        "availspiffs": 948,
        "spiffssize": 957,
        "initheap": 25392,
        "heap": 13944,
        "sketchsize": 673,
        "availsize": 2469,
        "ip": "10.10.10.198",
        "ssid": "my_ssid",
        "mac": "DC:4F:11:22:93:06",
        "signalstr": 62,
        "systemload": 0,
        "mqttconnected": true,
        "mqttheartbeat": false,
        "uptime": "0 days 0 hours 1 minute 45 seconds",
        "mqttloghdr": "home/ems-esp/",
        "mqttlog": [
            { "topic": "start", "payload": "start", "time": 1565956388 },
            { "topic": "shower_timer", "payload": "1", "time": 1565956388 },
            { "topic": "shower_alert", "payload": "0", "time": 1565956388 },
            { "topic": "boiler_data", "payload": "{\"wWComfort\":\"Hot\",\"wWSelTemp\":60,\"selFlowTemp\":5,\"selBurnPow\":0,\"curBurnPow\":0,\"pumpMod\":0,\"wWCurTmp\":48.4,\"wWCurFlow\":0,\"curFlowTemp\":49.3,\"retTemp\":49.3,\"sysPress\":1.8,\"boilTemp\":50.5,\"wWActivated\":\"on\",\"burnGas\":\"off\",\"heatPmp\":\"off\",\"fanWork\":\"off\",\"ignWork\":\"off\",\"wWCirc\":\"off\",\"wWHeat\":\"on\",\"burnStarts\":223397,\"burnWorkMin\":366019,\"heatWorkMin\":294036,\"ServiceCode\":\"0H\",\"ServiceCodeNumber\":203}", "time": 1565956463 },
            { "topic": "tapwater_active", "payload": "0", "time": 1565956408 },
            { "topic": "heating_active", "payload": "0", "time": 1565956408 },
            { "topic": "thermostat_data", "payload": "{\"thermostat_hc\":\"1\",\"thermostat_seltemp\":15,\"thermostat_currtemp\":23,\"thermostat_mode\":\"auto\"}", "time": 1565956444 }
        ]
    };

    wss.broadcast(stats);
}

function sendCustomStatus() {
    var stats = {
        "command": "custom_status",
        "version": "1.9.1",
        "customname": "EMS-ESP",
        "appurl": "https://github.com/proddy/EMS-ESP",
        "updateurl": "https://api.github.com/repos/proddy/EMS-ESP/releases/latest",

        "emsbus": {
            "ok": true,
            "msg": "EMS Bus Connected with both Rx and Tx active.",
            "devices": [
                { "type": 1, "model": "Buderus GB172/Nefit Trendline/Junkers Cerapur", "version": "06.01", "productid": 123, "deviceid": "8" },
                { "type": 5, "model": "BC10 Base Controller", "version": "01.03", "productid": 190, "deviceid": "9" },
                { "type": 2, "model": "RC20/Nefit Moduline 300", "version": "03.03", "productid": 77, "deviceid": "17" },
                { "type": 3, "model": "SM100 Solar Module", "version": "01.01", "productid": 163, "deviceid": "30" },
                { "type": 4, "model": "HeatPump Module", "version": "01.01", "productid": 252, "deviceid": "38" }
            ]
        },

        "thermostat": {
            "ok": true,
            "tm": "RC20/Nefit Moduline 300",
            "ts": 15,
            "tc": 24.5,
            "tmode": "auto"
        },

        "boiler": {
            "ok": true,
            "bm": "Buderus GB172/Nefit Trendline/Junkers Cerapur",
            "b1": "off",
            "b2": "off",
            "b3": 0,
            "b4": 53,
            "b5": 54.4,
            "b6": 53.3
        },

        "sm": {
            "ok": true,
            "sm": "SM100 Solar Module",
            "sm1": 34,
            "sm2": 24,
            "sm3": 60,
            "sm4": "on",
            "sm5": 2000,
            "sm6": 3000,
            "sm7": 123456
        },

        "hp": {
            "ok": true,
            "hm": "HeatPump Module",
            "hp1": 66,
            "hp2": 77
        }

    };

    wss.broadcast(stats);
}

wss.on('connection', function connection(ws) {
    ws.on("error", () => console.log("[WARN] WebSocket Error - Assume a client is disconnected."));
    ws.on('message', function incoming(message) {
        var obj = JSON.parse(message);
        console.log("[INFO] Got Command: " + obj.command);
        switch (obj.command) {
            case "configfile":
                console.log("[INFO] New system config received");
                configfile = obj;
                break;
            case "custom_configfile":
                console.log("[INFO] New custom config received");
                custom_configfile = obj;
                break;
            case "status":
                console.log("[INFO] Sending Fake Emulator Status");
                sendStatus();
                break;
            case "custom_status":
                console.log("[INFO] Sending custom status");
                sendCustomStatus();
                break;
            case "scan":
                console.log("[INFO] Sending Fake Wireless Networks");
                wss.broadcast(networks);
                break;
            case "gettime":
                console.log("[INFO] Sending time");
                var res = {};
                res.command = "gettime";
                res.epoch = Math.floor((new Date).getTime() / 1000);
                //res.epoch = 1567107755;
                wss.broadcast(res);
                break;
            case "settime":
                console.log("[INFO] Setting time (fake)");
                var res = {};
                res.command = "gettime";
                res.epoch = Math.floor((new Date).getTime() / 1000);
                wss.broadcast(res);
                break;
            case "getconf":
                console.log("[INFO] Sending system configuration file (if set any)");
                wss.broadcast(configfile);
                break;
            case "geteventlog":
                console.log("[INFO] Sending eventlog");
                sendEventLog();
                break;
            case "clearevent":
                console.log("[INFO] Clearing eventlog");
                break;
            case "restart":
                console.log("[INFO] Restart");
                break;
            case "destroy":
                console.log("[INFO] Destroy");
                break;
            case "forcentp":
                console.log("[INFO] getting ntp time");
                break;
            default:
                console.log("[WARN] Unknown command");
                break;
        }
    });
});