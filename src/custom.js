var custom_config = {
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
        "heating_circuit": 1,
        "tx_mode": 0
    }
}

function custom_commit() {
    websock.send(JSON.stringify(custom_config));
}

function listcustom() {

    document.getElementById("led_gpio").value = custom_config.settings.led_gpio;
    document.getElementById("dallas_gpio").value = custom_config.settings.dallas_gpio;
    document.getElementById("publish_time").value = custom_config.settings.publish_time;
    document.getElementById("heating_circuit").value = custom_config.settings.heating_circuit;
    document.getElementById("tx_mode").value = custom_config.settings.tx_mode;

    if (custom_config.settings.led) {
        $("input[name=\"led\"][value=\"1\"]").prop("checked", true);
    }
    if (custom_config.settings.dallas_parasite) {
        $("input[name=\"dallas_parasite\"][value=\"1\"]").prop("checked", true);
    }
    if (custom_config.settings.listen_mode) {
        $("input[name=\"listen_mode\"][value=\"1\"]").prop("checked", true);
    }
    if (custom_config.settings.shower_timer) {
        $("input[name=\"shower_timer\"][value=\"1\"]").prop("checked", true);
    }
    if (custom_config.settings.shower_alert) {
        $("input[name=\"shower_alert\"][value=\"1\"]").prop("checked", true);
    }
}

function savecustom() {
    custom_config.settings.led_gpio = parseInt(document.getElementById("led_gpio").value);
    custom_config.settings.dallas_gpio = parseInt(document.getElementById("dallas_gpio").value);

    custom_config.settings.dallas_parasite = false;
    if (parseInt($("input[name=\"dallas_parasite\"]:checked").val()) === 1) {
        custom_config.settings.dallas_parasite = true;
    }

    custom_config.settings.listen_mode = false;
    if (parseInt($("input[name=\"listen_mode\"]:checked").val()) === 1) {
        custom_config.settings.listen_mode = true;
    }

    custom_config.settings.shower_timer = false;
    if (parseInt($("input[name=\"shower_timer\"]:checked").val()) === 1) {
        custom_config.settings.shower_timer = true;
    }

    custom_config.settings.shower_alert = false;
    if (parseInt($("input[name=\"shower_alert\"]:checked").val()) === 1) {
        custom_config.settings.shower_alert = true;
    }

    custom_config.settings.led = false;
    if (parseInt($("input[name=\"led\"]:checked").val()) === 1) {
        custom_config.settings.led = true;
    }

    custom_config.settings.publish_time = parseInt(document.getElementById("publish_time").value);
    custom_config.settings.heating_circuit = parseInt(document.getElementById("heating_circuit").value);
    custom_config.settings.tx_mode = parseInt(document.getElementById("tx_mode").value);

    custom_uncommited();
}

function listCustomStats() {
    document.getElementById("msg").innerHTML = ajaxobj.emsbus.msg;
    if (ajaxobj.emsbus.ok) {
        document.getElementById("msg").className = "alert alert-success";
    } else {
        document.getElementById("msg").className = "alert alert-danger";
        document.getElementById("thermostat_show").style.display = "none";
        document.getElementById("boiler_show").style.display = "none";
        return;
    }

    var list = document.getElementById("devices");
    var obj = ajaxobj.emsbus.devices;
    for (var i = 0; i < obj.length; i++) {
        var l = document.createElement("li");
        var type = obj[i].type;
        if (type == 1) {
            var color = "info";
        } else if (type == 2) {
            var color = "warning";
        } else {
            var color = "";
        }
        l.innerHTML = "Model:" + obj[i].model + ", Version:" + obj[i].version + ", ProductID:" + obj[i].productid + ", DeviceID:" + obj[i].deviceid;
        l.className = "list-group-item list-group-item-" + color;
        list.appendChild(l);
    }

    if (ajaxobj.boiler.ok) {
        document.getElementById("boiler_show").style.display = "block";

        document.getElementById("bm").innerHTML = ajaxobj.boiler.bm;
        document.getElementById("b1").innerHTML = ajaxobj.boiler.b1;
        document.getElementById("b2").innerHTML = ajaxobj.boiler.b2;
        document.getElementById("b3").innerHTML = ajaxobj.boiler.b3 + " &#8451;";
        document.getElementById("b4").innerHTML = ajaxobj.boiler.b4 + " &#8451;";
    } else {
        document.getElementById("boiler_show").style.display = "none";
    }

    if (ajaxobj.thermostat.ok) {
        document.getElementById("thermostat_show").style.display = "block";

        document.getElementById("tm").innerHTML = ajaxobj.thermostat.tm;
        document.getElementById("ts").innerHTML = ajaxobj.thermostat.ts + " &#8451;";
        document.getElementById("tc").innerHTML = ajaxobj.thermostat.tc + " &#8451;";
        document.getElementById("tmode").innerHTML = ajaxobj.thermostat.tmode;
    } else {
        document.getElementById("thermostat_show").style.display = "none";

    }



}

