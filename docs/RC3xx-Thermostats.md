### How to interpret the tables below

Take an example telegram:
```
         OO TTTTT
10 00 FF 00 01 A5 00 D7 21 00 00 00 00 30 01 84 01 01 03 01 84 01 F1 00 00 11 01 00 08 63 00
   data block --> 00 01 02 03 04 06 07 08 09 0A...
                  ^
                  | 
             start of data (n=0)

OO = offset, 1 byte
TTTTT = type, 2 bytes
```

The data starts at the 7th byte straight after the typeID of `x01A5`. Position 0 is the first byte of the data block.

### Reading from and Writing to an EMS+ Telegram

```
Reading 2 bytes of data from 0x1A5 its: send 0B 90 FF 00 02 01 A5
Meaning:
0B - sender
90 - receiver (RC300) (Read request)
FF - EMS+ extended mode
00 - offset
02 - data length to query
01 A5 - EMS+ Command/Type

And writing a value of 0x77 to type 0x1B9 its: send 0B 10 FF 08 01 B9 77

Meaning:
0B - sender
10 - receiver (RC300) (Write request)
FF - EMS+ extended mode
08 - offset
01 B9 - EMS+ Command/Type
77 - the value to write
```

example to retrieve 25 bytes of data from HC1:

```
send 0B 90 FF 00 19 01 A5
Thermostat -> me, type 0x01A5, telegram: 10 0B FF 00 01 A5 00 D3 21 22 00 00 22 27 00 EF 01 01 03 00 EF 01 4B 00 00 11 01 04 08 42 00 (CRC=ED) #data=25
```



# Type 01A5

0x1A5 is a monitoring telegram from the 1st Heat Controller (HC1). The other HCs use `01A6`, `01A7` and `01A8` respectively and share the same format as below.

| data position (0..n) in decimal | description | format | notes | 
| - | - | - | - |
| 0 + 1 | current room temperature | 2 bytes, *10 |
| 2 | ?? (room night set point temperature) | 1 byte, *2 | **(needs verifying!)** most probably to be processed as bit(s) known values are always in (03, 21 ,23)|
| 3 | current target temperature | 1 byte, *2 | current target value |
| 4 | target flow temperature | 1 byte | target flow temp  |
| 6 | current setpoint temperature | 1 byte, *2 | temperature for current mode |
| 7 | next setpoint temperature | 1 byte, *2 | note, only when in auto mode |
| 8 | remaining time until next temp change | 2 bytes | remaining time in minutes.|
| 10 | mode and eco/comfort bits info | 1 byte| bit 1 : 0=manual 1=mode auto, bit 2 : 0=night 1=comfort2 |
| 11 | Current temperature mode | 1 byte  | eco : 1, comfort1 : 2, comfort2 : 3, comfort3 : 4 |
| 12 | Next temperature mode | 1 byte | eco : 1, comfort1 : 2, comfort2 : 3, comfort3 : 4 |
| 13 + 14 | remaining time to next setpoint | 2 bytes | time in minutes.|
| 15 + 16 | elapsed time in this setpoint | 2 bytes | time in minutes. | 


# Type 01B9

This telegram type is for managing the Heating mode 

```
AUTO Mode, setting temporary temp to 21.5(0x2B)
(03:43:45.965) Gateway -> Thermostat, type 0x01B9 telegram: 48 10 FF 08 01 B9 2B (CRC=FA)
(03:43:46.445) Thermostat -> all, type 0x01B9 telegram: 10 00 FF 08 01 B9 2B (CRC=17)

SET to Manual mode ( manual temp is 20.5 (0x29) )
(03:49:44.575) Gateway -> Thermostat, type 0x01B9 telegram: 48 10 FF 00 01 B9 00 (CRC=91)
(03:49:45.825) Thermostat -> all, type 0x01A5 telegram: 10 00 FF 0A 01 A5 02 (CRC=16)
(03:49:51.726) Thermostat -> all, type 0x01A5 telegram: 10 00 FF 03 01 A5 29 (CRC=75)
<--- RCPLUSStatusMessage(0x1A5) received
 (03:49:51.918) Thermostat -> all, type 0x01A5 telegram: 10 00 FF 06 01 A5 29 (CRC=5D)
<--- RCPLUSStatusMessage(0x1A5) received

```

| data position (0..n) in decimal | description | format | notes | 
| - | - | - | - |
| 0 | operation mode | 1 byte | (Auto=xFF, Manual=x00) |
| 1 | comfort3 level | 1 byte, *2 |  |
| 2 | comfort2 level | 1 byte, *2 |  |
| 3 | comfort1 level | 1 byte, *2 |  |
| 4 | eco level  | 1 byte, *2 |  |
| 5 |  |  |  |                                         
| 6 |  |  |  |                                          
| 7 |  |  |  |                                           
| 8 | temporary setpoint temperature  | 1 byte, *2 | when changing of templevel (in auto) value is reset and set to FF  |
| 9 |  |  |  |                                           
| 10 | manual setpoint temperature  | 1 byte, *2 |  |

# Type 01AF 

Summer - Winter Mode 

change the mode 
```send 0B 10 FF 00 AF 07 02```

query the mode 
```send 0B 90 FF 00 AF 07 01``` 

1 Byte value.

|Byte value | Meaning | 
| - | - |
| 00 | Off |
| 01 | Automatic |
| 02 | Forced |

Data log example :
```
Thermostat -> all, type 0x01AF telegram: 10 00 FF 07 01 AF 00 (Off)
Thermostat -> all, type 0x01AF telegram: 10 00 FF 07 01 AF 01 (Automatic)
Thermostat -> all, type 0x01AF telegram: 10 00 FF 07 01 AF 02 (Forced)
```


