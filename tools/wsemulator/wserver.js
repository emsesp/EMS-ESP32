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
        "{ \"type\": \"WARN\", \"src\": \"sys\", \"desc\": \"Event log cleared!\", \"data\": \"\", \"time\": 1563371160 }",
        "{ \"type\": \"WARN\", \"src\": \"sys\", \"desc\": \"Event log cleared!\", \"data\": \"\", \"time\": 1563371160 }",
        "{ \"type\": \"INFO\", \"src\": \"wifi\", \"desc\": \"WiFi is connected\", \"data\": \"SMC\", \"time\": 13 }",
        "{ \"type\": \"INFO\", \"src\": \"sys\", \"desc\": \"System setup completed, running\", \"data\": \"\", \"time\": 13 }",
        "{ \"type\": \"INFO\", \"src\": \"wifi\", \"desc\": \"WiFi is connected\", \"data\": \"SMC\", \"time\": 13 }",
        "{ \"type\": \"INFO\", \"src\": \"sys\", \"desc\": \"System setup completed, running\", \"data\": \"\", \"time\": 13 }",
        "{ \"type\": \"WARN\", \"src\": \"websrv\", \"desc\": \"New login attempt\", \"data\": \"\", \"time\": 1563371160 }",
        "{ \"type\": \"INFO\", \"src\": \"websrv\", \"desc\": \"Login success!\", \"data\": \"\", \"time\": 1563371160 }",
        "{ \"type\": \"INFO\", \"src\": \"wifi\", \"desc\": \"WiFi is connected\", \"data\": \"SMC\", \"time\": 13 }",
        "{ \"type\": \"INFO\", \"src\": \"sys\", \"desc\": \"System setup completed, running\", \"data\": \"\", \"time\": 13 }",
        "{ \"type\": \"WARN\", \"src\": \"websrv\", \"desc\": \"New login attempt\", \"data\": \"\", \"time\": 1563371160 }"
    ]
}

var configfile = {
    "command": "configfile",
    "network": {
        "ssid": "myssid",
        "wmode": "0",
        "password": "password"
    },
    "general": {
        "hostname": "myesp",
        "password": "admin",
        "serial": true
    },
    "mqtt": {
        "enabled": false,
        "ip": "ip",
        "port": "port",
        "base": "base",
        "user": "user",
        "password": "password",
        "heartbeat": false
    },
    "ntp": {
        "server": "pool.ntp.org",
        "interval": "30",
        "timezone": "0",
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
        "shower_timer": false,
        "shower_alert": false,
        "publish_time": 120,
        "heating_circuit": 1
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
        "heap": 30,
        "availsize": 555,
        "availspiffs": 445,
        "spiffssize": 888,
        "sketchsize": 222,
        "uptime": "1 Day 6 Hours",
        "ssid": "SSID",
        "mac": "EM:44:11:33:22",
        "ip": "192.168.2.2",
        "signalstr": 66,
        "systemload": 10,
        "mqttconnected": true,
        "mqttheartbeat": false
    };
    wss.broadcast(stats);
}

function sendCustomStatus() {
    var stats = {
        "command": "custom_status",
        "version": "1.9.0b",
        "customname": "ems-esp",
        "helpurl": "https://github.com/proddy/EMS-ESP/wiki",
        "updateurl": "https://api.github.com/repos/proddy/EMS-ESP/releases/latest",

        "emsbus": {
            "ok": true,
            "msg": "everything is OK",
            "devices": [
                { "type": 1, "model": "model 1", "deviceid": "device id1", "version": "version id1", "productid": "product id1" },
                { "type": 2, "model": "model 2", "deviceid": "device id2", "version": "version id2", "productid": "product id2" },
                { "type": 3, "model": "model 3", "deviceid": "device id3", "version": "version id3", "productid": "product id3" },
                { "type": 4, "model": "model 4", "deviceid": "device id3", "version": "version id3", "productid": "product id3" },
                { "type": 5, "model": "model 5", "deviceid": "device id3", "version": "version id3", "productid": "product id3" }

            ]
        },

        "thermostat": {
            "ok": true,
            "tm": "model abc",
            "ts": "23",
            "tc": "27.5",
            "tmode": "manual"
        },

        "boiler": {
            "ok": true,
            "bm": "mode boiler",
            "b1": "on",
            "b2": "off",
            "b3": 5.8,
            "b4": 61.5
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
                console.log("[INFO] New system settings file received");
                configfile = obj;
                break;
            case "custom_configfile":
                console.log("[INFO] New custom config file received");
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
                res.timezone = configfile.timezone;
                wss.broadcast(res);
                break;
            case "settime":
                console.log("[INFO] Setting time (fake)");
                var res = {};
                res.command = "gettime";
                res.epoch = Math.floor((new Date).getTime() / 1000);
                res.timezone = configfile.timezone;
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
            default:
                console.log("[WARN] Unknown command ");
                break;
        }
    });
});