This folder contains the default data used when testing in standalone mode.

It is used for simulation and testing and can be invoked by compiling with -DEMSESP_TEST and from the Console using the command `test general` or via an API call like http://ems-esp.local/api?device=system&cmd=test&data=general

To run in standalone mode without an ESP32 microcontroller use `make run` or `pio run -e standalone -t exec`

