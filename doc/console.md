# **Console commands**


Connecting to the console will give you more insight into the EMS bus traffic, MQTT queues and the full device information.

The console is reachable via Telnet (port 22) or via the Serial port if using an USB (on baud 115200). To change any settings in the console you must be admin (use `su` with the default password `ems-esp-neo`).
  
Some of the most common commands are:
  * `help` lists the commands and keywords. This works in each context.
  * `exit` will exit the console or exit the current context. `CTRL-D` does the same.
  * `CTRL-U` for Undo
  * `<TAB>` for auto-complete
  * Some specific commands are behind contexts. Think of this as a sub-menu. e.g. `system`, `thermostat`. The path will always show you which context you are in. `$` is the root.
  * `su` will switch to the Admin super-user. The default password is `ems-esp-neo` and can be changed with `passwd` from the system menu or via the Web interface (called secret password). When in Admin mode the command prompt switches from `$` to `#`.
  * Some settings can be changed in the console. The `set` command will list them.
  * `show` shows the data specific to the which context you're in. From the root it will show you all the EMS device information and any external temperature sensors. 
  * `log` sets the logging level. `log off` disables logging. Use `log debug` for debugging commands and actions. This will be reset next time the console is opened.
  * `watch` will output the incoming Rx telegrams directly to the console. You can also put on a watch on a specific EMS device ID or telegram ID. Also choose to output as verbose text as raw data bytes.

The `call` command is to execute a command. The command names (`[cmd]`) are the same as the MQTT commands used in MQTT.

```
(* = available in su/Admin mode)

common commands available in all contexts:
  exit
  help
  log [level]
  watch <on | off | raw> [ID]
  su

(from the root)
  system (enters a context)
  boiler (enters a context)
  thermostat (enters a context)
  set
  fetch
  scan devices [deep] *
  send telegram <"XX XX ..."> *
  set bus_id <device ID> *
  set tx_mode <n> *
  show
  show devices
  show ems
  show values
  show mqtt
  read <device ID> <type ID> *

system
  set
  show
  format *
  show users *
  passwd *
  restart *
  set wifi hostname <name> *
  set wifi password *
  set wifi ssid <name> *
  wifi reconnect *
  pin <gpio> [data] *

boiler
  read <type ID> *
  call [cmd] [data] *

thermostat
  set
  set master [device ID] *
  read <type ID> *
  call [cmd] [data] [heating circuit] *

```