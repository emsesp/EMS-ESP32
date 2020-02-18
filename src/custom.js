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
        "publish_time": 10,
        "tx_mode": 1
    }
};

function listcustom() {

    document.getElementById("led_gpio").value = custom_config.settings.led_gpio;
    document.getElementById("dallas_gpio").value = custom_config.settings.dallas_gpio;
    document.getElementById("publish_time").value = custom_config.settings.publish_time;
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
    custom_config.settings.tx_mode = parseInt(document.getElementById("tx_mode").value);

    custom_saveconfig();
}

function listCustomStats() {
    document.getElementById("msg").innerHTML = ajaxobj.emsbus.msg;
    if (ajaxobj.emsbus.ok) {
        document.getElementById("msg").className = "alert alert-success";
    } else {
        document.getElementById("msg").className = "alert alert-danger";
        document.getElementById("devicesshow").style.display = "none";
        document.getElementById("thermostat_show").style.display = "none";
        document.getElementById("boiler_show").style.display = "none";
        document.getElementById("sm_show").style.display = "none";
        document.getElementById("hp_show").style.display = "none";
        return;
    }

    var list = document.getElementById("devices");
    var obj = ajaxobj.emsbus.devices;

    document.getElementById("devicesshow").style.display = "block";

    for (var i = 0; i < obj.length; i++) {
        var l = document.createElement("li");
        var type = obj[i].type;
        var color = "";
        if (type === "Boiler") {
            color = "list-group-item-success";
        } else if (type === "Thermostat") {
            color = "list-group-item-info";
        } else if (type === "Solar Module") {
            color = "list-group-item-warning";
        } else if (type === "Heat Pump") {
            color = "list-group-item-success";
        }
        l.innerHTML = obj[i].model + " (DeviceID: 0x" + obj[i].deviceid + ", ProductID: " + obj[i].productid + ", Version: " + obj[i].version + ")";
        l.className = "list-group-item " + color;
        list.appendChild(l);
    }

    if (ajaxobj.boiler.ok) {
        document.getElementById("boiler_show").style.display = "block";

        document.getElementById("bm").innerHTML = ajaxobj.boiler.bm;
        document.getElementById("b1").innerHTML = ajaxobj.boiler.b1;
        document.getElementById("b2").innerHTML = ajaxobj.boiler.b2;
        document.getElementById("b3").innerHTML = ajaxobj.boiler.b3 + " &#8451;";
        document.getElementById("b4").innerHTML = ajaxobj.boiler.b4 + " &#8451;";
        document.getElementById("b5").innerHTML = ajaxobj.boiler.b5 + " &#8451;";
        document.getElementById("b6").innerHTML = ajaxobj.boiler.b6 + " &#8451;";
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

    if (ajaxobj.sm.ok) {
        document.getElementById("sm_show").style.display = "block";

        document.getElementById("sm").innerHTML = ajaxobj.sm.sm;
        document.getElementById("sm1").innerHTML = ajaxobj.sm.sm1 + " &#8451;";
        document.getElementById("sm2").innerHTML = ajaxobj.sm.sm2 + " &#8451;";
        document.getElementById("sm3").innerHTML = ajaxobj.sm.sm3 + " &#37;";
        document.getElementById("sm4").innerHTML = ajaxobj.sm.sm4;
        document.getElementById("sm5").innerHTML = ajaxobj.sm.sm5 + " Wh";
        document.getElementById("sm6").innerHTML = ajaxobj.sm.sm6 + " Wh";
        document.getElementById("sm7").innerHTML = ajaxobj.sm.sm7 + " KWh";
    } else {
        document.getElementById("sm_show").style.display = "none";
    }

    if (ajaxobj.hp.ok) {
        document.getElementById("hp_show").style.display = "block";

        document.getElementById("hm").innerHTML = ajaxobj.hp.hm;
        document.getElementById("hp1").innerHTML = ajaxobj.hp.hp1 + " &#37;";
        document.getElementById("hp2").innerHTML = ajaxobj.hp.hp2 + " &#37;";
    } else {
        document.getElementById("hp_show").style.display = "none";
    }


}

