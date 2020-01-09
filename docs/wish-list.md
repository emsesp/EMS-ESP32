The feature backlog is maintained in the GitHub Projects which you can find [here](https://github.com/proddy/EMS-ESP/projects/1).

Top of mind, these are the new features when I plan to address:

- Measure the amount of gas in m3 per day for the hot water vs the central heating, and convert this into cost.
- Support changing temperatures on an Nefit Easy. To do this we must send XMPP messages directly to the thermostat. There is already a TCP stack and a Wifi and Telnet server running in the code, so the building blocks are there to extend with another XMPP client. Here are a number of Python based projects that show how to do this:
  - https://github.com/patvdleer/nefit-client-python
  - https://github.com/marconfus/ha-nefit
  - https://github.com/robertklep/nefit-easy-core
- Improve detection of Heating Off without checking for selFlowTemp (selected flow temperature)
- Split MQTT into smaller chunks. Now the messages can be up to 600 bytes which may cause issues. Preferably make the topic items configurable.