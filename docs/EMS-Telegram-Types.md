> [!TIP]
> Another good source of EMS and EMS+ investigation look at https://github.com/Th3M3/buderus_ems-wiki_

## A2

This message is a EMS 1.0 message 

Display code and Cause

```
(00:28:29.966) Thermostat -> all, type 0xA2 telegram: 10 00 A2 00 00 00 00 00 00 (CRC=CA) #data=5
```


| data position (0..n) in decimal | description | format | notes | 
| - | - | - | - |
| 0-2 | Display code | 3 bytes | HEX To Decimal |
| 3-4 | Cause Code | 2 bytes | HEX To Decimal |

## 3E, 48, 52, 5C

These are EMS1.0 type broadcasted from an RC35 compatible thermostat (typically with a device ID of 0x10) as a Heating Controller Monitor Message. The data block is 16 bytes. We use this telegram to read the current room temperature, the setpoint temperature for up to 4 Heating Controllers 0x3E (HC1), 0x48 (HC2), 0x52 (HC3) and 0x5C (HC4)

Examples:
```
<  header   > < 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15> (data block)
 10 0B 3E 00   00 00 00 7D 00 00 00 00 00 00 00 00 00 11 05 04
 10 00 48 00   00 00 00 00 E4 00 00 00 00 00 00 00 00 11 05 04
 10 0B 52 00   00 00 00 00 00 00 00 00 00 00 00 00 00 11 00 00
 10 0B 5C 00   00 00 00 00 00 00 00 00 00 00 00 00 00 11 00 00
```
in night mode:
```
 10 00 3E 00   04 03 00 7D 00 00 00 00 00 00 00 00 00 11 05 00
 10 00 48 00   00 00 10 00 E9 00 00 00 00 00 00 00 00 11 05 00
```
in day mode:
```
 10 0B 3E 00   04 03 00 7D 00 00 00 00 00 00 00 00 00 11 05 00
 10 0B 48 00   00 00 10 00 E8 00 00 00 00 00 00 00 00 11 05 00
```
in auto day mode:
```
 10 00 3E 00   04 03 00 7D 00 00 00 00 00 00 00 00 00 11 05 00
 10 0B 48 00   04 03 00 00 EB 00 00 00 00 00 00 00 00 11 05 00
```

deciphering each data element from 0 to 15

| data #n | description | format | notes | 
| - | - | - | - |
| 0 | settings1 | 1 byte as a bit mask | bit 2 is automatic mode, bit 7 is manual mode, bit 5 is holiday mode | doesn't seem to make sense? |
| 1 | settings2 | 1 byte as a bit mask | bit 0 if in summer mode, bit 1 if in daytime | needs verifying |
| 2 | setpoint room temperature | 1 byte * 2 | |
| 3 + 4 | current room temperature | 2 bytes * 10 | has value 0x7D00 if switched off |
| 13 | switch state | 1 byte as a bit mask | always 11? |
| 14 | calculate setpoint flow temperature | 1 byte | this looks wrong |
| 15 | settings3 | 1 byte as a bit mask | we see two values 0 and 4 |
 
Questions: 

* how to determine if a heating circuit is not active? Check for data #14 if its 0 ?
* when to ignore the setpoint temperature being 0? This depends on the bit masks in data #1 and #2 somehow
* data #0 is sometimes 04 (b0100) always with data#1 being 3 (b0011). This means DWH priority is set and daytime mode, according to https://emswiki.thefischer.net/doku.php?id=wiki:ems:telegramme#wwbetriebsart. Is this correct since one of the examples we're in nightmode?


