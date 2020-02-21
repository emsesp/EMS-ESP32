var version = "";

var websock = null;
var wsUri = "ws://" + window.location.host + "/ws";
var ntpSeconds;
var ajaxobj;

var custom_config = {};

var xDown = null;
var yDown = null;

var file = {};
var backupstarted = false;
var updateurl = "";
var updateurl_dev = "";

var use_beta_firmware = false;

var myespcontent;

var formData = new FormData();

var nextIsNotJson = false;

var config = {};

function deviceTime() {
    var t = new Date(0); // The 0 there is the key, which sets the date to the epoch
    t.setUTCSeconds(ntpSeconds);
    document.getElementById("utc").innerHTML = t.toUTCString().slice(0, -3);
}

function handleNTPON() {
    document.getElementById("forcentp").style.display = "block";
}

function handleNTPOFF() {
    document.getElementById("forcentp").style.display = "none";
}

function listntp() {
    websock.send("{\"command\":\"gettime\"}");

    document.getElementById("ntpserver").value = config.ntp.server;
    document.getElementById("interval").value = config.ntp.interval;
    document.getElementById("timezone").value = config.ntp.timezone;

    if (config.ntp.enabled) {
        $("input[name=\"ntpenabled\"][value=\"1\"]").prop("checked", true);
        handleNTPON();
    } else {
        handleNTPOFF();
    }

    browserTime();
    deviceTime();
}

function home() {
    window.location = '/';
}

function restart_alert() {
    $("#commit").fadeOut(200, function () {
        $(this).css("background", "gold").fadeIn(1000);
    });
    document.getElementById("commit").innerHTML = "<h6>Settings have changed. It's recommended to reboot the system. Click here to restart.</h6>";

    $("#commit").click(function () {
        $("#reboot").modal("show");
        return false;
    });
}

function saveconfig() {
    websock.send(JSON.stringify(config));
    restart_alert();
}

function custom_saveconfig() {
    websock.send(JSON.stringify(custom_config));
    restart_alert();
}

function saventp() {
    config.ntp.server = document.getElementById("ntpserver").value;
    config.ntp.interval = parseInt(document.getElementById("interval").value);
    config.ntp.timezone = parseInt(document.getElementById("timezone").value);

    config.ntp.enabled = false;
    if (parseInt($("input[name=\"ntpenabled\"]:checked").val()) === 1) {
        config.ntp.enabled = true;
    }

    saveconfig();
}

function forcentp() {
    websock.send("{\"command\":\"forcentp\"}");
}

function savegeneral() {
    var a = document.getElementById("adminpwd").value;
    if (a === null || a === "") {
        alert("Administrator password cannot be empty");
        return;
    }
    config.general.password = a;
    config.general.hostname = document.getElementById("hostname").value;

    config.general.serial = false;
    if (parseInt($("input[name=\"serialenabled\"]:checked").val()) === 1) {
        config.general.serial = true;
    }

    config.general.log_events = false;
    if (parseInt($("input[name=\"logeventsenabled\"]:checked").val()) === 1) {
        config.general.log_events = true;
    }

    config.general.log_ip = document.getElementById("log_ip").value;

    saveconfig();
}

function savemqtt() {
    config.mqtt.enabled = false;
    if (parseInt($("input[name=\"mqttenabled\"]:checked").val()) === 1) {
        config.mqtt.enabled = true;
    }

    config.mqtt.heartbeat = false;
    if (parseInt($("input[name=\"mqttheartbeat\"]:checked").val()) === 1) {
        config.mqtt.heartbeat = true;
    }

    config.mqtt.retain = false;
    if (parseInt($("input[name=\"mqttretain\"]:checked").val()) === 1) {
        config.mqtt.retain = true;
    }

    config.mqtt.ip = document.getElementById("mqttip").value;
    config.mqtt.port = parseInt(document.getElementById("mqttport").value);
    config.mqtt.qos = parseInt(document.getElementById("mqttqos").value);
    config.mqtt.keepalive = parseInt(document.getElementById("mqttkeepalive").value);
    config.mqtt.base = document.getElementById("mqttbase").value;
    config.mqtt.user = document.getElementById("mqttuser").value;
    config.mqtt.password = document.getElementById("mqttpwd").value;

    saveconfig();
}

function savenetwork() {
    var wmode = 0;
    if (document.getElementById("inputtohide").style.display === "none") {
        var b = document.getElementById("ssid");
        config.network.ssid = b.options[b.selectedIndex].value;
    } else {
        config.network.ssid = document.getElementById("inputtohide").value;
    }

    if (document.getElementById("wmodeap").checked) {
        wmode = 1;
    }

    config.network.wmode = wmode;
    config.network.password = document.getElementById("wifipass").value;
    config.network.staticip = document.getElementById("staticip").value;
    config.network.gatewayip = document.getElementById("gatewayip").value;
    config.network.nmask = document.getElementById("nmask").value;
    config.network.dnsip = document.getElementById("dnsip").value;

    saveconfig();
}

function inProgress(callback) {
    $("body").load("myesp.html #progresscontent", function (responseTxt, statusTxt, xhr) {
        if (statusTxt === "success") {
            $(".progress").css("height", "40");
            $(".progress").css("font-size", "xx-large");
            var i = 0;
            var prg = setInterval(function () {
                $(".progress-bar").css("width", i + "%").attr("aria-valuenow", i).html(i + "%");
                i = i + 5;
                if (i === 105) {
                    clearInterval(prg);
                    var a = document.createElement("a");
                    a.href = "http://" + config.general.hostname + ".local";
                    a.innerText = "Re-connect...";
                    document.getElementById("reconnect").appendChild(a);
                    document.getElementById("reconnect").style.display = "block";
                    document.getElementById("updateprog").className = "progress-bar progress-bar-success";
                    document.getElementById("updateprog").innerHTML = "Completed";
                }
            }, 500);
            switch (callback) {
                case "upload":
                    $.ajax({
                        url: "/update",
                        type: "POST",
                        data: formData,
                        processData: false,
                        contentType: false
                    });
                    break;
                case "destroy":
                    websock.send("{\"command\":\"destroy\"}");
                    break;
                case "restart":
                    websock.send("{\"command\":\"restart\"}");
                    break;
                default:
                    break;

            }
        }
    }).hide().fadeIn();
}

function inProgressUpload() {
    $("body").load("myesp.html #progressupload", function (responseTxt, statusTxt, xhr) {
        if (statusTxt === "success") {
            $(".progress").css("height", "40");
            $(".progress").css("font-size", "xx-large");
            var i = 0;
            var prg = setInterval(function () {
                $(".progress-bar").css("width", i + "%").attr("aria-valuenow", i).html(i + "%");
                i = i + 1;
                if (i === 101) {
                    clearInterval(prg);
                    document.getElementById("updateprog").className = "progress-bar progress-bar-success";
                    document.getElementById("updateprog").innerHTML = "Completed";
                }
            }, 500);
            $.ajax({
                url: "/update",
                type: "POST",
                data: formData,
                processData: false,
                contentType: false
            });
        }
    }).hide().fadeIn();
}

function handleSTA() {
    document.getElementById("scanb").style.display = "block";
    document.getElementById("hideclient").style.display = "block";
}

function handleAP() {
    document.getElementById("ssid").style.display = "none";
    document.getElementById("scanb").style.display = "none";
    document.getElementById("hideclient").style.display = "none";

    document.getElementById("inputtohide").style.display = "block";
}

function listnetwork() {
    document.getElementById("inputtohide").value = config.network.ssid;
    document.getElementById("wifipass").value = config.network.password;
    if (config.network.wmode === 1) {
        document.getElementById("wmodeap").checked = true;
        handleAP();
    } else {
        document.getElementById("wmodesta").checked = true;
        handleSTA();
    }

    document.getElementById("staticip").value = config.network.staticip;
    document.getElementById("gatewayip").value = config.network.gatewayip;
    document.getElementById("nmask").value = config.network.nmask;
    document.getElementById("dnsip").value = config.network.dnsip;

}

function listgeneral() {
    document.getElementById("adminpwd").value = config.general.password;
    document.getElementById("hostname").value = config.general.hostname;

    if (config.general.serial) {
        $("input[name=\"serialenabled\"][value=\"1\"]").prop("checked", true);
    }

    if (config.general.log_events) {
        $("input[name=\"logeventsenabled\"][value=\"1\"]").prop("checked", true);
    }

    document.getElementById("log_ip").value = config.general.log_ip;

}

function listmqtt() {
    if (config.mqtt.enabled) {
        $("input[name=\"mqttenabled\"][value=\"1\"]").prop("checked", true);
    }

    if (config.mqtt.heartbeat) {
        $("input[name=\"mqttheartbeat\"][value=\"1\"]").prop("checked", true);
    }

    if (config.mqtt.retain) {
        $("input[name=\"mqttretain\"][value=\"1\"]").prop("checked", true);
    }

    document.getElementById("mqttip").value = config.mqtt.ip;
    document.getElementById("mqttport").value = config.mqtt.port;
    document.getElementById("mqttqos").value = config.mqtt.qos;
    document.getElementById("mqttkeepalive").value = config.mqtt.keepalive;
    document.getElementById("mqttbase").value = config.mqtt.base;
    document.getElementById("mqttuser").value = config.mqtt.user;
    document.getElementById("mqttpwd").value = config.mqtt.password;
}

function listBSSID() {
    var select = document.getElementById("ssid");
    document.getElementById("wifibssid").value = select.options[select.selectedIndex].bssidvalue;
}

function listSSID(obj) {
    var select = document.getElementById("ssid");
    for (var i = 0; i < obj.list.length; i++) {
        var x = parseInt(obj.list[i].rssi);
        var percentage = Math.min(Math.max(2 * (x + 100), 0), 100);
        var opt = document.createElement("option");
        opt.value = obj.list[i].ssid;
        opt.bssidvalue = obj.list[i].bssid;
        opt.innerHTML = "BSSID: " + obj.list[i].bssid + ", Signal Strength: %" + percentage + ", Network: " + obj.list[i].ssid;
        select.appendChild(opt);
    }
    document.getElementById("scanb").innerHTML = "Re-scan...";
    listBSSID();
}

function scanWifi() {
    websock.send("{\"command\":\"scan\"}");
    document.getElementById("scanb").innerHTML = "...";
    document.getElementById("inputtohide").style.display = "none";
    var node = document.getElementById("ssid");
    node.style.display = "inline";
    while (node.hasChildNodes()) {
        node.removeChild(node.lastChild);
    }
}

function isVisible(e) {
    return !!(e.offsetWidth || e.offsetHeight || e.getClientRects().length);
}

function colorStatusbar(ref) {
    var percentage = ref.style.width.slice(0, -1);
    if (percentage > 50) { ref.className = "progress-bar progress-bar-success"; } else if (percentage > 25) { ref.className = "progress-bar progress-bar-warning"; } else { ref.class = "progress-bar progress-bar-danger"; }
}

function listStats() {

    document.getElementById("uptime").innerHTML = ajaxobj.uptime;

    document.getElementById("heap").innerHTML = ajaxobj.heap + " bytes";
    document.getElementById("heap").style.width = (ajaxobj.heap * 100) / ajaxobj.initheap + "%";
    colorStatusbar(document.getElementById("heap"));

    document.getElementById("flash").innerHTML = ajaxobj.availsize + " KB";
    document.getElementById("flash").style.width = (ajaxobj.availsize * 100) / (ajaxobj.availsize + ajaxobj.sketchsize) + "%";
    colorStatusbar(document.getElementById("flash"));

    document.getElementById("spiffs").innerHTML = ajaxobj.availspiffs + " KB";
    document.getElementById("spiffs").style.width = (ajaxobj.availspiffs * 100) / ajaxobj.spiffssize + "%";
    colorStatusbar(document.getElementById("spiffs"));

    document.getElementById("ssidstat").innerHTML = ajaxobj.ssid;
    document.getElementById("ip").innerHTML = ajaxobj.ip;
    document.getElementById("mac").innerHTML = ajaxobj.mac;
    document.getElementById("signalstr").innerHTML = ajaxobj.signalstr + " %";
    document.getElementById("systemload").innerHTML = ajaxobj.systemload + " %";

    if (ajaxobj.mqttconnected) {
        document.getElementById("mqttconnected").innerHTML = "MQTT is connected";
        document.getElementById("mqttconnected").className = "label label-success";
    } else {
        document.getElementById("mqttconnected").innerHTML = "MQTT is not connected";
        document.getElementById("mqttconnected").className = "label label-danger";
    }

    if (ajaxobj.mqttheartbeat) {
        document.getElementById("mqttheartbeat").innerHTML = "MQTT hearbeat is enabled";
        document.getElementById("mqttheartbeat").className = "label label-success";
    } else {
        document.getElementById("mqttheartbeat").innerHTML = "MQTT hearbeat is disabled";
        document.getElementById("mqttheartbeat").className = "label label-primary";
    }

}

function getContent(contentname) {
    $("#dismiss").click();
    $(".overlay").fadeOut().promise().done(function () {
        var content = $(contentname).html();
        $("#ajaxcontent").html(content).promise().done(function () {
            switch (contentname) {
                case "#statuscontent":
                    listStats();
                    break;
                case "#backupcontent":
                    break;
                case "#ntpcontent":
                    listntp();
                    break;
                case "#mqttcontent":
                    listmqtt();
                    break;
                case "#generalcontent":
                    listgeneral();
                    break;
                case "#networkcontent":
                    listnetwork();
                    break;
                case "#customcontent":
                    listcustom();
                    break;
                case "#custom_statuscontent":
                    var version = "version " + ajaxobj.version;
                    $("#mainver").text(version);
                    $("#customname").text(ajaxobj.customname);
                    var customname2 = " " + ajaxobj.customname;
                    $("#customname2").text(customname2);

                    var elem;

                    if (config.network.wmode === 0) {
                        elem = document.getElementById("helpurl");
                        var helpurl = ajaxobj.appurl + "/wiki";
                        elem.setAttribute("href", helpurl);
                        document.getElementById("helpurl").style.display = "block";
                    } else {
                        document.getElementById("helpurl").style.display = "none";
                    }

                    elem = document.getElementById("appurl");
                    elem.setAttribute("href", ajaxobj.appurl);
                    $("#appurl2").text(ajaxobj.appurl);

                    updateurl = ajaxobj.updateurl;
                    updateurl_dev = ajaxobj.updateurl_dev;
                    listCustomStats();
                    break;
                default:
                    break;
            }
            $("[data-toggle=\"popover\"]").popover({
                container: "body"
            });
            $(this).hide().fadeIn();
        });
    });
}

function backupset() {
    var dataStr = "data:text/json;charset=utf-8," + encodeURIComponent(JSON.stringify(config, null, 2));
    var dlAnchorElem = document.getElementById("downloadSet");
    dlAnchorElem.setAttribute("href", dataStr);
    dlAnchorElem.setAttribute("download", "system_config.json");
    dlAnchorElem.click();
}

function backupCustomSet() {
    var dataStr = "data:text/json;charset=utf-8," + encodeURIComponent(JSON.stringify(custom_config, null, 2));
    var dlAnchorElem = document.getElementById("downloadCustomSet");
    dlAnchorElem.setAttribute("href", dataStr);
    dlAnchorElem.setAttribute("download", "custom_config.json");
    dlAnchorElem.click();
}

function restoreSet() {
    var input = document.getElementById("restoreSet");
    var reader = new FileReader();
    if ("files" in input) {
        if (input.files.length === 0) {
            alert("You did not select file to restore!");
        } else {
            reader.onload = function () {
                var json;
                try {
                    json = JSON.parse(reader.result);
                } catch (e) {
                    alert("Not a valid backup file!");
                    return;
                }
                if (json.command === "configfile") {
                    var x = confirm("System Config file seems to be valid, do you wish to continue?");
                    if (x) {
                        config = json;
                        saveconfig();
                    }
                }
            };
            reader.readAsText(input.files[0]);
        }
    }
}

function restoreCustomSet() {
    var input = document.getElementById("restoreCustomSet");
    var reader = new FileReader();
    if ("files" in input) {
        if (input.files.length === 0) {
            alert("You did not select file to restore!");
        } else {
            reader.onload = function () {
                var json;
                try {
                    json = JSON.parse(reader.result);
                } catch (e) {
                    alert("Not a valid backup file!");
                    return;
                }
                if (json.command === "custom_configfile") {
                    var x = confirm("Custom Config file seems to be valid, do you wish to continue?");
                    if (x) {
                        custom_config = json;
                        custom_saveconfig();
                    }
                }
            };
            reader.readAsText(input.files[0]);
        }
    }
}

function twoDigits(value) {
    if (value < 10) {
        return "0" + value;
    }
    return value;
}

function socketMessageListener(evt) {
    var obj = JSON.parse(evt.data);
    if (obj.hasOwnProperty("command")) {
        switch (obj.command) {
            case "status":
                ajaxobj = obj;
                getContent("#statuscontent");
                break;
            case "custom_settings":
                ajaxobj = obj;
                break;
            case "custom_status":
                ajaxobj = obj;
                getContent("#custom_statuscontent");
                break;
            case "gettime":
                ntpSeconds = obj.epoch;
                deviceTime();
                break;
            case "ssidlist":
                listSSID(obj);
                break;
            case "configfile":
                config = obj;
                break;
            case "custom_configfile":
                custom_config = obj;
                break;
            default:
                break;
        }
    }
}

function compareDestroy() {
    if (config.general.hostname === document.getElementById("compare").value) {
        $("#destroybtn").prop("disabled", false);
    } else { $("#destroybtn").prop("disabled", true); }
}

function destroy() {
    inProgress("destroy");
}

function restart() {
    inProgress("restart");
}

function handleTouchStart(evt) {
    xDown = evt.touches[0].clientX;
    yDown = evt.touches[0].clientY;
}

function handleTouchMove(evt) {
    if (!xDown || !yDown) {
        return;
    }

    var xUp = evt.touches[0].clientX;
    var yUp = evt.touches[0].clientY;

    var xDiff = xDown - xUp;
    var yDiff = yDown - yUp;

    if (Math.abs(xDiff) > Math.abs(yDiff)) { /*most significant*/
        if (xDiff > 0) {
            $("#dismiss").click();
        } else {
            $("#sidebarCollapse").click();
            /* right swipe */
        }
    } else {
        if (yDiff > 0) {
            /* up swipe */
        } else {
            /* down swipe */
        }
    }
    /* reset values */
    xDown = null;
    yDown = null;
}

function logout() {
    jQuery.ajax({
        type: "GET",
        url: "/login",
        async: false,
        username: "logmeout",
        password: "logmeout",
    })
        .done(function () {
            // If we don"t get an error, we actually got an error as we expect an 401!
        })
        .fail(function () {
            // We expect to get an 401 Unauthorized error! In this case we are successfully
            // logged out and we redirect the user.
            document.location = "index.html";
        });
    return false;
}

function connectWS() {
    if (window.location.protocol === "https:") {
        wsUri = "wss://" + window.location.host + "/ws";
    } else if (window.location.protocol === "file:") {
        wsUri = "ws://" + "localhost" + "/ws";
    }

    websock = new WebSocket(wsUri);
    websock.addEventListener("message", socketMessageListener);

    websock.onopen = function (evt) {
        websock.send("{\"command\":\"getconf\"}");
        websock.send("{\"command\":\"custom_status\"}");
    };
}

function upload() {
    formData.append("bin", $("#binform")[0].files[0]);
    inProgressUpload();
}

function login() {
    if (document.getElementById("password").value === "neo") {
        $("#signin").modal("hide");
        connectWS();
    } else {
        var username = "admin";
        var password = document.getElementById("password").value;
        var url = "/login";
        var xhr = new XMLHttpRequest();
        xhr.open("get", url, true, username, password);
        xhr.onload = function (e) {
            if (xhr.readyState === 4) {
                if (xhr.status === 200) {
                    $("#signin").modal("hide");
                    connectWS();
                } else {
                    alert("Incorrect password!");
                }
            }
        };
        xhr.send(null);
    }
}

function getfirmware() {
    if (use_beta_firmware) {
        use_beta_firmware = false;
        document.getElementById("updateb").innerHTML = "Switch to Development build";
    } else {
        use_beta_firmware = true;
        document.getElementById("updateb").innerHTML = "Switch to Stable release";
    }
    getLatestReleaseInfo();
}

function getLatestReleaseInfo() {

    if (use_beta_firmware) {
        var url = updateurl_dev;
    } else {
        var url = updateurl;
    }

    $.getJSON(url).done(function (release) {
        var asset = release.assets[0];
        var downloadCount = 0;
        for (var i = 0; i < release.assets.length; i++) {
            downloadCount += release.assets[i].download_count;
        }
        var oneHour = 60 * 60 * 1000;
        var oneDay = 24 * oneHour;
        var dateDiff = new Date() - new Date(release.published_at);
        var timeAgo;
        if (dateDiff < oneDay) {
            timeAgo = (dateDiff / oneHour).toFixed(1) + " hours ago";
        } else {
            timeAgo = (dateDiff / oneDay).toFixed(1) + " days ago";
        }

        var releaseInfo = release.name + " was updated " + timeAgo + " and downloaded " + downloadCount.toLocaleString() + " times.";
        $("#downloadupdate").attr("href", asset.browser_download_url);
        $("#releasehead").text(releaseInfo);
        $("#releasebody").text(release.body);
        $("#releaseinfo").fadeIn("slow");
    }).error(function () { $("#onlineupdate").html("<h5>Couldn't get release details. Make sure there is an Internet connection.</h5>"); });
}

function allowUpload() {
    $("#upbtn").prop("disabled", false);
}

function start() {
    myespcontent = document.createElement("div");
    myespcontent.id = "mastercontent";
    myespcontent.style.display = "none";
    document.body.appendChild(myespcontent);
    $("#signin").on("shown.bs.modal", function () {
        $("#password").focus().select();
    });

    $("#mastercontent").load("myesp.html", function (responseTxt, statusTxt, xhr) {
        if (statusTxt === "success") {
            $("#signin").modal({ backdrop: "static", keyboard: false });
            $("[data-toggle=\"popover\"]").popover({
                container: "body"
            });

        }
    });
}

function refreshCustomStatus() {
    websock.send("{\"command\":\"custom_status\"}");
}

function refreshStatus() {
    websock.send("{\"command\":\"status\"}");
}

$("#dismiss, .overlay").on("click", function () {
    $("#sidebar").removeClass("active");
    $(".overlay").fadeOut();
});

$("#sidebarCollapse").on("click", function () {
    $("#sidebar").addClass("active");
    $(".overlay").fadeIn();
    $(".collapse.in").toggleClass("in");
    $("a[aria-expanded=true]").attr("aria-expanded", "false");
});

$("#custom_status").click(function () { websock.send("{\"command\":\"custom_status\"}"); return false; });
$("#status").click(function () { websock.send("{\"command\":\"status\"}"); return false; });
$("#custom").click(function () { getContent("#customcontent"); return false; });
$("#network").on("click", (function () { getContent("#networkcontent"); return false; }));
$("#general").click(function () { getContent("#generalcontent"); return false; });
$("#mqtt").click(function () { getContent("#mqttcontent"); return false; });
$("#ntp").click(function () { getContent("#ntpcontent"); return false; });
$("#backup").click(function () { getContent("#backupcontent"); return false; });
$("#reset").click(function () { $("#destroy").modal("show"); return false; });
$("#restart").click(function () { $("#reboot").modal("show"); return false; });
$(".noimp").on("click", function () { $("#noimp").modal("show"); });
$("#update").on("shown.bs.modal", function (e) { getfirmware(); });

document.addEventListener("touchstart", handleTouchStart, false);
document.addEventListener("touchmove", handleTouchMove, false);
