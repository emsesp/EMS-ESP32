The various types from newer EMS+ controllers like the MC110 found in GB122/125's.

Byte 1 is the 1st byte of the data block (which is the 5th byte of the telegram for EMS 1.0 for example).

Bit 0 is the first byte (so value & 0x01)

This information was compiled with most if it coming from https://github.com/Th3M3/buderus_ems-wiki. Kudos to @Th3M3 for collecting this.

## 0xD1: UBAOutdoorTemp

Sent every minute. The first 2 bytes is a short value * 10

```
1 - 2 = outdoor temp, short value * 10
```

## 0x14: UBABetriebszeit

```
1 - 2 = total operating time in minutes
```

## 0xBF: UBAErrorMessage

Sent every 10 minutes

```
1 = 0x08
2 = Model ID, 0x85 for MC110/GB125, 0xEA for GB122
3 = blocking error: always 0x0A (fault class, see service manual)
4 = blocking error: 0x00 or 0x10 = interlocking error, 0x11 = blocking error, 0x12 = system error, 0x13 = reset system error
5 = ASCII | Fault code 0. character (possibly 0x0a = space)
6 = ASCII | Fault code (operation code) 1st character
7 = ASCII | Fault code (operation code) 2nd character
8 - 9 = additional code (status code)
```

## 0xE3: UBA??

Sent every 10 seconds

```
3 = heat request. is 1 if byte 1/bit 1 is set, otherwise 0
6 = bit 1 for warming water yes/no
12 - 17 = boiler actual temperature * 10 C
14 = Burner capacity-actual in %
15 = Always 0x64 (100%)
16 = Boiler max. temperature in C
19 = %, is 0x64 (100%) if byte 1/bit 1 is set to 1. Otherwise 0.
20 = Cool set temperature for DHW preparation in C
```

## 0xE4: UBAMonitorFast

Sent every 3 seconds

```
3 =  operation code 1st character
4 = operation code 2nd character
5 - 6 = status code, as numerical value
7 = boiler set temp/heater temp in C
8 - 9 = actual boiler temp * 10
10 = 0x64 (= 100%) in heating mode, 0x00 in DHW preparation, as %
11 = Burner capacity/power actual as %
12 = bit 0 = flame yes/no, bit 1 = day yes/no, bit 2 = WW prep yes/no
20 - 21 = flame current * 10 in uA
24 - 25 = same as bytes 8-9
```

## 0xE5: UBAMonitorSlow

Sent every 60 seconds

```
3 =
 bit 0 = fuel valve 1 open
 bit 1 = fuel valve 2 open
 bit 2 = blower on
 bit 3 = ignition on
 bit 4 = oil preheater on
 bit 5 = boiler circuit pump on
 bit 6 = 3-way valve on WW
 bit 7 = circulating pump on
9 = bit 0 = fire control started
11 - 13 = total burner starts
14 - 16 = burner operating hours
17 - 19 = burner level 2 operating hours
20 - 22 = Burner operating hours due to heating demand (excluding DHW preparation)
23 - 25 = Burner starting by heat demand (excluding WW-preparation)
26 = Heating circuit pump modulation %
```

## Type 0xE9: DHW status
  
Sent every 10 seconds

```
1 = DHW setpoint (with setpoint = "off": 0x0A = 10 ° C)
2 - 3 = hot water actual value * 10
4 - 5 = hot water actual value * 10
10 = setpoint thermal disinfection
11 = setpoint daily heating temperature
13 = 
 bit 0 : DHW preparation activated by automatic program
 bit 2 : DHW preparation activated by one-time charge
 bit 3 : DHW preparation activated by thermal disinfection
 bit 4 : DHW preparation active (always 1 if bit 0, 2 or 3 also 1)
14 =
 bit 0 : Circulation day mode (or by single charge / th. disinfection activated)
 bit 2 : circulating pump on
 bit 4 : WW-preparation (when switching to daytime operation sometimes short 0, then again 1)
 bit 5 : WW preparation
15 - 17 = Warmwater preparations
18 - 20 = # hours water heating
21 = Warming water (0x64 = 100%)
24 = Set value of time program (if set = "off": 0x00)
26 = same as byte 1, in C
```

## 0x07E4: UBA Status

Sent every 10 seconds

```
6 = operation code 1st character
7 = operation code 2nd character
10 = Boiler setpoint temperature during heating operation (0x00 with DHW preparation)
11 = 0x64 (= 100%) in heating mode, 0x00 in DHW preparation or when night
12 = Boiler setpoint temperature during DHW preparation (0x00 during heating operation or when night)
```
