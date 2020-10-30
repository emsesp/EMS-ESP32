"""
<plugin key="nefit" name="Nefit EMS-ESP with Proddy firmware" version="0.0.1">
    <description>
      Plugin to control Nefit EMS-ESP with '<a href="https://github.com/proddy/EMS-ESP"> Proddy</a>' firmware<br/>
      <br/>
      Automatically creates Domoticz devices for connected device.<br/>
	  Do not forget to "Accept new Hardware Devices" on first run<br/>
    </description>
    <params>
        <param field="Address" label="MQTT Server address" width="300px" required="true" default="127.0.0.1"/>
        <param field="Port" label="Port" width="300px" required="true" default="1883"/>
        <param field="Mode6" label="Debug" width="75px">
            <options>
                <option label="Extra verbose" value="Verbose+"/>
                <option label="Verbose" value="Verbose"/>
                <option label="True" value="Debug"/>
                <option label="False" value="Normal" default="true" />
            </options>
        </param>
    </params>
</plugin>
"""

import Domoticz
import json
import time
from mqtt import MqttClient

class Thermostat:
    def checkDevices(self):
        if 1 not in Devices:
                Domoticz.Debug("Create Temperature Device")
                Domoticz.Device(Name="Woonkamer", Unit=1, Type=80, Subtype=5).Create()
        if 2 not in Devices:
                Domoticz.Debug("Create System Pressure Device")
                Domoticz.Device(Name="System Pressure", Unit=2, Type=243, Subtype=9).Create()
        if 3 not in Devices:
                Domoticz.Debug("Create Thermostat Device")
                Domoticz.Device(Name="Nefit", Unit=3, Type=242, Subtype=1).Create()

    def onMqttMessage(self, topic, payload):
        if "sysPress" in payload:
                pressure=payload["sysPress"]
                Domoticz.Debug("System Pressure: {}".format(pressure))
                Devices[2].Update(nValue=1, sValue=str(pressure))

        if "currtemp" in payload:
                temp=round(float(payload["currtemp"]),1)
                Domoticz.Debug("Current temp: {}".format(temp))
                if Devices[1].sValue != temp:
                        Devices[1].Update(nValue=1, sValue=str(temp))
        if "seltemp" in payload:
                temp=payload["seltemp"]
                Domoticz.Debug("Temp setting: {}".format(temp))
                if Devices[3].sValue != temp:
                        Devices[3].Update(nValue=1, sValue=str(temp))

    def onCommand(self, mqttClient, unit, command, level, color):
        topic = "ems-esp/thermostat_cmd"
        if (command == "Set Level"):
	    #MQTT command syntax for setting the temperature: {"cmd":"temp", "data":<temp>, "hc":1}
            cmdstr = chr(123) + chr(34) + "cmd" +chr(34) + ":" + chr(34) + "temp" + chr(34) + ", "	#that's {"cmd":"temp", 
            cmdstr = cmdstr + chr(34) + "data" + chr(34) + ":" + str(level) +", "			#that adds "data":<temp>, 
            cmdstr = cmdstr + chr(34) + "hc" + chr(34) + ":1" + chr(125)				#that adds "hc":1}
            mqttClient.Publish(topic, cmdstr)

class BasePlugin:
    mqttClient = None

    def onStart(self):
        self.debugging = Parameters["Mode6"]
        
        if self.debugging == "Verbose+":
            Domoticz.Debugging(2+4+8+16+64)
        if self.debugging == "Verbose":
            Domoticz.Debugging(2+4+8+16+64)
        if self.debugging == "Debug":
            Domoticz.Debugging(2+4+8)

        self.controller = Thermostat()

        self.controller.checkDevices()

        self.topics = list(["ems-esp/thermostat_data1", "ems-esp/boiler_data", "ems-esp/STATE"])
        self.mqttserveraddress = Parameters["Address"].replace(" ", "")
        self.mqttserverport = Parameters["Port"].replace(" ", "")
        self.mqttClient = MqttClient(self.mqttserveraddress, self.mqttserverport, self.onMQTTConnected, self.onMQTTDisconnected, self.onMQTTPublish, self.onMQTTSubscribed)

    def checkDevices(self):
        Domoticz.Log("checkDevices called")

    def onStop(self):
        Domoticz.Log("onStop called")

    def onCommand(self, Unit, Command, Level, Color):
        Domoticz.Debug("Command: " + Command + " (" + str(Level))
        self.controller.onCommand(self.mqttClient, Unit, Command, Level, Color)

    def onConnect(self, Connection, Status, Description):
        self.mqttClient.onConnect(Connection, Status, Description)

    def onDisconnect(self, Connection):
        self.mqttClient.onDisconnect(Connection)

    def onMessage(self, Connection, Data):
        self.mqttClient.onMessage(Connection, Data)

    def onHeartbeat(self):
        Domoticz.Debug("Heartbeating...")

        # Reconnect if connection has dropped
        if self.mqttClient.mqttConn is None or (not self.mqttClient.mqttConn.Connecting() and not self.mqttClient.mqttConn.Connected() or not self.mqttClient.isConnected):
            Domoticz.Debug("Reconnecting")
            self.mqttClient.Open()
        else:
            self.mqttClient.Ping()

    def onMQTTConnected(self):
        Domoticz.Debug("onMQTTConnected")
        self.mqttClient.Subscribe(self.topics)

    def onMQTTDisconnected(self):
        Domoticz.Debug("onMQTTDisconnected")

    def onMQTTSubscribed(self):
        Domoticz.Debug("onMQTTSubscribed")

    def onMQTTPublish(self, topic, rawmessage):
        Domoticz.Debug("MQTT message: " + topic + " " + str(rawmessage))

        message = ""
        try:
            message = json.loads(rawmessage.decode('utf8'))
        except ValueError:
            message = rawmessage.decode('utf8')

        if (topic in self.topics):
            self.controller.onMqttMessage(topic, message)

global _plugin
_plugin = BasePlugin()

def onStart():
    global _plugin
    _plugin.onStart()

def onStop():
    global _plugin
    _plugin.onStop()
    
def onConnect(Connection, Status, Description):
    global _plugin
    _plugin.onConnect(Connection, Status, Description)

def onDisconnect(Connection):
    global _plugin
    _plugin.onDisconnect(Connection)

def onMessage(Connection, Data):
    global _plugin
    _plugin.onMessage(Connection, Data)

def onCommand(Unit, Command, Level, Color):
    global _plugin
    _plugin.onCommand(Unit, Command, Level, Color)

def onHeartbeat():
    global _plugin
    _plugin.onHeartbeat()
