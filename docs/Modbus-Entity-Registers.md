<!-- Use full browser width for this page, the tables are wide -->
<style>
.md-grid {
    max-width: 100%; /* or 100%, if you want to stretch to full-width */
}
</style>

# Entity/Register Mapping

!!! note

    This file has been auto-generated. Do not modify.

## Devices of type *alert*
### EM10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| setflowtemp | set flow temperature | uint8 (>=0<=254) | C | false | DEVICE_DATA | 0 | 1 | 1 | 
| setburnpow | burner set power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 1 | 1 | 1 | 

## Devices of type *boiler*
### BK13/BK15, Smartline, GB1*2
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### C1200W
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### CS*800i, Logatherm WLW*
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=167772) | kWh | false | DHW | 0 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrgcool | energy cooling | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 83 | 2 | 1/100 | 
| metertotal | meter total | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 | 
| metercomp | meter compressor | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 87 | 2 | 1/100 | 
| metereheat | meter e-heater | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 89 | 2 | 1/100 | 
| meterheat | meter heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 91 | 2 | 1/100 | 
| metercool | meter cooling | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 93 | 2 | 1/100 | 
| meter | meter | uint24 (>=0<=167772) | kWh | false | DHW | 2 | 2 | 1/100 | 
| uptimetotal | heatpump total uptime | time (>=0<=279620) | minutes | false | DEVICE_DATA | 95 | 2 | 1/60 | 
| uptimecontrol | total operating time heat | time (>=0<=279620) | minutes | false | DEVICE_DATA | 97 | 2 | 1/60 | 
| uptimecompheating | operating time compressor heating | time (>=0<=279620) | minutes | false | DEVICE_DATA | 99 | 2 | 1/60 | 
| uptimecompcooling | operating time compressor cooling | time (>=0<=279620) | minutes | false | DEVICE_DATA | 101 | 2 | 1/60 | 
| uptimecomp | operating time compressor | time (>=0<=279620) | minutes | false | DHW | 4 | 2 | 1/60 | 
| uptimecomppool | operating time compressor pool | time (>=0<=279620) | minutes | false | DEVICE_DATA | 103 | 2 | 1/60 | 
| totalcompstarts | total compressor control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 105 | 2 | 1 | 
| heatingstarts | heating control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 107 | 2 | 1 | 
| coolingstarts | cooling control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 109 | 2 | 1 | 
| startshp | starts hp | uint24 (>=0<=16777214) |   | false | DHW | 6 | 2 | 1 | 
| poolstarts | pool control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 111 | 2 | 1 | 
| nrgconstotal | total energy consumption | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 113 | 2 | 1 | 
| nrgconscomptotal | total energy consumption compressor | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 115 | 2 | 1 | 
| nrgconscompheating | energy consumption compressor heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 117 | 2 | 1 | 
| nrgconscomp | energy consumption compressor | uint24 (>=0<=16777214) | kWh | false | DHW | 8 | 2 | 1 | 
| nrgconscompcooling | energy consumption compressor cooling | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 119 | 2 | 1 | 
| nrgconscomppool | energy consumption compressor pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 121 | 2 | 1 | 
| auxelecheatnrgconstotal | total aux elec. heater energy consumption | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 123 | 2 | 1 | 
| auxelecheatnrgconsheating | aux elec. heater energy consumption heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 125 | 2 | 1 | 
| auxelecheatnrgcons | aux elec. heater energy consumption | uint24 (>=0<=16777214) | kWh | false | DHW | 10 | 2 | 1 | 
| auxelecheatnrgconspool | aux elec. heater energy consumption pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 127 | 2 | 1 | 
| nrgsupptotal | total energy supplied | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 129 | 2 | 1 | 
| nrgsuppheating | total energy supplied heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 131 | 2 | 1 | 
| nrgsupp | total energy warm supplied | uint24 (>=0<=16777214) | kWh | false | DHW | 12 | 2 | 1 | 
| nrgsuppcooling | total energy supplied cooling | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 133 | 2 | 1 | 
| nrgsupppool | total energy supplied pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 135 | 2 | 1 | 
| hppower | compressor power output | uint8 (>=0<=25) | kW | false | DEVICE_DATA | 137 | 1 | 1/10 | 
| hpmaxpower | compressor max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 138 | 1 | 1 | 
| pvmaxcomp | pv compressor max power | uint8 (>=0<=25) | kW | true | DEVICE_DATA | 139 | 1 | 1/10 | 
| powerreduction | power reduction | uint8 (>=30<=60) | % | true | DEVICE_DATA | 140 | 1 | 10 | 
| hpsetdiffpress | set differential pressure | uint8 (>=150<=750) | mbar | true | DEVICE_DATA | 141 | 1 | 50 | 
| hpcompon | hp compressor | boolean |   | false | DEVICE_DATA | 142 | 1 | 1 | 
| hpactivity | compressor activity | enum [off\|heating\|cooling\|hot water\|pool\|pool heating\|defrost\|compressor alarm] |   | false | DEVICE_DATA | 143 | 1 | 1 | 
| hpbrinepumpspd | brine pump speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 144 | 1 | 1 | 
| hpswitchvalve | switch valve | boolean |   | false | DEVICE_DATA | 145 | 1 | 1 | 
| hpcompspd | compressor speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 146 | 1 | 1 | 
| hpcircspd | circulation pump speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 147 | 1 | 1 | 
| hpbrinein | brine in/evaporator | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 148 | 1 | 1/10 | 
| hpbrineout | brine out/condenser | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 149 | 1 | 1/10 | 
| hptc0 | heat carrier return (TC0) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 150 | 1 | 1/10 | 
| hptc1 | heat carrier forward (TC1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 151 | 1 | 1/10 | 
| hptc3 | condenser temperature (TC3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 152 | 1 | 1/10 | 
| hptr1 | compressor temperature (TR1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 153 | 1 | 1/10 | 
| hptr3 | refrigerant temperature liquid side (condenser output) (TR3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 154 | 1 | 1/10 | 
| hptr4 | evaporator inlet temperature (TR4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 155 | 1 | 1/10 | 
| hptr5 | compressor inlet temperature (TR5) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 156 | 1 | 1/10 | 
| hptr6 | compressor outlet temperature (TR6) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 157 | 1 | 1/10 | 
| hptr7 | refrigerant temperature gas side (condenser input) (TR7) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 158 | 1 | 1/10 | 
| hptl2 | air inlet temperature (TL2) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 159 | 1 | 1/10 | 
| hppl1 | low pressure side temperature (PL1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 160 | 1 | 1/10 | 
| hpph1 | high pressure side temperature (PH1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 161 | 1 | 1/10 | 
| hpta4 | drain pan temp (TA4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 162 | 1 | 1/10 | 
| hptw1 | reservoir temp (TW1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 163 | 1 | 1/10 | 
| poolsettemp | pool set temperature | uint8 (>=0<=127) | C | true | DEVICE_DATA | 164 | 1 | 1/2 | 
| hp4way | 4-way valve (VR4) | enum [cooling & defrost\|heating & dhw] |   | false | DEVICE_DATA | 165 | 1 | 1 | 
| hpin1 | input 1 state | boolean |   | false | DEVICE_DATA | 166 | 1 | 1 | 
| hpin1opt | input 1 options | string |   | true | DEVICE_DATA | 167 | 8 | 1 | 
| hpin2 | input 2 state | boolean |   | false | DEVICE_DATA | 175 | 1 | 1 | 
| hpin2opt | input 2 options | string |   | true | DEVICE_DATA | 176 | 8 | 1 | 
| hpin3 | input 3 state | boolean |   | false | DEVICE_DATA | 184 | 1 | 1 | 
| hpin3opt | input 3 options | string |   | true | DEVICE_DATA | 185 | 8 | 1 | 
| hpin4 | input 4 state | boolean |   | false | DEVICE_DATA | 193 | 1 | 1 | 
| hpin4opt | input 4 options | string |   | true | DEVICE_DATA | 194 | 8 | 1 | 
| maxheatcomp | heat limit compressor | enum [0 kW\|2 kW\|3 kW\|4 kW\|6 kW\|9 kW] |   | true | DEVICE_DATA | 202 | 1 | 1 | 
| maxheatheat | heat limit heating | enum [0 kW\|2 kW\|3 kW\|4 kW\|6 kW\|9 kW] |   | true | DEVICE_DATA | 203 | 1 | 1 | 
| maxheat | heat limit | enum [0 kW\|2 kW\|3 kW\|4 kW\|6 kW\|9 kW] |   | true | DHW | 14 | 1 | 1 | 
| mandefrost | manual defrost | boolean |   | true | DEVICE_DATA | 204 | 1 | 1 | 
| pvcooling | cooling only with PV | boolean |   | true | DEVICE_DATA | 205 | 1 | 1 | 
| auxheateronly | aux heater only | boolean |   | true | DEVICE_DATA | 206 | 1 | 1 | 
| auxheateroff | disable aux heater | boolean |   | true | DEVICE_DATA | 207 | 1 | 1 | 
| auxheaterstatus | aux heater status | boolean |   | false | DEVICE_DATA | 208 | 1 | 1 | 
| auxheaterlevel | aux heater level | uint8 (>=0<=100) | % | false | DEVICE_DATA | 209 | 1 | 1 | 
| auxheaterdelay | aux heater on delay | uint16 (>=10<=1000) | K*min | true | DEVICE_DATA | 210 | 1 | 10 | 
| auxmaxlimit | aux heater max limit | uint8 (>=0<=10) | K | true | DEVICE_DATA | 211 | 1 | 1/10 | 
| auxlimitstart | aux heater limit start | uint8 (>=0<=10) | K | true | DEVICE_DATA | 212 | 1 | 1/10 | 
| auxheatrmode | aux heater mode | enum [eco\|comfort] |   | true | DEVICE_DATA | 213 | 1 | 1 | 
| hphystheat | on/off hyst heat | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 214 | 1 | 5 | 
| hphystcool | on/off hyst cool | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 215 | 1 | 5 | 
| hphystpool | on/off hyst pool | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 216 | 1 | 5 | 
| silentmode | silent mode | enum [off\|auto\|on] |   | true | DEVICE_DATA | 217 | 1 | 1 | 
| silentfrom | silent mode from | uint8 (>=0<=3810) | minutes | true | DEVICE_DATA | 218 | 1 | 15 | 
| silentto | silent mode to | uint8 (>=0<=3810) | minutes | true | DEVICE_DATA | 219 | 1 | 15 | 
| mintempsilent | min outside temp for silent mode | int8 (>=-126<=126) | C | true | DEVICE_DATA | 220 | 1 | 1 | 
| tempparmode | outside temp parallel mode | int8 (>=-126<=126) | C | true | DEVICE_DATA | 221 | 1 | 1 | 
| auxheatmix | aux heater mixing valve | int8 (>=-100<=100) | % | false | DEVICE_DATA | 222 | 1 | 1 | 
| tempdiffheat | temp diff TC3/TC0 heat | uint8 (>=2<=10) | K | true | DEVICE_DATA | 223 | 1 | 1/10 | 
| tempdiffcool | temp diff TC3/TC0 cool | uint8 (>=2<=10) | K | true | DEVICE_DATA | 224 | 1 | 1/10 | 
| vpcooling | valve/pump cooling | boolean |   | true | DEVICE_DATA | 225 | 1 | 1 | 
| heatcable | heating cable | boolean |   | true | DEVICE_DATA | 226 | 1 | 1 | 
| vc0valve | VC0 valve | boolean |   | true | DEVICE_DATA | 227 | 1 | 1 | 
| primepump | primary heatpump | boolean |   | true | DEVICE_DATA | 228 | 1 | 1 | 
| primepumpmod | primary heatpump modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 229 | 1 | 1 | 
| hp3way | 3-way valve | boolean |   | true | DEVICE_DATA | 230 | 1 | 1 | 
| elheatstep1 | el. heater step 1 | boolean |   | true | DEVICE_DATA | 231 | 1 | 1 | 
| elheatstep2 | el. heater step 2 | boolean |   | true | DEVICE_DATA | 232 | 1 | 1 | 
| elheatstep3 | el. heater step 3 | boolean |   | true | DEVICE_DATA | 233 | 1 | 1 | 
| hpea0 | condensate reservoir heating (EA0) | boolean |   | false | DEVICE_DATA | 234 | 1 | 1 | 
| hppumpmode | primary heatpump mode | enum [auto\|continuous] |   | true | DEVICE_DATA | 235 | 1 | 1 | 
| fan | fan | uint8 (>=20<=100) | % | true | DEVICE_DATA | 236 | 1 | 1 | 
| shutdown | shutdown | cmd [off\|on] |   | true | DEVICE_DATA | 237 | 1 | 1 | 
| hpcurrpower | compressor current power | uint16 (>=0<=31999) | W | false | DEVICE_DATA | 238 | 1 | 1 | 
| hppowerlimit | power limit | uint16 (>=0<=31999) | W | true | DEVICE_DATA | 239 | 1 | 1 | 
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 240 | 1 | 1 | 
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 241 | 1 | 1 | 
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 242 | 1 | 1 | 
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 243 | 1 | 1 | 
| alternatingop | alternating operation | boolean |   | true | DHW | 15 | 1 | 1 | 
| altopprioheat | prioritise heating during dhw | uint8 (>=20<=120) | minutes | true | DHW | 16 | 1 | 1 | 
| altopprio | prioritise dhw during heating | uint8 (>=30<=120) | minutes | true | DHW | 17 | 1 | 1 | 
| comfoff | comfort switch off | uint8 (>=15<=65) | C | true | DHW | 18 | 1 | 1 | 
| ecooff | eco switch off | uint8 (>=15<=65) | C | true | DHW | 19 | 1 | 1 | 
| ecoplusoff | eco+ switch off | uint8 (>=48<=63) | C | true | DHW | 20 | 1 | 1 | 
| comfdiff | comfort diff | uint8 (>=6<=12) | K | true | DHW | 21 | 1 | 1 | 
| ecodiff | eco diff | uint8 (>=6<=12) | K | true | DHW | 22 | 1 | 1 | 
| ecoplusdiff | eco+ diff | uint8 (>=6<=12) | K | true | DHW | 23 | 1 | 1 | 
| comfstop | comfort stop temp | uint8 (>=0<=254) | C | true | DHW | 24 | 1 | 1 | 
| ecostop | eco stop temp | uint8 (>=0<=254) | C | true | DHW | 25 | 1 | 1 | 
| ecoplusstop | eco+ stop temp | uint8 (>=0<=254) | C | true | DHW | 26 | 1 | 1 | 
| hpcircpump | circulation pump available during dhw | boolean |   | true | DHW | 27 | 1 | 1 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 

### Cascade CM10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Cascade MC400
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Cascade MCM10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Cerapur Aero
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Condens 5000i, Greenstar 8000/GC9800IW, GB192i*2
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Condens, Logamax/Logomatic, Cerapur Top, Greenstar, Generic HT3
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### EasyControl Adapter
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Ecomline Excellent
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Enviline, Compress 6000AW, Hybrid 3000-7000iAW, SupraEco/Geo 5xx, WLW196i/WSW196i
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=167772) | kWh | false | DHW | 0 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrgcool | energy cooling | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 83 | 2 | 1/100 | 
| metertotal | meter total | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 | 
| metercomp | meter compressor | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 87 | 2 | 1/100 | 
| metereheat | meter e-heater | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 89 | 2 | 1/100 | 
| meterheat | meter heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 91 | 2 | 1/100 | 
| metercool | meter cooling | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 93 | 2 | 1/100 | 
| meter | meter | uint24 (>=0<=167772) | kWh | false | DHW | 2 | 2 | 1/100 | 
| uptimetotal | heatpump total uptime | time (>=0<=279620) | minutes | false | DEVICE_DATA | 95 | 2 | 1/60 | 
| uptimecontrol | total operating time heat | time (>=0<=279620) | minutes | false | DEVICE_DATA | 97 | 2 | 1/60 | 
| uptimecompheating | operating time compressor heating | time (>=0<=279620) | minutes | false | DEVICE_DATA | 99 | 2 | 1/60 | 
| uptimecompcooling | operating time compressor cooling | time (>=0<=279620) | minutes | false | DEVICE_DATA | 101 | 2 | 1/60 | 
| uptimecomp | operating time compressor | time (>=0<=279620) | minutes | false | DHW | 4 | 2 | 1/60 | 
| uptimecomppool | operating time compressor pool | time (>=0<=279620) | minutes | false | DEVICE_DATA | 103 | 2 | 1/60 | 
| totalcompstarts | total compressor control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 105 | 2 | 1 | 
| heatingstarts | heating control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 107 | 2 | 1 | 
| coolingstarts | cooling control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 109 | 2 | 1 | 
| startshp | starts hp | uint24 (>=0<=16777214) |   | false | DHW | 6 | 2 | 1 | 
| poolstarts | pool control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 111 | 2 | 1 | 
| nrgconstotal | total energy consumption | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 113 | 2 | 1 | 
| nrgconscomptotal | total energy consumption compressor | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 115 | 2 | 1 | 
| nrgconscompheating | energy consumption compressor heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 117 | 2 | 1 | 
| nrgconscomp | energy consumption compressor | uint24 (>=0<=16777214) | kWh | false | DHW | 8 | 2 | 1 | 
| nrgconscompcooling | energy consumption compressor cooling | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 119 | 2 | 1 | 
| nrgconscomppool | energy consumption compressor pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 121 | 2 | 1 | 
| auxelecheatnrgconstotal | total aux elec. heater energy consumption | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 123 | 2 | 1 | 
| auxelecheatnrgconsheating | aux elec. heater energy consumption heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 125 | 2 | 1 | 
| auxelecheatnrgcons | aux elec. heater energy consumption | uint24 (>=0<=16777214) | kWh | false | DHW | 10 | 2 | 1 | 
| auxelecheatnrgconspool | aux elec. heater energy consumption pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 127 | 2 | 1 | 
| nrgsupptotal | total energy supplied | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 129 | 2 | 1 | 
| nrgsuppheating | total energy supplied heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 131 | 2 | 1 | 
| nrgsupp | total energy warm supplied | uint24 (>=0<=16777214) | kWh | false | DHW | 12 | 2 | 1 | 
| nrgsuppcooling | total energy supplied cooling | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 133 | 2 | 1 | 
| nrgsupppool | total energy supplied pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 135 | 2 | 1 | 
| hppower | compressor power output | uint8 (>=0<=25) | kW | false | DEVICE_DATA | 137 | 1 | 1/10 | 
| hpmaxpower | compressor max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 138 | 1 | 1 | 
| pvmaxcomp | pv compressor max power | uint8 (>=0<=25) | kW | true | DEVICE_DATA | 139 | 1 | 1/10 | 
| powerreduction | power reduction | uint8 (>=30<=60) | % | true | DEVICE_DATA | 140 | 1 | 10 | 
| hpsetdiffpress | set differential pressure | uint8 (>=150<=750) | mbar | true | DEVICE_DATA | 141 | 1 | 50 | 
| hpcompon | hp compressor | boolean |   | false | DEVICE_DATA | 142 | 1 | 1 | 
| hpactivity | compressor activity | enum [off\|heating\|cooling\|hot water\|pool\|pool heating\|defrost\|compressor alarm] |   | false | DEVICE_DATA | 143 | 1 | 1 | 
| hpbrinepumpspd | brine pump speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 144 | 1 | 1 | 
| hpswitchvalve | switch valve | boolean |   | false | DEVICE_DATA | 145 | 1 | 1 | 
| hpcompspd | compressor speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 146 | 1 | 1 | 
| hpcircspd | circulation pump speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 147 | 1 | 1 | 
| hpbrinein | brine in/evaporator | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 148 | 1 | 1/10 | 
| hpbrineout | brine out/condenser | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 149 | 1 | 1/10 | 
| hptc0 | heat carrier return (TC0) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 150 | 1 | 1/10 | 
| hptc1 | heat carrier forward (TC1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 151 | 1 | 1/10 | 
| hptc3 | condenser temperature (TC3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 152 | 1 | 1/10 | 
| hptr1 | compressor temperature (TR1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 153 | 1 | 1/10 | 
| hptr3 | refrigerant temperature liquid side (condenser output) (TR3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 154 | 1 | 1/10 | 
| hptr4 | evaporator inlet temperature (TR4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 155 | 1 | 1/10 | 
| hptr5 | compressor inlet temperature (TR5) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 156 | 1 | 1/10 | 
| hptr6 | compressor outlet temperature (TR6) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 157 | 1 | 1/10 | 
| hptr7 | refrigerant temperature gas side (condenser input) (TR7) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 158 | 1 | 1/10 | 
| hptl2 | air inlet temperature (TL2) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 159 | 1 | 1/10 | 
| hppl1 | low pressure side temperature (PL1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 160 | 1 | 1/10 | 
| hpph1 | high pressure side temperature (PH1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 161 | 1 | 1/10 | 
| hpta4 | drain pan temp (TA4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 162 | 1 | 1/10 | 
| hptw1 | reservoir temp (TW1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 163 | 1 | 1/10 | 
| poolsettemp | pool set temperature | uint8 (>=0<=127) | C | true | DEVICE_DATA | 164 | 1 | 1/2 | 
| hp4way | 4-way valve (VR4) | enum [cooling & defrost\|heating & dhw] |   | false | DEVICE_DATA | 165 | 1 | 1 | 
| hpin1 | input 1 state | boolean |   | false | DEVICE_DATA | 166 | 1 | 1 | 
| hpin1opt | input 1 options | string |   | true | DEVICE_DATA | 167 | 8 | 1 | 
| hpin2 | input 2 state | boolean |   | false | DEVICE_DATA | 175 | 1 | 1 | 
| hpin2opt | input 2 options | string |   | true | DEVICE_DATA | 176 | 8 | 1 | 
| hpin3 | input 3 state | boolean |   | false | DEVICE_DATA | 184 | 1 | 1 | 
| hpin3opt | input 3 options | string |   | true | DEVICE_DATA | 185 | 8 | 1 | 
| hpin4 | input 4 state | boolean |   | false | DEVICE_DATA | 193 | 1 | 1 | 
| hpin4opt | input 4 options | string |   | true | DEVICE_DATA | 194 | 8 | 1 | 
| maxheatcomp | heat limit compressor | enum [0 kW\|2 kW\|3 kW\|4 kW\|6 kW\|9 kW] |   | true | DEVICE_DATA | 202 | 1 | 1 | 
| maxheatheat | heat limit heating | enum [0 kW\|2 kW\|3 kW\|4 kW\|6 kW\|9 kW] |   | true | DEVICE_DATA | 203 | 1 | 1 | 
| maxheat | heat limit | enum [0 kW\|2 kW\|3 kW\|4 kW\|6 kW\|9 kW] |   | true | DHW | 14 | 1 | 1 | 
| mandefrost | manual defrost | boolean |   | true | DEVICE_DATA | 204 | 1 | 1 | 
| pvcooling | cooling only with PV | boolean |   | true | DEVICE_DATA | 205 | 1 | 1 | 
| auxheateronly | aux heater only | boolean |   | true | DEVICE_DATA | 206 | 1 | 1 | 
| auxheateroff | disable aux heater | boolean |   | true | DEVICE_DATA | 207 | 1 | 1 | 
| auxheaterstatus | aux heater status | boolean |   | false | DEVICE_DATA | 208 | 1 | 1 | 
| auxheaterlevel | aux heater level | uint8 (>=0<=100) | % | false | DEVICE_DATA | 209 | 1 | 1 | 
| auxheaterdelay | aux heater on delay | uint16 (>=10<=1000) | K*min | true | DEVICE_DATA | 210 | 1 | 10 | 
| auxmaxlimit | aux heater max limit | uint8 (>=0<=10) | K | true | DEVICE_DATA | 211 | 1 | 1/10 | 
| auxlimitstart | aux heater limit start | uint8 (>=0<=10) | K | true | DEVICE_DATA | 212 | 1 | 1/10 | 
| auxheatrmode | aux heater mode | enum [eco\|comfort] |   | true | DEVICE_DATA | 213 | 1 | 1 | 
| hphystheat | on/off hyst heat | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 214 | 1 | 5 | 
| hphystcool | on/off hyst cool | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 215 | 1 | 5 | 
| hphystpool | on/off hyst pool | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 216 | 1 | 5 | 
| silentmode | silent mode | enum [off\|auto\|on] |   | true | DEVICE_DATA | 217 | 1 | 1 | 
| silentfrom | silent mode from | uint8 (>=0<=3810) | minutes | true | DEVICE_DATA | 218 | 1 | 15 | 
| silentto | silent mode to | uint8 (>=0<=3810) | minutes | true | DEVICE_DATA | 219 | 1 | 15 | 
| mintempsilent | min outside temp for silent mode | int8 (>=-126<=126) | C | true | DEVICE_DATA | 220 | 1 | 1 | 
| tempparmode | outside temp parallel mode | int8 (>=-126<=126) | C | true | DEVICE_DATA | 221 | 1 | 1 | 
| auxheatmix | aux heater mixing valve | int8 (>=-100<=100) | % | false | DEVICE_DATA | 222 | 1 | 1 | 
| tempdiffheat | temp diff TC3/TC0 heat | uint8 (>=2<=10) | K | true | DEVICE_DATA | 223 | 1 | 1/10 | 
| tempdiffcool | temp diff TC3/TC0 cool | uint8 (>=2<=10) | K | true | DEVICE_DATA | 224 | 1 | 1/10 | 
| vpcooling | valve/pump cooling | boolean |   | true | DEVICE_DATA | 225 | 1 | 1 | 
| heatcable | heating cable | boolean |   | true | DEVICE_DATA | 226 | 1 | 1 | 
| vc0valve | VC0 valve | boolean |   | true | DEVICE_DATA | 227 | 1 | 1 | 
| primepump | primary heatpump | boolean |   | true | DEVICE_DATA | 228 | 1 | 1 | 
| primepumpmod | primary heatpump modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 229 | 1 | 1 | 
| hp3way | 3-way valve | boolean |   | true | DEVICE_DATA | 230 | 1 | 1 | 
| elheatstep1 | el. heater step 1 | boolean |   | true | DEVICE_DATA | 231 | 1 | 1 | 
| elheatstep2 | el. heater step 2 | boolean |   | true | DEVICE_DATA | 232 | 1 | 1 | 
| elheatstep3 | el. heater step 3 | boolean |   | true | DEVICE_DATA | 233 | 1 | 1 | 
| hpea0 | condensate reservoir heating (EA0) | boolean |   | false | DEVICE_DATA | 234 | 1 | 1 | 
| hppumpmode | primary heatpump mode | enum [auto\|continuous] |   | true | DEVICE_DATA | 235 | 1 | 1 | 
| fan | fan | uint8 (>=20<=100) | % | true | DEVICE_DATA | 236 | 1 | 1 | 
| shutdown | shutdown | cmd [off\|on] |   | true | DEVICE_DATA | 237 | 1 | 1 | 
| hpcurrpower | compressor current power | uint16 (>=0<=31999) | W | false | DEVICE_DATA | 238 | 1 | 1 | 
| hppowerlimit | power limit | uint16 (>=0<=31999) | W | true | DEVICE_DATA | 239 | 1 | 1 | 
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 240 | 1 | 1 | 
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 241 | 1 | 1 | 
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 242 | 1 | 1 | 
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 243 | 1 | 1 | 
| alternatingop | alternating operation | boolean |   | true | DHW | 15 | 1 | 1 | 
| altopprioheat | prioritise heating during dhw | uint8 (>=20<=120) | minutes | true | DHW | 16 | 1 | 1 | 
| altopprio | prioritise dhw during heating | uint8 (>=30<=120) | minutes | true | DHW | 17 | 1 | 1 | 
| comfoff | comfort switch off | uint8 (>=15<=65) | C | true | DHW | 18 | 1 | 1 | 
| ecooff | eco switch off | uint8 (>=15<=65) | C | true | DHW | 19 | 1 | 1 | 
| ecoplusoff | eco+ switch off | uint8 (>=48<=63) | C | true | DHW | 20 | 1 | 1 | 
| comfdiff | comfort diff | uint8 (>=6<=12) | K | true | DHW | 21 | 1 | 1 | 
| ecodiff | eco diff | uint8 (>=6<=12) | K | true | DHW | 22 | 1 | 1 | 
| ecoplusdiff | eco+ diff | uint8 (>=6<=12) | K | true | DHW | 23 | 1 | 1 | 
| comfstop | comfort stop temp | uint8 (>=0<=254) | C | true | DHW | 24 | 1 | 1 | 
| ecostop | eco stop temp | uint8 (>=0<=254) | C | true | DHW | 25 | 1 | 1 | 
| ecoplusstop | eco+ stop temp | uint8 (>=0<=254) | C | true | DHW | 26 | 1 | 1 | 
| hpcircpump | circulation pump available during dhw | boolean |   | true | DHW | 27 | 1 | 1 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 

### GB*72, Trendline, Cerapur, Greenstar Si
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### GB212
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### GC7000F
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Geo 5xx
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=167772) | kWh | false | DHW | 0 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrgcool | energy cooling | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 83 | 2 | 1/100 | 
| metertotal | meter total | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 | 
| metercomp | meter compressor | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 87 | 2 | 1/100 | 
| metereheat | meter e-heater | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 89 | 2 | 1/100 | 
| meterheat | meter heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 91 | 2 | 1/100 | 
| metercool | meter cooling | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 93 | 2 | 1/100 | 
| meter | meter | uint24 (>=0<=167772) | kWh | false | DHW | 2 | 2 | 1/100 | 
| uptimetotal | heatpump total uptime | time (>=0<=279620) | minutes | false | DEVICE_DATA | 95 | 2 | 1/60 | 
| uptimecontrol | total operating time heat | time (>=0<=279620) | minutes | false | DEVICE_DATA | 97 | 2 | 1/60 | 
| uptimecompheating | operating time compressor heating | time (>=0<=279620) | minutes | false | DEVICE_DATA | 99 | 2 | 1/60 | 
| uptimecompcooling | operating time compressor cooling | time (>=0<=279620) | minutes | false | DEVICE_DATA | 101 | 2 | 1/60 | 
| uptimecomp | operating time compressor | time (>=0<=279620) | minutes | false | DHW | 4 | 2 | 1/60 | 
| uptimecomppool | operating time compressor pool | time (>=0<=279620) | minutes | false | DEVICE_DATA | 103 | 2 | 1/60 | 
| totalcompstarts | total compressor control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 105 | 2 | 1 | 
| heatingstarts | heating control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 107 | 2 | 1 | 
| coolingstarts | cooling control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 109 | 2 | 1 | 
| startshp | starts hp | uint24 (>=0<=16777214) |   | false | DHW | 6 | 2 | 1 | 
| poolstarts | pool control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 111 | 2 | 1 | 
| nrgconstotal | total energy consumption | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 113 | 2 | 1 | 
| nrgconscomptotal | total energy consumption compressor | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 115 | 2 | 1 | 
| nrgconscompheating | energy consumption compressor heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 117 | 2 | 1 | 
| nrgconscomp | energy consumption compressor | uint24 (>=0<=16777214) | kWh | false | DHW | 8 | 2 | 1 | 
| nrgconscompcooling | energy consumption compressor cooling | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 119 | 2 | 1 | 
| nrgconscomppool | energy consumption compressor pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 121 | 2 | 1 | 
| auxelecheatnrgconstotal | total aux elec. heater energy consumption | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 123 | 2 | 1 | 
| auxelecheatnrgconsheating | aux elec. heater energy consumption heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 125 | 2 | 1 | 
| auxelecheatnrgcons | aux elec. heater energy consumption | uint24 (>=0<=16777214) | kWh | false | DHW | 10 | 2 | 1 | 
| auxelecheatnrgconspool | aux elec. heater energy consumption pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 127 | 2 | 1 | 
| nrgsupptotal | total energy supplied | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 129 | 2 | 1 | 
| nrgsuppheating | total energy supplied heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 131 | 2 | 1 | 
| nrgsupp | total energy warm supplied | uint24 (>=0<=16777214) | kWh | false | DHW | 12 | 2 | 1 | 
| nrgsuppcooling | total energy supplied cooling | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 133 | 2 | 1 | 
| nrgsupppool | total energy supplied pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 135 | 2 | 1 | 
| hppower | compressor power output | uint8 (>=0<=25) | kW | false | DEVICE_DATA | 137 | 1 | 1/10 | 
| hpmaxpower | compressor max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 138 | 1 | 1 | 
| pvmaxcomp | pv compressor max power | uint8 (>=0<=25) | kW | true | DEVICE_DATA | 139 | 1 | 1/10 | 
| powerreduction | power reduction | uint8 (>=30<=60) | % | true | DEVICE_DATA | 140 | 1 | 10 | 
| hpsetdiffpress | set differential pressure | uint8 (>=150<=750) | mbar | true | DEVICE_DATA | 141 | 1 | 50 | 
| hpcompon | hp compressor | boolean |   | false | DEVICE_DATA | 142 | 1 | 1 | 
| hpactivity | compressor activity | enum [off\|heating\|cooling\|hot water\|pool\|pool heating\|defrost\|compressor alarm] |   | false | DEVICE_DATA | 143 | 1 | 1 | 
| hpbrinepumpspd | brine pump speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 144 | 1 | 1 | 
| hpswitchvalve | switch valve | boolean |   | false | DEVICE_DATA | 145 | 1 | 1 | 
| hpcompspd | compressor speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 146 | 1 | 1 | 
| hpcircspd | circulation pump speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 147 | 1 | 1 | 
| hpbrinein | brine in/evaporator | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 148 | 1 | 1/10 | 
| hpbrineout | brine out/condenser | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 149 | 1 | 1/10 | 
| hptc0 | heat carrier return (TC0) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 150 | 1 | 1/10 | 
| hptc1 | heat carrier forward (TC1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 151 | 1 | 1/10 | 
| hptc3 | condenser temperature (TC3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 152 | 1 | 1/10 | 
| hptr1 | compressor temperature (TR1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 153 | 1 | 1/10 | 
| hptr3 | refrigerant temperature liquid side (condenser output) (TR3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 154 | 1 | 1/10 | 
| hptr4 | evaporator inlet temperature (TR4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 155 | 1 | 1/10 | 
| hptr5 | compressor inlet temperature (TR5) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 156 | 1 | 1/10 | 
| hptr6 | compressor outlet temperature (TR6) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 157 | 1 | 1/10 | 
| hptr7 | refrigerant temperature gas side (condenser input) (TR7) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 158 | 1 | 1/10 | 
| hptl2 | air inlet temperature (TL2) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 159 | 1 | 1/10 | 
| hppl1 | low pressure side temperature (PL1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 160 | 1 | 1/10 | 
| hpph1 | high pressure side temperature (PH1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 161 | 1 | 1/10 | 
| hpta4 | drain pan temp (TA4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 162 | 1 | 1/10 | 
| hptw1 | reservoir temp (TW1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 163 | 1 | 1/10 | 
| poolsettemp | pool set temperature | uint8 (>=0<=127) | C | true | DEVICE_DATA | 164 | 1 | 1/2 | 
| hp4way | 4-way valve (VR4) | enum [cooling & defrost\|heating & dhw] |   | false | DEVICE_DATA | 165 | 1 | 1 | 
| hpin1 | input 1 state | boolean |   | false | DEVICE_DATA | 166 | 1 | 1 | 
| hpin1opt | input 1 options | string |   | true | DEVICE_DATA | 167 | 8 | 1 | 
| hpin2 | input 2 state | boolean |   | false | DEVICE_DATA | 175 | 1 | 1 | 
| hpin2opt | input 2 options | string |   | true | DEVICE_DATA | 176 | 8 | 1 | 
| hpin3 | input 3 state | boolean |   | false | DEVICE_DATA | 184 | 1 | 1 | 
| hpin3opt | input 3 options | string |   | true | DEVICE_DATA | 185 | 8 | 1 | 
| hpin4 | input 4 state | boolean |   | false | DEVICE_DATA | 193 | 1 | 1 | 
| hpin4opt | input 4 options | string |   | true | DEVICE_DATA | 194 | 8 | 1 | 
| maxheatcomp | heat limit compressor | enum [0 kW\|2 kW\|3 kW\|4 kW\|6 kW\|9 kW] |   | true | DEVICE_DATA | 202 | 1 | 1 | 
| maxheatheat | heat limit heating | enum [0 kW\|2 kW\|3 kW\|4 kW\|6 kW\|9 kW] |   | true | DEVICE_DATA | 203 | 1 | 1 | 
| maxheat | heat limit | enum [0 kW\|2 kW\|3 kW\|4 kW\|6 kW\|9 kW] |   | true | DHW | 14 | 1 | 1 | 
| mandefrost | manual defrost | boolean |   | true | DEVICE_DATA | 204 | 1 | 1 | 
| pvcooling | cooling only with PV | boolean |   | true | DEVICE_DATA | 205 | 1 | 1 | 
| auxheateronly | aux heater only | boolean |   | true | DEVICE_DATA | 206 | 1 | 1 | 
| auxheateroff | disable aux heater | boolean |   | true | DEVICE_DATA | 207 | 1 | 1 | 
| auxheaterstatus | aux heater status | boolean |   | false | DEVICE_DATA | 208 | 1 | 1 | 
| auxheaterlevel | aux heater level | uint8 (>=0<=100) | % | false | DEVICE_DATA | 209 | 1 | 1 | 
| auxheaterdelay | aux heater on delay | uint16 (>=10<=1000) | K*min | true | DEVICE_DATA | 210 | 1 | 10 | 
| auxmaxlimit | aux heater max limit | uint8 (>=0<=10) | K | true | DEVICE_DATA | 211 | 1 | 1/10 | 
| auxlimitstart | aux heater limit start | uint8 (>=0<=10) | K | true | DEVICE_DATA | 212 | 1 | 1/10 | 
| auxheatrmode | aux heater mode | enum [eco\|comfort] |   | true | DEVICE_DATA | 213 | 1 | 1 | 
| hphystheat | on/off hyst heat | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 214 | 1 | 5 | 
| hphystcool | on/off hyst cool | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 215 | 1 | 5 | 
| hphystpool | on/off hyst pool | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 216 | 1 | 5 | 
| silentmode | silent mode | enum [off\|auto\|on] |   | true | DEVICE_DATA | 217 | 1 | 1 | 
| silentfrom | silent mode from | uint8 (>=0<=3810) | minutes | true | DEVICE_DATA | 218 | 1 | 15 | 
| silentto | silent mode to | uint8 (>=0<=3810) | minutes | true | DEVICE_DATA | 219 | 1 | 15 | 
| mintempsilent | min outside temp for silent mode | int8 (>=-126<=126) | C | true | DEVICE_DATA | 220 | 1 | 1 | 
| tempparmode | outside temp parallel mode | int8 (>=-126<=126) | C | true | DEVICE_DATA | 221 | 1 | 1 | 
| auxheatmix | aux heater mixing valve | int8 (>=-100<=100) | % | false | DEVICE_DATA | 222 | 1 | 1 | 
| tempdiffheat | temp diff TC3/TC0 heat | uint8 (>=2<=10) | K | true | DEVICE_DATA | 223 | 1 | 1/10 | 
| tempdiffcool | temp diff TC3/TC0 cool | uint8 (>=2<=10) | K | true | DEVICE_DATA | 224 | 1 | 1/10 | 
| vpcooling | valve/pump cooling | boolean |   | true | DEVICE_DATA | 225 | 1 | 1 | 
| heatcable | heating cable | boolean |   | true | DEVICE_DATA | 226 | 1 | 1 | 
| vc0valve | VC0 valve | boolean |   | true | DEVICE_DATA | 227 | 1 | 1 | 
| primepump | primary heatpump | boolean |   | true | DEVICE_DATA | 228 | 1 | 1 | 
| primepumpmod | primary heatpump modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 229 | 1 | 1 | 
| hp3way | 3-way valve | boolean |   | true | DEVICE_DATA | 230 | 1 | 1 | 
| elheatstep1 | el. heater step 1 | boolean |   | true | DEVICE_DATA | 231 | 1 | 1 | 
| elheatstep2 | el. heater step 2 | boolean |   | true | DEVICE_DATA | 232 | 1 | 1 | 
| elheatstep3 | el. heater step 3 | boolean |   | true | DEVICE_DATA | 233 | 1 | 1 | 
| hpea0 | condensate reservoir heating (EA0) | boolean |   | false | DEVICE_DATA | 234 | 1 | 1 | 
| hppumpmode | primary heatpump mode | enum [auto\|continuous] |   | true | DEVICE_DATA | 235 | 1 | 1 | 
| fan | fan | uint8 (>=20<=100) | % | true | DEVICE_DATA | 236 | 1 | 1 | 
| shutdown | shutdown | cmd [off\|on] |   | true | DEVICE_DATA | 237 | 1 | 1 | 
| hpcurrpower | compressor current power | uint16 (>=0<=31999) | W | false | DEVICE_DATA | 238 | 1 | 1 | 
| hppowerlimit | power limit | uint16 (>=0<=31999) | W | true | DEVICE_DATA | 239 | 1 | 1 | 
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 240 | 1 | 1 | 
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 241 | 1 | 1 | 
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 242 | 1 | 1 | 
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 243 | 1 | 1 | 
| alternatingop | alternating operation | boolean |   | true | DHW | 15 | 1 | 1 | 
| altopprioheat | prioritise heating during dhw | uint8 (>=20<=120) | minutes | true | DHW | 16 | 1 | 1 | 
| altopprio | prioritise dhw during heating | uint8 (>=30<=120) | minutes | true | DHW | 17 | 1 | 1 | 
| comfoff | comfort switch off | uint8 (>=15<=65) | C | true | DHW | 18 | 1 | 1 | 
| ecooff | eco switch off | uint8 (>=15<=65) | C | true | DHW | 19 | 1 | 1 | 
| ecoplusoff | eco+ switch off | uint8 (>=48<=63) | C | true | DHW | 20 | 1 | 1 | 
| comfdiff | comfort diff | uint8 (>=6<=12) | K | true | DHW | 21 | 1 | 1 | 
| ecodiff | eco diff | uint8 (>=6<=12) | K | true | DHW | 22 | 1 | 1 | 
| ecoplusdiff | eco+ diff | uint8 (>=6<=12) | K | true | DHW | 23 | 1 | 1 | 
| comfstop | comfort stop temp | uint8 (>=0<=254) | C | true | DHW | 24 | 1 | 1 | 
| ecostop | eco stop temp | uint8 (>=0<=254) | C | true | DHW | 25 | 1 | 1 | 
| ecoplusstop | eco+ stop temp | uint8 (>=0<=254) | C | true | DHW | 26 | 1 | 1 | 
| hpcircpump | circulation pump available during dhw | boolean |   | true | DHW | 27 | 1 | 1 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 

### Greenstar 30Ri Compact
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Greenstar HIU, Logamax kompakt WS170
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| netflowtemp | heat network flow temp | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 271 | 1 | 1/10 | 
| heatvalve | heating valve | uint8 (>=0<=100) | % | false | DEVICE_DATA | 272 | 1 | 1 | 
| dhwvalve | valve | uint8 (>=0<=100) | % | false | DHW | 73 | 1 | 1 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| keepwarmtemp | keep warm temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 273 | 1 | 1 | 
| setreturntemp | set temp return | uint8 (>=0<=254) | C | true | DEVICE_DATA | 274 | 1 | 1 | 
| heating | heating | boolean |   | false | DEVICE_DATA | 275 | 1 | 1 | 
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 

### Hybrid Heatpump
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Logamax Plus GB022
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Logamax Plus GB122, Condense 2300, Junkers Cerapur GC2200W
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Logamax Plus, GB192, Condens GC9000, Greenstar ErP
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Logamax U122, Cerapur
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Logano GB1*5, Logamatic MC10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Logano GB125/KB195i, Logamatic MC110
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Logano GB212
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Proline
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

### Topline, GB162
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 | 
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 1 | 1 | 1 | 
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 | 
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 4 | 1 | 1 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 244 | 1 | 1/10 | 
| burngas | gas | boolean |   | false | DEVICE_DATA | 245 | 1 | 1 | 
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 246 | 1 | 1 | 
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 247 | 1 | 1/10 | 
| fanwork | fan | boolean |   | false | DEVICE_DATA | 248 | 1 | 1 | 
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 249 | 1 | 1 | 
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 250 | 1 | 1 | 
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 251 | 1 | 1 | 
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 252 | 1 | 1 | 
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 253 | 1 | 1 | 
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 254 | 1 | 1 | 
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 255 | 1 | 1/10 | 
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 256 | 1 | 1 | 
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 257 | 1 | 1 | 
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 258 | 1 | 1 | 
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 259 | 1 | 1 | 
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 260 | 1 | 1 | 
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 261 | 1 | 1 | 
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 262 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 263 | 1 | 1 | 
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 264 | 1 | 1 | 
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 265 | 1 | 1 | 
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 13 | 1 | 1 | 
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 14 | 1 | 1 | 
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 15 | 1 | 1 | 
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 16 | 1 | 1 | 
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 | 
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 18 | 1 | 1 | 
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 19 | 1 | 1 | 
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 20 | 1 | 1 | 
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 21 | 1 | 1 | 
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 22 | 1 | 1 | 
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 24 | 2 | 1 | 
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 26 | 2 | 1 | 
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 28 | 2 | 1 | 
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 30 | 2 | 1 | 
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 32 | 2 | 1 | 
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 34 | 2 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 36 | 28 | 1 | 
| servicecode | service code | string |   | false | DEVICE_DATA | 64 | 2 | 1 | 
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 67 | 2 | 1 | 
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 69 | 1 | 1 | 
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 70 | 1 | 1 | 
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 71 | 6 | 1 | 
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 77 | 1 | 1 | 
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 78 | 1 | 1 | 
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 91 | 2 | 1/10 | 
| meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 | 
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 266 | 2 | 1/10 | 
| gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 69 | 2 | 1/10 | 
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 268 | 2 | 1/10 | 
| nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 | 
| tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 | 
| seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 | 
| tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 | 
| seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 | 
| seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 | 
| solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 | 
| type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 | 
| comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 | 
| comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 | 
| chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 | 
| maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 | 
| circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 | 
| chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 | 
| curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 | 
| curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 | 
| storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 | 
| activated | activated | boolean |   | true | DHW | 55 | 1 | 1 | 
| onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 | 
| charging | charging | boolean |   | false | DHW | 58 | 1 | 1 | 
| recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 | 
| tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 | 
| active | active | boolean |   | false | DHW | 61 | 1 | 1 | 
| 3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 | 
| mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 63 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 | 
| starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 65 | 2 | 1 | 
| workm | active time | time (>=0<=16777214) | minutes | false | DHW | 67 | 2 | 1 | 
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 270 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 79 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 81 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 | 

## Devices of type *controller*
### Rego 3000
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| datetime | date/time | string |   | false | DEVICE_DATA | 0 | 13 | 1 | 

## Devices of type *extension*
### EM10, EM100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| flowtempvf | flow temperature in header (T0/Vf) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 0 | 1 | 1/10 | 
| input | input | uint8 (>=0<=25) | V | false | DEVICE_DATA | 1 | 1 | 1/10 | 
| outpow | output IO1 | uint8 (>=0<=100) | % | false | DEVICE_DATA | 2 | 1 | 1 | 
| setpower | request power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 3 | 1 | 1 | 
| setpoint | set temp. | uint8 (>=0<=254) | C | false | DEVICE_DATA | 4 | 1 | 1 | 
| minv | min volt. | uint8 (>=0<=25) | V | true | DEVICE_DATA | 5 | 1 | 1/10 | 
| maxv | max volt. | uint8 (>=0<=25) | V | true | DEVICE_DATA | 6 | 1 | 1/10 | 
| mint | min temp. | uint8 (>=0<=254) | C | true | DEVICE_DATA | 7 | 1 | 1 | 
| maxt | max temp. | uint8 (>=0<=254) | C | true | DEVICE_DATA | 8 | 1 | 1 | 
| mode | operating mode | uint8 (>=0<=254) |   | false | DEVICE_DATA | 9 | 1 | 1 | 

### T1RF
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| flowtempvf | flow temperature in header (T0/Vf) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 0 | 1 | 1/10 | 
| input | input | uint8 (>=0<=25) | V | false | DEVICE_DATA | 1 | 1 | 1/10 | 
| outpow | output IO1 | uint8 (>=0<=100) | % | false | DEVICE_DATA | 2 | 1 | 1 | 
| setpower | request power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 3 | 1 | 1 | 
| setpoint | set temp. | uint8 (>=0<=254) | C | false | DEVICE_DATA | 4 | 1 | 1 | 
| minv | min volt. | uint8 (>=0<=25) | V | true | DEVICE_DATA | 5 | 1 | 1/10 | 
| maxv | max volt. | uint8 (>=0<=25) | V | true | DEVICE_DATA | 6 | 1 | 1/10 | 
| mint | min temp. | uint8 (>=0<=254) | C | true | DEVICE_DATA | 7 | 1 | 1 | 
| maxt | max temp. | uint8 (>=0<=254) | C | true | DEVICE_DATA | 8 | 1 | 1 | 
| mode | operating mode | uint8 (>=0<=254) |   | false | DEVICE_DATA | 9 | 1 | 1 | 

## Devices of type *heatpump*
### HP Module
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| airhumidity | relative air humidity | uint8 (>=0<=100) | % | false | DEVICE_DATA | 0 | 1 | 1 | 
| dewtemperature | dew point temperature | uint8 (>=0<=254) | C | false | DEVICE_DATA | 1 | 1 | 1 | 
| curflowtemp | current flow temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 2 | 1 | 1/10 | 
| rettemp | return temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 3 | 1 | 1/10 | 
| sysrettemp | system return temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 4 | 1 | 1/10 | 
| hpta4 | drain pan temp (TA4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 5 | 1 | 1/10 | 
| hptr1 | compressor temperature (TR1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| hptr3 | refrigerant temperature liquid side (condenser output) (TR3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| hptr4 | evaporator inlet temperature (TR4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| hptr5 | compressor inlet temperature (TR5) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| hptr6 | compressor outlet temperature (TR6) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| hptl2 | air inlet temperature (TL2) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| hppl1 | low pressure side temperature (PL1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| hpph1 | high pressure side temperature (PH1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 14 | 1 | 1 | 
| hpcompspd | compressor speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 15 | 1 | 1 | 
| hybridstrategy | hybrid control strategy | enum [cost optimized\|co2 optimized\|outside temperature alt.\|outside temperature parallel\|heatpump preferred\|boiler only] |   | true | DEVICE_DATA | 16 | 1 | 1 | 
| lownoisemode | low noise mode | enum [off\|reduced output\|switch off hp\|perm. reduced] |   | true | DEVICE_DATA | 17 | 1 | 1 | 
| lownoisestart | low noise starttime | uint8 (>=0<=23) | hours | true | DEVICE_DATA | 18 | 1 | 1 | 
| lownoisestop | low noise stoptime | uint8 (>=0<=23) | hours | true | DEVICE_DATA | 19 | 1 | 1 | 
| hybriddhw | hybrid DHW | enum [eco\|high comfort] |   | true | DHW | 0 | 1 | 1 | 
| energypricegas | energy price gas | uint8 (>=0<=254) | ct/kWh | true | DEVICE_DATA | 20 | 1 | 1 | 
| energypriceel | energy price electric | uint8 (>=0<=254) | ct/kWh | true | DEVICE_DATA | 21 | 1 | 1 | 
| energyfeedpv | feed in PV | uint8 (>=0<=254) | ct/kWh | true | DEVICE_DATA | 22 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 23 | 1 | 1 | 
| airpurgemode | air purge mode | boolean |   | true | DEVICE_DATA | 24 | 1 | 1 | 
| heatpumpoutput | heatpump output | uint8 (>=0<=100) | % | true | DEVICE_DATA | 25 | 1 | 1 | 
| coolingcircuit | cooling circuit | boolean |   | true | DEVICE_DATA | 26 | 1 | 1 | 
| compstartmod | compressor start modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 27 | 1 | 1 | 
| heatdrainpan | heat drain pan | boolean |   | true | DEVICE_DATA | 28 | 1 | 1 | 
| heatcable | heating cable | boolean |   | true | DEVICE_DATA | 29 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 30 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=167772) | kWh | false | DHW | 1 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 32 | 2 | 1/100 | 
| metertotal | meter total | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 34 | 2 | 1/100 | 
| metercomp | meter compressor | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 36 | 2 | 1/100 | 
| metereheat | meter e-heater | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 38 | 2 | 1/100 | 
| meterheat | meter heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 40 | 2 | 1/100 | 
| meter | meter | uint24 (>=0<=167772) | kWh | false | DHW | 3 | 2 | 1/100 | 
| heatingstarts | heating control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 42 | 2 | 1 | 
| startshp | starts hp | uint24 (>=0<=16777214) |   | false | DHW | 5 | 2 | 1 | 
| fuelheat | fuel consumption heating | uint32 (>=0<=1677721) | kWh | false | DEVICE_DATA | 44 | 2 | 1/10 | 
| fueldhw | fuel consumption | uint32 (>=0<=1677721) | kWh | false | DHW | 7 | 2 | 1/10 | 
| elheat | el. consumption heating | uint32 (>=0<=1677721) | kWh | false | DEVICE_DATA | 46 | 2 | 1/10 | 
| eldhw | el. consumption | uint32 (>=0<=1677721) | kWh | false | DHW | 9 | 2 | 1/10 | 
| elgenheat | el. generation heating | uint32 (>=0<=1677721) | kWh | false | DEVICE_DATA | 48 | 2 | 1/10 | 
| elgendhw | el generation | uint32 (>=0<=1677721) | kWh | false | DHW | 11 | 2 | 1/10 | 

### Hybrid Manager HM200
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| airhumidity | relative air humidity | uint8 (>=0<=100) | % | false | DEVICE_DATA | 0 | 1 | 1 | 
| dewtemperature | dew point temperature | uint8 (>=0<=254) | C | false | DEVICE_DATA | 1 | 1 | 1 | 
| curflowtemp | current flow temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 2 | 1 | 1/10 | 
| rettemp | return temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 3 | 1 | 1/10 | 
| sysrettemp | system return temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 4 | 1 | 1/10 | 
| hpta4 | drain pan temp (TA4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 5 | 1 | 1/10 | 
| hptr1 | compressor temperature (TR1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 6 | 1 | 1/10 | 
| hptr3 | refrigerant temperature liquid side (condenser output) (TR3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 | 
| hptr4 | evaporator inlet temperature (TR4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 | 
| hptr5 | compressor inlet temperature (TR5) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 | 
| hptr6 | compressor outlet temperature (TR6) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 | 
| hptl2 | air inlet temperature (TL2) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 11 | 1 | 1/10 | 
| hppl1 | low pressure side temperature (PL1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 | 
| hpph1 | high pressure side temperature (PH1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 | 
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 14 | 1 | 1 | 
| hpcompspd | compressor speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 15 | 1 | 1 | 
| hybridstrategy | hybrid control strategy | enum [cost optimized\|co2 optimized\|outside temperature alt.\|outside temperature parallel\|heatpump preferred\|boiler only] |   | true | DEVICE_DATA | 16 | 1 | 1 | 
| lownoisemode | low noise mode | enum [off\|reduced output\|switch off hp\|perm. reduced] |   | true | DEVICE_DATA | 17 | 1 | 1 | 
| lownoisestart | low noise starttime | uint8 (>=0<=23) | hours | true | DEVICE_DATA | 18 | 1 | 1 | 
| lownoisestop | low noise stoptime | uint8 (>=0<=23) | hours | true | DEVICE_DATA | 19 | 1 | 1 | 
| hybriddhw | hybrid DHW | enum [eco\|high comfort] |   | true | DHW | 0 | 1 | 1 | 
| energypricegas | energy price gas | uint8 (>=0<=254) | ct/kWh | true | DEVICE_DATA | 20 | 1 | 1 | 
| energypriceel | energy price electric | uint8 (>=0<=254) | ct/kWh | true | DEVICE_DATA | 21 | 1 | 1 | 
| energyfeedpv | feed in PV | uint8 (>=0<=254) | ct/kWh | true | DEVICE_DATA | 22 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 23 | 1 | 1 | 
| airpurgemode | air purge mode | boolean |   | true | DEVICE_DATA | 24 | 1 | 1 | 
| heatpumpoutput | heatpump output | uint8 (>=0<=100) | % | true | DEVICE_DATA | 25 | 1 | 1 | 
| coolingcircuit | cooling circuit | boolean |   | true | DEVICE_DATA | 26 | 1 | 1 | 
| compstartmod | compressor start modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 27 | 1 | 1 | 
| heatdrainpan | heat drain pan | boolean |   | true | DEVICE_DATA | 28 | 1 | 1 | 
| heatcable | heating cable | boolean |   | true | DEVICE_DATA | 29 | 1 | 1 | 
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 30 | 2 | 1/100 | 
| nrg | energy | uint24 (>=0<=167772) | kWh | false | DHW | 1 | 2 | 1/100 | 
| nrgheat | energy heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 32 | 2 | 1/100 | 
| metertotal | meter total | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 34 | 2 | 1/100 | 
| metercomp | meter compressor | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 36 | 2 | 1/100 | 
| metereheat | meter e-heater | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 38 | 2 | 1/100 | 
| meterheat | meter heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 40 | 2 | 1/100 | 
| meter | meter | uint24 (>=0<=167772) | kWh | false | DHW | 3 | 2 | 1/100 | 
| heatingstarts | heating control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 42 | 2 | 1 | 
| startshp | starts hp | uint24 (>=0<=16777214) |   | false | DHW | 5 | 2 | 1 | 
| fuelheat | fuel consumption heating | uint32 (>=0<=1677721) | kWh | false | DEVICE_DATA | 44 | 2 | 1/10 | 
| fueldhw | fuel consumption | uint32 (>=0<=1677721) | kWh | false | DHW | 7 | 2 | 1/10 | 
| elheat | el. consumption heating | uint32 (>=0<=1677721) | kWh | false | DEVICE_DATA | 46 | 2 | 1/10 | 
| eldhw | el. consumption | uint32 (>=0<=1677721) | kWh | false | DHW | 9 | 2 | 1/10 | 
| elgenheat | el. generation heating | uint32 (>=0<=1677721) | kWh | false | DEVICE_DATA | 48 | 2 | 1/10 | 
| elgendhw | el generation | uint32 (>=0<=1677721) | kWh | false | DHW | 11 | 2 | 1/10 | 

## Devices of type *mixer*
### HM210
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | HC | 0 | 1 | 1/10 | 
| valvestatus | mixing valve actuator (VC1) | uint8 (>=0<=100) | % | false | HC | 1 | 1 | 1 | 
| flowsettemp | setpoint flow temperature | uint8 (>=0<=254) | C | true | HC | 2 | 1 | 1 | 
| pumpstatus | pump status (PC1) | boolean |   | true | HC | 3 | 1 | 1 | 
| activated | activated | boolean |   | true | HC | 4 | 1 | 1 | 
| valvesettime | time to set valve | uint8 (>=10<=600) | seconds | true | HC | 5 | 1 | 10 | 
| flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | K | true | HC | 7 | 1 | 1 | 

### IPM
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | HC | 0 | 1 | 1/10 | 
| valvestatus | mixing valve actuator (VC1) | uint8 (>=0<=100) | % | false | HC | 1 | 1 | 1 | 
| flowsettemp | setpoint flow temperature | uint8 (>=0<=254) | C | true | HC | 2 | 1 | 1 | 
| pumpstatus | pump status (PC1) | boolean |   | true | HC | 3 | 1 | 1 | 
| flowtempvf | flow temperature in header (T0/Vf) | uint16 (>=0<=3199) | C | false | HC | 6 | 1 | 1/10 | 

### IPM2
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | HC | 0 | 1 | 1/10 | 
| valvestatus | mixing valve actuator (VC1) | uint8 (>=0<=100) | % | false | HC | 1 | 1 | 1 | 
| flowsettemp | setpoint flow temperature | uint8 (>=0<=254) | C | true | HC | 2 | 1 | 1 | 
| pumpstatus | pump status (PC1) | boolean |   | true | HC | 3 | 1 | 1 | 
| flowtempvf | flow temperature in header (T0/Vf) | uint16 (>=0<=3199) | C | false | HC | 6 | 1 | 1/10 | 

### MM10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | HC | 0 | 1 | 1/10 | 
| valvestatus | mixing valve actuator (VC1) | int8 (>=-100<=100) | % | false | HC | 1 | 1 | 1 | 
| flowsettemp | setpoint flow temperature | uint8 (>=0<=254) | C | true | HC | 2 | 1 | 1 | 
| pumpstatus | pump status (PC1) | boolean |   | true | HC | 3 | 1 | 1 | 
| activated | activated | boolean |   | true | HC | 4 | 1 | 1 | 
| valvesettime | time to set valve | uint8 (>=10<=600) | seconds | true | HC | 5 | 1 | 10 | 

### MM100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | HC | 0 | 1 | 1/10 | 
| valvestatus | mixing valve actuator (VC1) | uint8 (>=0<=100) | % | false | HC | 1 | 1 | 1 | 
| flowsettemp | setpoint flow temperature | uint8 (>=0<=254) | C | true | HC | 2 | 1 | 1 | 
| pumpstatus | pump status (PC1) | boolean |   | true | HC | 3 | 1 | 1 | 
| activated | activated | boolean |   | true | HC | 4 | 1 | 1 | 
| valvesettime | time to set valve | uint8 (>=10<=600) | seconds | true | HC | 5 | 1 | 10 | 
| flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | K | true | HC | 7 | 1 | 1 | 

### MM200
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | HC | 0 | 1 | 1/10 | 
| valvestatus | mixing valve actuator (VC1) | uint8 (>=0<=100) | % | false | HC | 1 | 1 | 1 | 
| flowsettemp | setpoint flow temperature | uint8 (>=0<=254) | C | true | HC | 2 | 1 | 1 | 
| pumpstatus | pump status (PC1) | boolean |   | true | HC | 3 | 1 | 1 | 
| activated | activated | boolean |   | true | HC | 4 | 1 | 1 | 
| valvesettime | time to set valve | uint8 (>=10<=600) | seconds | true | HC | 5 | 1 | 10 | 
| flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | K | true | HC | 7 | 1 | 1 | 

### MM50
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | HC | 0 | 1 | 1/10 | 
| valvestatus | mixing valve actuator (VC1) | uint8 (>=0<=100) | % | false | HC | 1 | 1 | 1 | 
| flowsettemp | setpoint flow temperature | uint8 (>=0<=254) | C | true | HC | 2 | 1 | 1 | 
| pumpstatus | pump status (PC1) | boolean |   | true | HC | 3 | 1 | 1 | 
| activated | activated | boolean |   | true | HC | 4 | 1 | 1 | 
| valvesettime | time to set valve | uint8 (>=10<=600) | seconds | true | HC | 5 | 1 | 10 | 
| flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | K | true | HC | 7 | 1 | 1 | 

### MZ100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | HC | 0 | 1 | 1/10 | 
| valvestatus | mixing valve actuator (VC1) | uint8 (>=0<=100) | % | false | HC | 1 | 1 | 1 | 
| flowsettemp | setpoint flow temperature | uint8 (>=0<=254) | C | true | HC | 2 | 1 | 1 | 
| pumpstatus | pump status (PC1) | boolean |   | true | HC | 3 | 1 | 1 | 
| activated | activated | boolean |   | true | HC | 4 | 1 | 1 | 
| valvesettime | time to set valve | uint8 (>=10<=600) | seconds | true | HC | 5 | 1 | 10 | 
| flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | K | true | HC | 7 | 1 | 1 | 

## Devices of type *pool*
### MP100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| pooltemp | pool temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | -1 | 1 | 1/10 | 
| poolshuntstatus | pool shunt status opening/closing | enum [stopped\|opening\|closing\|open\|close] |   | false | DEVICE_DATA | -1 | 1 | 1 | 
| poolshunt | pool shunt open/close (0% = pool / 100% = heat) | uint8 (>=0<=100) | % | false | DEVICE_DATA | -1 | 1 | 1 | 

## Devices of type *solar*
### ISM1
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| collectortemp | collector temperature (TS1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 0 | 1 | 1/10 | 
| cylbottomtemp | cylinder bottom temperature (TS2) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 1 | 1 | 1/10 | 
| solarpump | pump (PS1) | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| pumpworktime | pump working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 3 | 2 | 1 | 
| cylmaxtemp | maximum cylinder temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 5 | 1 | 1 | 
| collectorshutdown | collector shutdown | boolean |   | false | DEVICE_DATA | 6 | 1 | 1 | 
| cylheated | cyl heated | boolean |   | false | DEVICE_DATA | 7 | 1 | 1 | 
| cylmiddletemp | cylinder middle temperature (TS3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 17 | 1 | 1/10 | 
| retheatassist | return temperature heat assistance (TS4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 18 | 1 | 1/10 | 
| heatassistvalve | heat assistance valve (M1) | boolean |   | false | DEVICE_DATA | 19 | 1 | 1 | 
| energylasthour | energy last hour | uint24 (>=0<=1677721) | Wh | false | DEVICE_DATA | 13 | 2 | 1/10 | 

### ISM2
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| collectortemp | collector temperature (TS1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 0 | 1 | 1/10 | 
| cylbottomtemp | cylinder bottom temperature (TS2) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 1 | 1 | 1/10 | 
| solarpump | pump (PS1) | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| pumpworktime | pump working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 3 | 2 | 1 | 
| cylmaxtemp | maximum cylinder temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 5 | 1 | 1 | 
| collectorshutdown | collector shutdown | boolean |   | false | DEVICE_DATA | 6 | 1 | 1 | 
| cylheated | cyl heated | boolean |   | false | DEVICE_DATA | 7 | 1 | 1 | 
| cylmiddletemp | cylinder middle temperature (TS3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 17 | 1 | 1/10 | 
| retheatassist | return temperature heat assistance (TS4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 18 | 1 | 1/10 | 
| heatassistvalve | heat assistance valve (M1) | boolean |   | false | DEVICE_DATA | 19 | 1 | 1 | 
| energylasthour | energy last hour | uint24 (>=0<=1677721) | Wh | false | DEVICE_DATA | 13 | 2 | 1/10 | 

### SM10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| collectortemp | collector temperature (TS1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 0 | 1 | 1/10 | 
| cylbottomtemp | cylinder bottom temperature (TS2) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 1 | 1 | 1/10 | 
| solarpump | pump (PS1) | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| pumpworktime | pump working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 3 | 2 | 1 | 
| cylmaxtemp | maximum cylinder temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 5 | 1 | 1 | 
| collectorshutdown | collector shutdown | boolean |   | false | DEVICE_DATA | 6 | 1 | 1 | 
| cylheated | cyl heated | boolean |   | false | DEVICE_DATA | 7 | 1 | 1 | 
| solarpumpmod | pump modulation (PS1) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 8 | 1 | 1 | 
| pumpminmod | minimum pump modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 9 | 1 | 1 | 
| turnondiff | pump turn on difference | uint8 (>=0<=254) | C | true | DEVICE_DATA | 10 | 1 | 1 | 
| turnoffdiff | pump turn off difference | uint8 (>=0<=254) | C | true | DEVICE_DATA | 11 | 1 | 1 | 
| solarpower | actual solar power | int16 (>=-31999<=31999) | W | false | DEVICE_DATA | 12 | 1 | 1 | 
| energylasthour | energy last hour | uint24 (>=0<=1677721) | Wh | false | DEVICE_DATA | 13 | 2 | 1/10 | 
| maxflow | maximum solar flow | uint8 (>=0<=25) | l/min | true | DEVICE_DATA | 15 | 1 | 1/10 | 
| mintemp | minimum temperature | uint8 (>=0<=254) | C | true | DHW | 0 | 1 | 1 | 
| solarenabled | solarmodule enabled | boolean |   | true | DEVICE_DATA | 16 | 1 | 1 | 

### SM100, MS100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| collectortemp | collector temperature (TS1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 0 | 1 | 1/10 | 
| cylbottomtemp | cylinder bottom temperature (TS2) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 1 | 1 | 1/10 | 
| solarpump | pump (PS1) | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| pumpworktime | pump working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 3 | 2 | 1 | 
| cylmaxtemp | maximum cylinder temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 5 | 1 | 1 | 
| collectorshutdown | collector shutdown | boolean |   | false | DEVICE_DATA | 6 | 1 | 1 | 
| cylheated | cyl heated | boolean |   | false | DEVICE_DATA | 7 | 1 | 1 | 
| solarpumpmod | pump modulation (PS1) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 8 | 1 | 1 | 
| pumpminmod | minimum pump modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 9 | 1 | 5 | 
| turnondiff | pump turn on difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 10 | 1 | 1/10 | 
| turnoffdiff | pump turn off difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 11 | 1 | 1/10 | 
| collector2temp | collector 2 temperature (TS7) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 20 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 17 | 1 | 1/10 | 
| retheatassist | return temperature heat assistance (TS4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 18 | 1 | 1/10 | 
| heatassistvalve | heat assistance valve (M1) | boolean |   | false | DEVICE_DATA | 19 | 1 | 1 | 
| heatassistpower | heat assistance valve power (M1) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 21 | 1 | 1 | 
| solarpump2 | pump 2 (PS4) | boolean |   | false | DEVICE_DATA | 22 | 1 | 1 | 
| solarpump2mod | pump 2 modulation (PS4) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| cyl2bottomtemp | second cylinder bottom temperature (TS5) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 24 | 1 | 1/10 | 
| cyl3bottomtemp | third cylinder bottom temperature (TS11) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 25 | 1 | 1/10 | 
| cyltoptemp | cylinder top temperature (TS10) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 26 | 1 | 1/10 | 
| heatexchangertemp | heat exchanger temperature (TS6) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 27 | 1 | 1/10 | 
| cylpumpmod | cylinder pump modulation (PS5) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 28 | 1 | 1 | 
| valvestatus | valve status | boolean |   | false | DEVICE_DATA | 29 | 1 | 1 | 
| vs1status | valve status VS1 | boolean |   | false | DEVICE_DATA | 30 | 1 | 1 | 
| vs3status | valve status VS3 | boolean |   | false | DEVICE_DATA | 31 | 1 | 1 | 
| transferpump | transfer pump | boolean |   | false | DEVICE_DATA | 32 | 1 | 1 | 
| transferpumpmod | transfer pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 33 | 1 | 1 | 
| collectormaxtemp | maximum collector temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 34 | 1 | 1 | 
| collectormintemp | minimum collector temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 35 | 1 | 1 | 
| energylasthour | energy last hour | uint24 (>=0<=1677721) | Wh | false | DEVICE_DATA | 13 | 2 | 1/10 | 
| energytoday | total energy today | uint24 (>=0<=16777214) | Wh | false | DEVICE_DATA | 36 | 2 | 1 | 
| energytotal | total energy | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 38 | 2 | 1/10 | 
| pump2worktime | pump 2 working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 40 | 2 | 1 | 
| m1worktime | differential control working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 42 | 2 | 1 | 
| heattransfersystem | heattransfer system | boolean |   | true | DEVICE_DATA | 44 | 1 | 1 | 
| externalcyl | external cylinder | boolean |   | true | DEVICE_DATA | 45 | 1 | 1 | 
| thermaldisinfect | thermal disinfection | boolean |   | true | DEVICE_DATA | 46 | 1 | 1 | 
| heatmetering | heatmetering | boolean |   | true | DEVICE_DATA | 47 | 1 | 1 | 
| activated | activated | boolean |   | true | DEVICE_DATA | 48 | 1 | 1 | 
| solarpumpmode | solar pump mode | enum [constant\|pwm\|analog] |   | true | DEVICE_DATA | 49 | 1 | 1 | 
| solarpumpkick | solar pump kick | boolean |   | true | DEVICE_DATA | 50 | 1 | 1 | 
| plainwatermode | plain water mode | boolean |   | true | DEVICE_DATA | 51 | 1 | 1 | 
| doublematchflow | doublematchflow | boolean |   | true | DEVICE_DATA | 52 | 1 | 1 | 
| pump2minmod | minimum pump 2 modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 53 | 1 | 1 | 
| turnondiff2 | pump 2 turn on difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 54 | 1 | 1/10 | 
| turnoffdiff2 | pump 2 turn off difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 55 | 1 | 1/10 | 
| pump2kick | pump kick 2 | boolean |   | true | DEVICE_DATA | 56 | 1 | 1 | 
| climatezone | climate zone | uint8 (>=0<=254) |   | true | DEVICE_DATA | 57 | 1 | 1 | 
| collector1area | collector 1 area | uint16 (>=0<=3199) | m² | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| collector1type | collector 1 type | enum [flat\|vacuum] |   | true | DEVICE_DATA | 59 | 1 | 1 | 
| collector2area | collector 2 area | uint16 (>=0<=3199) | m² | true | DEVICE_DATA | 60 | 1 | 1/10 | 
| collector2type | collector 2 type | enum [flat\|vacuum] |   | true | DEVICE_DATA | 61 | 1 | 1 | 
| cylpriority | cylinder priority | enum [cyl 1\|cyl 2] |   | true | DEVICE_DATA | 62 | 1 | 1 | 
| heatcntflowtemp | heat counter flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 63 | 1 | 1/10 | 
| heatcntrettemp | heat counter return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 64 | 1 | 1/10 | 
| heatcnt | heat counter impulses | uint8 (>=0<=254) |   | false | DEVICE_DATA | 65 | 1 | 1 | 
| swapflowtemp | swap flow temperature (TS14) | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 66 | 1 | 1/10 | 
| swaprettemp | swap return temperature (TS15) | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 67 | 1 | 1/10 | 

### SM200, MS200
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| collectortemp | collector temperature (TS1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 0 | 1 | 1/10 | 
| cylbottomtemp | cylinder bottom temperature (TS2) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 1 | 1 | 1/10 | 
| solarpump | pump (PS1) | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| pumpworktime | pump working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 3 | 2 | 1 | 
| cylmaxtemp | maximum cylinder temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 5 | 1 | 1 | 
| collectorshutdown | collector shutdown | boolean |   | false | DEVICE_DATA | 6 | 1 | 1 | 
| cylheated | cyl heated | boolean |   | false | DEVICE_DATA | 7 | 1 | 1 | 
| solarpumpmod | pump modulation (PS1) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 8 | 1 | 1 | 
| pumpminmod | minimum pump modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 9 | 1 | 5 | 
| turnondiff | pump turn on difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 10 | 1 | 1/10 | 
| turnoffdiff | pump turn off difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 11 | 1 | 1/10 | 
| collector2temp | collector 2 temperature (TS7) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 20 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 17 | 1 | 1/10 | 
| retheatassist | return temperature heat assistance (TS4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 18 | 1 | 1/10 | 
| heatassistvalve | heat assistance valve (M1) | boolean |   | false | DEVICE_DATA | 19 | 1 | 1 | 
| heatassistpower | heat assistance valve power (M1) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 21 | 1 | 1 | 
| solarpump2 | pump 2 (PS4) | boolean |   | false | DEVICE_DATA | 22 | 1 | 1 | 
| solarpump2mod | pump 2 modulation (PS4) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| cyl2bottomtemp | second cylinder bottom temperature (TS5) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 24 | 1 | 1/10 | 
| cyl3bottomtemp | third cylinder bottom temperature (TS11) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 25 | 1 | 1/10 | 
| cyltoptemp | cylinder top temperature (TS10) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 26 | 1 | 1/10 | 
| heatexchangertemp | heat exchanger temperature (TS6) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 27 | 1 | 1/10 | 
| cylpumpmod | cylinder pump modulation (PS5) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 28 | 1 | 1 | 
| valvestatus | valve status | boolean |   | false | DEVICE_DATA | 29 | 1 | 1 | 
| vs1status | valve status VS1 | boolean |   | false | DEVICE_DATA | 30 | 1 | 1 | 
| vs3status | valve status VS3 | boolean |   | false | DEVICE_DATA | 31 | 1 | 1 | 
| transferpump | transfer pump | boolean |   | false | DEVICE_DATA | 32 | 1 | 1 | 
| transferpumpmod | transfer pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 33 | 1 | 1 | 
| collectormaxtemp | maximum collector temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 34 | 1 | 1 | 
| collectormintemp | minimum collector temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 35 | 1 | 1 | 
| energylasthour | energy last hour | uint24 (>=0<=1677721) | Wh | false | DEVICE_DATA | 13 | 2 | 1/10 | 
| energytoday | total energy today | uint24 (>=0<=16777214) | Wh | false | DEVICE_DATA | 36 | 2 | 1 | 
| energytotal | total energy | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 38 | 2 | 1/10 | 
| pump2worktime | pump 2 working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 40 | 2 | 1 | 
| m1worktime | differential control working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 42 | 2 | 1 | 
| heattransfersystem | heattransfer system | boolean |   | true | DEVICE_DATA | 44 | 1 | 1 | 
| externalcyl | external cylinder | boolean |   | true | DEVICE_DATA | 45 | 1 | 1 | 
| thermaldisinfect | thermal disinfection | boolean |   | true | DEVICE_DATA | 46 | 1 | 1 | 
| heatmetering | heatmetering | boolean |   | true | DEVICE_DATA | 47 | 1 | 1 | 
| activated | activated | boolean |   | true | DEVICE_DATA | 48 | 1 | 1 | 
| solarpumpmode | solar pump mode | enum [constant\|pwm\|analog] |   | true | DEVICE_DATA | 49 | 1 | 1 | 
| solarpumpkick | solar pump kick | boolean |   | true | DEVICE_DATA | 50 | 1 | 1 | 
| plainwatermode | plain water mode | boolean |   | true | DEVICE_DATA | 51 | 1 | 1 | 
| doublematchflow | doublematchflow | boolean |   | true | DEVICE_DATA | 52 | 1 | 1 | 
| pump2minmod | minimum pump 2 modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 53 | 1 | 1 | 
| turnondiff2 | pump 2 turn on difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 54 | 1 | 1/10 | 
| turnoffdiff2 | pump 2 turn off difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 55 | 1 | 1/10 | 
| pump2kick | pump kick 2 | boolean |   | true | DEVICE_DATA | 56 | 1 | 1 | 
| climatezone | climate zone | uint8 (>=0<=254) |   | true | DEVICE_DATA | 57 | 1 | 1 | 
| collector1area | collector 1 area | uint16 (>=0<=3199) | m² | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| collector1type | collector 1 type | enum [flat\|vacuum] |   | true | DEVICE_DATA | 59 | 1 | 1 | 
| collector2area | collector 2 area | uint16 (>=0<=3199) | m² | true | DEVICE_DATA | 60 | 1 | 1/10 | 
| collector2type | collector 2 type | enum [flat\|vacuum] |   | true | DEVICE_DATA | 61 | 1 | 1 | 
| cylpriority | cylinder priority | enum [cyl 1\|cyl 2] |   | true | DEVICE_DATA | 62 | 1 | 1 | 
| heatcntflowtemp | heat counter flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 63 | 1 | 1/10 | 
| heatcntrettemp | heat counter return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 64 | 1 | 1/10 | 
| heatcnt | heat counter impulses | uint8 (>=0<=254) |   | false | DEVICE_DATA | 65 | 1 | 1 | 
| swapflowtemp | swap flow temperature (TS14) | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 66 | 1 | 1/10 | 
| swaprettemp | swap return temperature (TS15) | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 67 | 1 | 1/10 | 

### SM50
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| collectortemp | collector temperature (TS1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 0 | 1 | 1/10 | 
| cylbottomtemp | cylinder bottom temperature (TS2) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 1 | 1 | 1/10 | 
| solarpump | pump (PS1) | boolean |   | false | DEVICE_DATA | 2 | 1 | 1 | 
| pumpworktime | pump working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 3 | 2 | 1 | 
| cylmaxtemp | maximum cylinder temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 5 | 1 | 1 | 
| collectorshutdown | collector shutdown | boolean |   | false | DEVICE_DATA | 6 | 1 | 1 | 
| cylheated | cyl heated | boolean |   | false | DEVICE_DATA | 7 | 1 | 1 | 
| solarpumpmod | pump modulation (PS1) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 8 | 1 | 1 | 
| pumpminmod | minimum pump modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 9 | 1 | 5 | 
| turnondiff | pump turn on difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 10 | 1 | 1/10 | 
| turnoffdiff | pump turn off difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 11 | 1 | 1/10 | 
| collector2temp | collector 2 temperature (TS7) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 20 | 1 | 1/10 | 
| cylmiddletemp | cylinder middle temperature (TS3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 17 | 1 | 1/10 | 
| retheatassist | return temperature heat assistance (TS4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 18 | 1 | 1/10 | 
| heatassistvalve | heat assistance valve (M1) | boolean |   | false | DEVICE_DATA | 19 | 1 | 1 | 
| heatassistpower | heat assistance valve power (M1) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 21 | 1 | 1 | 
| solarpump2 | pump 2 (PS4) | boolean |   | false | DEVICE_DATA | 22 | 1 | 1 | 
| solarpump2mod | pump 2 modulation (PS4) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 | 
| cyl2bottomtemp | second cylinder bottom temperature (TS5) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 24 | 1 | 1/10 | 
| cyl3bottomtemp | third cylinder bottom temperature (TS11) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 25 | 1 | 1/10 | 
| cyltoptemp | cylinder top temperature (TS10) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 26 | 1 | 1/10 | 
| heatexchangertemp | heat exchanger temperature (TS6) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 27 | 1 | 1/10 | 
| cylpumpmod | cylinder pump modulation (PS5) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 28 | 1 | 1 | 
| valvestatus | valve status | boolean |   | false | DEVICE_DATA | 29 | 1 | 1 | 
| vs1status | valve status VS1 | boolean |   | false | DEVICE_DATA | 30 | 1 | 1 | 
| vs3status | valve status VS3 | boolean |   | false | DEVICE_DATA | 31 | 1 | 1 | 
| transferpump | transfer pump | boolean |   | false | DEVICE_DATA | 32 | 1 | 1 | 
| transferpumpmod | transfer pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 33 | 1 | 1 | 
| collectormaxtemp | maximum collector temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 34 | 1 | 1 | 
| collectormintemp | minimum collector temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 35 | 1 | 1 | 
| energylasthour | energy last hour | uint24 (>=0<=1677721) | Wh | false | DEVICE_DATA | 13 | 2 | 1/10 | 
| energytoday | total energy today | uint24 (>=0<=16777214) | Wh | false | DEVICE_DATA | 36 | 2 | 1 | 
| energytotal | total energy | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 38 | 2 | 1/10 | 
| pump2worktime | pump 2 working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 40 | 2 | 1 | 
| m1worktime | differential control working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 42 | 2 | 1 | 
| heattransfersystem | heattransfer system | boolean |   | true | DEVICE_DATA | 44 | 1 | 1 | 
| externalcyl | external cylinder | boolean |   | true | DEVICE_DATA | 45 | 1 | 1 | 
| thermaldisinfect | thermal disinfection | boolean |   | true | DEVICE_DATA | 46 | 1 | 1 | 
| heatmetering | heatmetering | boolean |   | true | DEVICE_DATA | 47 | 1 | 1 | 
| activated | activated | boolean |   | true | DEVICE_DATA | 48 | 1 | 1 | 
| solarpumpmode | solar pump mode | enum [constant\|pwm\|analog] |   | true | DEVICE_DATA | 49 | 1 | 1 | 
| solarpumpkick | solar pump kick | boolean |   | true | DEVICE_DATA | 50 | 1 | 1 | 
| plainwatermode | plain water mode | boolean |   | true | DEVICE_DATA | 51 | 1 | 1 | 
| doublematchflow | doublematchflow | boolean |   | true | DEVICE_DATA | 52 | 1 | 1 | 
| pump2minmod | minimum pump 2 modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 53 | 1 | 1 | 
| turnondiff2 | pump 2 turn on difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 54 | 1 | 1/10 | 
| turnoffdiff2 | pump 2 turn off difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 55 | 1 | 1/10 | 
| pump2kick | pump kick 2 | boolean |   | true | DEVICE_DATA | 56 | 1 | 1 | 
| climatezone | climate zone | uint8 (>=0<=254) |   | true | DEVICE_DATA | 57 | 1 | 1 | 
| collector1area | collector 1 area | uint16 (>=0<=3199) | m² | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| collector1type | collector 1 type | enum [flat\|vacuum] |   | true | DEVICE_DATA | 59 | 1 | 1 | 
| collector2area | collector 2 area | uint16 (>=0<=3199) | m² | true | DEVICE_DATA | 60 | 1 | 1/10 | 
| collector2type | collector 2 type | enum [flat\|vacuum] |   | true | DEVICE_DATA | 61 | 1 | 1 | 
| cylpriority | cylinder priority | enum [cyl 1\|cyl 2] |   | true | DEVICE_DATA | 62 | 1 | 1 | 
| heatcntflowtemp | heat counter flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 63 | 1 | 1/10 | 
| heatcntrettemp | heat counter return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 64 | 1 | 1/10 | 
| heatcnt | heat counter impulses | uint8 (>=0<=254) |   | false | DEVICE_DATA | 65 | 1 | 1 | 
| swapflowtemp | swap flow temperature (TS14) | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 66 | 1 | 1/10 | 
| swaprettemp | swap return temperature (TS15) | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 67 | 1 | 1/10 | 

## Devices of type *switch*
### WM10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| activated | activated | boolean |   | false | DEVICE_DATA | 0 | 1 | 1 | 
| flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 1 | 1 | 1/10 | 
| status | status | int8 (>=-126<=126) |   | false | DEVICE_DATA | 2 | 1 | 1 | 

## Devices of type *thermostat*
### CR11
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [manual\|auto] |   | false | HC | 3 | 1 | 1 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 

### CRF200S
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| seltemp | selected room temperature | int16 (>=-15999<=15999) | C | false | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [auto\|off] |   | false | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [off\|on] |   | false | HC | 4 | 1 | 1 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 

### Comfort RF
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| seltemp | selected room temperature | int16 (>=-15999<=15999) | C | false | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [auto\|off] |   | false | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [off\|on] |   | false | HC | 4 | 1 | 1 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 

### Comfort+2RF
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| seltemp | selected room temperature | int16 (>=-15999<=15999) | C | false | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [auto\|off] |   | false | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [off\|on] |   | false | HC | 4 | 1 | 1 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 

### ES72, RC20
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 | 
| daytemp | day temperature | uint8 (>=0<=127) | C | true | HC | 53 | 1 | 1/2 | 
| nighttemp | night temperature | uint8 (>=0<=127) | C | true | HC | 54 | 1 | 1/2 | 
| program | program | enum [family\|morning\|evening\|am\|pm\|midday\|singles\|seniors] |   | true | HC | 25 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 | 
| tempautotemp | temporary set temperature automode | uint8 (>=0<=127) | C | true | HC | 26 | 1 | 1/2 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 | 
| summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 | 
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 | 
| daytemp | day temperature | uint8 (>=0<=127) | C | true | HC | 53 | 1 | 1/2 | 
| nighttemp | night temperature | uint8 (>=0<=127) | C | true | HC | 54 | 1 | 1/2 | 
| program | program | enum [family\|morning\|evening\|am\|pm\|midday\|singles\|seniors] |   | true | HC | 25 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 | 
| tempautotemp | temporary set temperature automode | uint8 (>=0<=127) | C | true | HC | 26 | 1 | 1/2 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 | 
| summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 | 

### ES73
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| display | display | enum [internal temperature\|internal setpoint\|external temperature\|burner temperature\|dhw temperature\|functioning mode\|time\|date\|smoke temperature] |   | true | DEVICE_DATA | 65 | 1 | 1 | 
| language | language | enum [german\|dutch\|french\|italian] |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| clockoffset | clock offset | int8 (>=-126<=126) | seconds | true | DEVICE_DATA | 67 | 1 | 1 | 
| intoffset | internal temperature offset | int8 (>=-12<=12) | C | true | DEVICE_DATA | 46 | 1 | 1/10 | 
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 | 
| inttemp1 | temperature sensor 1 | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 68 | 1 | 1/10 | 
| inttemp2 | temperature sensor 2 | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 69 | 1 | 1/10 | 
| damping | damping outdoor temperature | boolean |   | true | DEVICE_DATA | 52 | 1 | 1 | 
| dampedoutdoortemp | damped outdoor temperature | int8 (>=-126<=126) | C | false | DEVICE_DATA | 48 | 1 | 1 | 
| building | building type | enum [light\|medium\|heavy] |   | true | DEVICE_DATA | 50 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 | 
| daytemp | day temperature | uint8 (>=10<=30) | C | true | HC | 53 | 1 | 1/2 | 
| nighttemp | night temperature | uint8 (>=10<=30) | C | true | HC | 54 | 1 | 1/2 | 
| designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 | 
| offsettemp | offset temperature | int8 (>=-5<=5) | C | true | HC | 10 | 1 | 1/2 | 
| holidaytemp | holiday temperature | uint8 (>=5<=30) | C | true | HC | 55 | 1 | 1/2 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1 | 
| summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 | 
| holidaymode | holiday mode | boolean |   | false | HC | 56 | 1 | 1 | 
| nofrosttemp | nofrost temperature | int8 (>=-20<=10) | C | true | HC | 17 | 1 | 1 | 
| nofrostmode | nofrost mode | enum [off\|outdoor\|room] |   | true | HC | 16 | 1 | 1 | 
| roominfluence | room influence | uint8 (>=0<=10) | C | true | HC | 13 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 | 
| flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | C | true | HC | 57 | 1 | 1 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| reducemode | reduce mode | enum [nofrost\|reduce\|room\|outdoor] |   | true | HC | 30 | 1 | 1 | 
| controlmode | control mode | enum [outdoor\|room] |   | true | HC | 24 | 1 | 1 | 
| control | control device | enum [off\|RC20\|RC3x] |   | true | HC | 40 | 1 | 1 | 
| holidays | holiday dates | string |   | true | HC | 58 | 13 | 1 | 
| vacations | vacation dates | string |   | true | HC | 71 | 13 | 1 | 
| program | program | enum [own 1\|family\|morning\|evening\|am\|pm\|midday\|singles\|seniors\|new\|own 2] |   | true | HC | 25 | 1 | 1 | 
| pause | pause time | uint8 (>=0<=99) | hours | true | HC | 84 | 1 | 1 | 
| party | party time | uint8 (>=0<=99) | hours | true | HC | 85 | 1 | 1 | 
| tempautotemp | temporary set temperature automode | uint8 (>=0<=30) | C | true | HC | 26 | 1 | 1/2 | 
| noreducetemp | no reduce below temperature | int8 (>=-31<=10) | C | true | HC | 31 | 1 | 1 | 
| reducetemp | off/reduce switch temperature | int8 (>=-20<=10) | C | true | HC | 32 | 1 | 1 | 
| vacreducetemp | vacations off/reduce switch temperature | int8 (>=-20<=10) | C | true | HC | 86 | 1 | 1 | 
| vacreducemode | vacations reduce mode | enum [nofrost\|reduce\|room\|outdoor] |   | true | HC | 87 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| dhwprio | dhw priority | boolean |   | true | HC | 33 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| switchtime1 | own1 program switchtime | string |   | true | HC | 88 | 8 | 1 | 
| switchtime2 | own2 program switchtime | string |   | true | HC | 96 | 8 | 1 | 
| mode | operating mode | enum [off\|on\|auto] |   | true | DHW | 0 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|on\|auto] |   | true | DHW | 3 | 1 | 1 | 
| progmode | program | enum [std prog\|own prog] |   | true | DHW | 12 | 1 | 1 | 
| circprog | circulation program | enum [std prog\|own prog] |   | true | DHW | 13 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 7 | 1 | 1 | 
| disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 8 | 1 | 1 | 
| disinfecthour | disinfection hour | uint8 (>=0<=23) |   | true | DHW | 14 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=254) | C | true | DHW | 15 | 1 | 1 | 
| onetimekey | one time key function | boolean |   | true | DHW | 16 | 1 | 1 | 
| switchtimeWW | program switchtime warm water | string |   | true | DHW | 17 | 11 | 1 | 
| circswitchtime | circulation program switchtime | string |   | true | DHW | 28 | 8 | 1 | 
| holidays | holiday dates | string |   | true | DHW | 36 | 13 | 1 | 
| vacations | vacation dates | string |   | true | DHW | 49 | 13 | 1 | 

### ES79
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 | 
| intoffset | internal temperature offset | int8 (>=-5<=5) | C | true | DEVICE_DATA | 46 | 1 | 1/10 | 
| minexttemp | minimal external temperature | int8 (>=-30<=0) | C | true | DEVICE_DATA | 51 | 1 | 1 | 
| inttemp1 | temperature sensor 1 | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 68 | 1 | 1/10 | 
| inttemp2 | temperature sensor 2 | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 69 | 1 | 1/10 | 
| damping | damping outdoor temperature | boolean |   | true | DEVICE_DATA | 52 | 1 | 1 | 
| dampedoutdoortemp | damped outdoor temperature | int8 (>=-126<=126) | C | false | DEVICE_DATA | 48 | 1 | 1 | 
| building | building type | enum [light\|medium\|heavy] |   | true | DEVICE_DATA | 50 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 | 
| daytemp | day temperature | uint8 (>=10<=30) | C | true | HC | 53 | 1 | 1/2 | 
| nighttemp | night temperature | uint8 (>=10<=30) | C | true | HC | 54 | 1 | 1/2 | 
| designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 | 
| offsettemp | offset temperature | int8 (>=-5<=5) | C | true | HC | 10 | 1 | 1/2 | 
| holidaytemp | holiday temperature | uint8 (>=5<=30) | C | true | HC | 55 | 1 | 1/2 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1 | 
| summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 | 
| holidaymode | holiday mode | boolean |   | false | HC | 56 | 1 | 1 | 
| nofrosttemp | nofrost temperature | int8 (>=-20<=10) | C | true | HC | 17 | 1 | 1 | 
| nofrostmode | nofrost mode | enum [off\|outdoor\|room] |   | true | HC | 16 | 1 | 1 | 
| roominfluence | room influence | uint8 (>=0<=10) | C | true | HC | 13 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 | 
| flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | C | true | HC | 57 | 1 | 1 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| reducemode | reduce mode | enum [nofrost\|reduce\|room\|outdoor] |   | true | HC | 30 | 1 | 1 | 
| controlmode | control mode | enum [outdoor\|room] |   | true | HC | 24 | 1 | 1 | 
| control | control device | enum [off\|RC20\|RC3x] |   | true | HC | 40 | 1 | 1 | 
| holidays | holiday dates | string |   | true | HC | 58 | 13 | 1 | 
| vacations | vacation dates | string |   | true | HC | 71 | 13 | 1 | 
| program | program | enum [own 1\|family\|morning\|evening\|am\|pm\|midday\|singles\|seniors\|new\|own 2] |   | true | HC | 25 | 1 | 1 | 
| pause | pause time | uint8 (>=0<=99) | hours | true | HC | 84 | 1 | 1 | 
| party | party time | uint8 (>=0<=99) | hours | true | HC | 85 | 1 | 1 | 
| tempautotemp | temporary set temperature automode | uint8 (>=0<=30) | C | true | HC | 26 | 1 | 1/2 | 
| noreducetemp | no reduce below temperature | int8 (>=-31<=10) | C | true | HC | 31 | 1 | 1 | 
| reducetemp | off/reduce switch temperature | int8 (>=-20<=10) | C | true | HC | 32 | 1 | 1 | 
| vacreducetemp | vacations off/reduce switch temperature | int8 (>=-20<=10) | C | true | HC | 86 | 1 | 1 | 
| vacreducemode | vacations reduce mode | enum [nofrost\|reduce\|room\|outdoor] |   | true | HC | 87 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| dhwprio | dhw priority | boolean |   | true | HC | 33 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| switchtime1 | own1 program switchtime | string |   | true | HC | 88 | 8 | 1 | 
| switchtime2 | own2 program switchtime | string |   | true | HC | 96 | 8 | 1 | 
| mode | operating mode | enum [off\|on\|auto] |   | true | DHW | 0 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|on\|auto] |   | true | DHW | 3 | 1 | 1 | 
| progmode | program | enum [std prog\|own prog] |   | true | DHW | 12 | 1 | 1 | 
| circprog | circulation program | enum [std prog\|own prog] |   | true | DHW | 13 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 7 | 1 | 1 | 
| disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 8 | 1 | 1 | 
| disinfecthour | disinfection hour | uint8 (>=0<=23) |   | true | DHW | 14 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=60<=80) | C | true | DHW | 15 | 1 | 1 | 
| onetimekey | one time key function | boolean |   | true | DHW | 16 | 1 | 1 | 
| switchtimeWW | program switchtime warm water | string |   | true | DHW | 17 | 11 | 1 | 
| circswitchtime | circulation program switchtime | string |   | true | DHW | 28 | 8 | 1 | 
| holidays | holiday dates | string |   | true | DHW | 36 | 13 | 1 | 
| vacations | vacation dates | string |   | true | DHW | 49 | 13 | 1 | 

### EasyControl, CT200
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| seltemp | selected room temperature | int16 (>=-319<=319) | C | false | HC | 0 | 1 | 1/100 | 
| currtemp | current room temperature | int16 (>=-319<=319) | C | false | HC | 1 | 1 | 1/100 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 

### FB10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 | 
| heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 195 | 1 | 1/2 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 | 
| control | control device | enum [off\|FB10\|FB100] |   | true | HC | 40 | 1 | 1 | 
| program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 25 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 | 
| roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 196 | 1 | 1 | 
| holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 56 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 197 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 | 
| roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 | 
| heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 24 | 1 | 1 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 

### FB100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 | 
| heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 195 | 1 | 1/2 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 | 
| control | control device | enum [off\|FB10\|FB100] |   | true | HC | 40 | 1 | 1 | 
| program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 25 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 | 
| roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 196 | 1 | 1 | 
| holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 56 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 197 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 | 
| roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 | 
| heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 24 | 1 | 1 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 

### FR10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 | 
| heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 195 | 1 | 1/2 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 | 
| control | control device | enum [off\|FB10\|FB100] |   | true | HC | 40 | 1 | 1 | 
| program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 25 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 | 
| roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 196 | 1 | 1 | 
| holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 56 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 197 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 | 
| roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 | 
| heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 24 | 1 | 1 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 

### FR100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 | 
| heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 195 | 1 | 1/2 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 | 
| control | control device | enum [off\|FB10\|FB100] |   | true | HC | 40 | 1 | 1 | 
| program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 25 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 | 
| roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 196 | 1 | 1 | 
| holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 56 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 197 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 | 
| roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 | 
| heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 24 | 1 | 1 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 

### FR110
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 | 
| heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 195 | 1 | 1/2 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 | 
| control | control device | enum [off\|FB10\|FB100] |   | true | HC | 40 | 1 | 1 | 
| program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 25 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 | 
| roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 196 | 1 | 1 | 
| holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 56 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 197 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 | 
| roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 | 
| heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 24 | 1 | 1 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 

### FR120
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 | 
| heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 195 | 1 | 1/2 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 | 
| control | control device | enum [off\|FB10\|FB100] |   | true | HC | 40 | 1 | 1 | 
| program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 25 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 | 
| roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 196 | 1 | 1 | 
| holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 56 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 197 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 | 
| roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 | 
| heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 24 | 1 | 1 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 

### FR50
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 | 
| heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 195 | 1 | 1/2 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 | 
| control | control device | enum [off\|FB10\|FB100] |   | true | HC | 40 | 1 | 1 | 
| program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 25 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 | 
| roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 196 | 1 | 1 | 
| holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 56 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 197 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 | 
| roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 | 
| heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 24 | 1 | 1 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 

### FW100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 | 
| heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 195 | 1 | 1/2 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 | 
| control | control device | enum [off\|FB10\|FB100] |   | true | HC | 40 | 1 | 1 | 
| program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 25 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 | 
| roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 196 | 1 | 1 | 
| holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 56 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 197 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 | 
| roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 | 
| heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 24 | 1 | 1 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 

### FW120
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 | 
| heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 195 | 1 | 1/2 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 | 
| control | control device | enum [off\|FB10\|FB100] |   | true | HC | 40 | 1 | 1 | 
| program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 25 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 | 
| roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 196 | 1 | 1 | 
| holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 56 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 197 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 | 
| roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 | 
| heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 24 | 1 | 1 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 

### FW200
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 | 
| heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 195 | 1 | 1/2 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 | 
| control | control device | enum [off\|FB10\|FB100] |   | true | HC | 40 | 1 | 1 | 
| program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 25 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 | 
| roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 196 | 1 | 1 | 
| holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 56 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 197 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 | 
| roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 | 
| heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 24 | 1 | 1 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 

### FW500
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 | 
| heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 195 | 1 | 1/2 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 | 
| control | control device | enum [off\|FB10\|FB100] |   | true | HC | 40 | 1 | 1 | 
| program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 25 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 | 
| roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 196 | 1 | 1 | 
| holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 56 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 197 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 | 
| roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 | 
| heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 24 | 1 | 1 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 

### Logamatic TC100, Moduline Easy
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| seltemp | selected room temperature | int16 (>=-319<=319) | C | false | HC | 0 | 1 | 1/100 | 
| currtemp | current room temperature | int16 (>=-319<=319) | C | false | HC | 1 | 1 | 1/100 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 

### Moduline 200
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| intoffset | internal temperature offset | int8 (>=-12<=12) | C | true | DEVICE_DATA | 46 | 1 | 1/10 | 
| heatingpid | heating PID | enum [fast\|medium\|slow] |   | true | DEVICE_DATA | 74 | 1 | 1 | 
| backlight | key backlight | boolean |   | true | DEVICE_DATA | 71 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [nofrost\|night\|day] |   | true | HC | 3 | 1 | 1 | 
| daytemp | day temperature | uint8 (>=0<=127) | C | true | HC | 53 | 1 | 1/2 | 
| nighttemp | night temperature | uint8 (>=0<=127) | C | true | HC | 54 | 1 | 1/2 | 
| reducehours | duration for nighttemp | uint8 (>=0<=254) | hours | true | HC | 193 | 1 | 1 | 
| reduceminutes | remaining time for nightmode | uint16 (>=0<=31999) | minutes | false | HC | 194 | 1 | 1 | 
| mode | operating mode | enum [on\|off\|auto] |   | true | DHW | 0 | 1 | 1 | 

### Moduline 400
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| clockoffset | clock offset | int8 (>=-126<=126) | seconds | true | DEVICE_DATA | 67 | 1 | 1 | 
| autodst | automatic change daylight saving time | boolean |   | true | DEVICE_DATA | 70 | 1 | 1 | 
| language | language | enum [german\|dutch] |   | true | DEVICE_DATA | 66 | 1 | 1 | 
| backlight | key backlight | boolean |   | true | DEVICE_DATA | 71 | 1 | 1 | 
| brightness | screen brightness | int8 (>=-15<=15) |   | true | DEVICE_DATA | 72 | 1 | 1 | 
| mixingvalves | mixing valves | uint8 (>=0<=2) |   | true | DEVICE_DATA | 73 | 1 | 1 | 
| building | building type | enum [light\|medium\|heavy] |   | true | DEVICE_DATA | 50 | 1 | 1 | 
| heatingpid | heating PID | enum [fast\|medium\|slow] |   | true | DEVICE_DATA | 74 | 1 | 1 | 
| preheating | preheating in the clock program | boolean |   | true | DEVICE_DATA | 75 | 1 | 1 | 
| intoffset | internal temperature offset | int8 (>=-12<=12) | C | true | DEVICE_DATA | 46 | 1 | 1/10 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [off\|manual\|auto] |   | true | HC | 3 | 1 | 1 | 
| pause | pause time | uint8 (>=0<=254) | hours | true | HC | 84 | 1 | 1 | 
| party | party time | uint8 (>=0<=254) | hours | true | HC | 85 | 1 | 1 | 
| vacations1 | vacation dates 1 | string |   | true | HC | 116 | 11 | 1 | 
| vacations2 | vacation dates 2 | string |   | true | HC | 127 | 11 | 1 | 
| vacations3 | vacation dates 3 | string |   | true | HC | 138 | 11 | 1 | 
| vacations4 | vacation dates 4 | string |   | true | HC | 149 | 11 | 1 | 
| vacations5 | vacation dates 5 | string |   | true | HC | 160 | 11 | 1 | 
| vacations6 | vacation dates 6 | string |   | true | HC | 171 | 11 | 1 | 
| vacations7 | vacation dates 7 | string |   | true | HC | 182 | 11 | 1 | 
| program | program | enum [own 1\|family\|morning\|evening\|am\|pm\|midday\|singles\|seniors\|new\|own 2] |   | true | HC | 25 | 1 | 1 | 
| switchtime1 | own1 program switchtime | string |   | true | HC | 88 | 8 | 1 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| controlmode | control mode | enum [outdoor\|room] |   | true | HC | 24 | 1 | 1 | 
| holidaytemp | holiday temperature | uint8 (>=0<=127) | C | true | HC | 55 | 1 | 1/2 | 
| nighttemp | night temperature T1 | uint8 (>=0<=127) | C | true | HC | 54 | 1 | 1/2 | 
| daytemp2 | day temperature T2 | uint8 (>=0<=127) | C | true | HC | 105 | 1 | 1/2 | 
| daytemp3 | day temperature T3 | uint8 (>=0<=127) | C | true | HC | 106 | 1 | 1/2 | 
| daytemp4 | day temperature T4 | uint8 (>=0<=127) | C | true | HC | 107 | 1 | 1/2 | 
| manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 | 
| offtemp | temperature when mode is off | uint8 (>=0<=127) | C | true | HC | 104 | 1 | 1/2 | 
| mode | operating mode | enum [on\|off\|auto] |   | true | DHW | 0 | 1 | 1 | 
| whenmodeoff | when thermostat mode off | boolean |   | true | DHW | 62 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 7 | 1 | 1 | 
| disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 8 | 1 | 1 | 
| disinfecthour | disinfection hour | uint8 (>=0<=23) |   | true | DHW | 14 | 1 | 1 | 

### RC10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 | 
| daytemp | day temperature | uint8 (>=0<=127) | C | true | HC | 53 | 1 | 1/2 | 
| nighttemp | night temperature | uint8 (>=0<=127) | C | true | HC | 54 | 1 | 1/2 | 
| program | program | enum [family\|morning\|evening\|am\|pm\|midday\|singles\|seniors] |   | true | HC | 25 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 | 
| tempautotemp | temporary set temperature automode | uint8 (>=0<=127) | C | true | HC | 26 | 1 | 1/2 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 | 
| summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 | 

### RC10, Moduline 100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| intoffset | internal temperature offset | int8 (>=-12<=12) | C | true | DEVICE_DATA | 46 | 1 | 1/10 | 
| heatingpid | heating PID | enum [fast\|medium\|slow] |   | true | DEVICE_DATA | 74 | 1 | 1 | 
| backlight | key backlight | boolean |   | true | DEVICE_DATA | 71 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [nofrost\|night\|day] |   | true | HC | 3 | 1 | 1 | 
| daytemp | day temperature | uint8 (>=0<=127) | C | true | HC | 53 | 1 | 1/2 | 
| nighttemp | night temperature | uint8 (>=0<=127) | C | true | HC | 54 | 1 | 1/2 | 
| reducehours | duration for nighttemp | uint8 (>=0<=254) | hours | true | HC | 193 | 1 | 1 | 
| reduceminutes | remaining time for nightmode | uint16 (>=0<=31999) | minutes | false | HC | 194 | 1 | 1 | 
| mode | operating mode | enum [on\|off\|auto] |   | true | DHW | 0 | 1 | 1 | 
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 | 
| daytemp | day temperature | uint8 (>=0<=127) | C | true | HC | 53 | 1 | 1/2 | 
| nighttemp | night temperature | uint8 (>=0<=127) | C | true | HC | 54 | 1 | 1/2 | 
| program | program | enum [family\|morning\|evening\|am\|pm\|midday\|singles\|seniors] |   | true | HC | 25 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 | 
| tempautotemp | temporary set temperature automode | uint8 (>=0<=127) | C | true | HC | 26 | 1 | 1/2 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 | 
| summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 | 

### RC100, CR10, Moduline 1000/1010
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 | 
| intoffset | internal temperature offset | int8 (>=-12<=12) | C | true | DEVICE_DATA | 46 | 1 | 1/10 | 
| floordry | floor drying | enum [off\|start\|heat\|hold\|cool\|end] |   | false | DEVICE_DATA | 47 | 1 | 1 | 
| dampedoutdoortemp | damped outdoor temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 48 | 1 | 1/10 | 
| floordrytemp | floor drying temperature | uint8 (>=0<=254) | C | false | DEVICE_DATA | 49 | 1 | 1 | 
| building | building type | enum [light\|medium\|heavy] |   | true | DEVICE_DATA | 50 | 1 | 1 | 
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 | 
| damping | damping outdoor temperature | boolean |   | true | DEVICE_DATA | 52 | 1 | 1 | 
| solar | solar | boolean |   | true | DEVICE_DATA | 53 | 1 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| pvenabledhw | enable raise dhw | boolean |   | true | DEVICE_DATA | 61 | 1 | 1 | 
| pvraiseheat | raise heating with PV | int8 (>=0<=5) | K | true | DEVICE_DATA | 62 | 1 | 1 | 
| pvlowercool | lower cooling with PV | int8 (>=-5<=0) | K | true | DEVICE_DATA | 63 | 1 | 1 | 
| absent | absent | boolean |   | true | DEVICE_DATA | 64 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [manual\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [eco\|comfort] |   | false | HC | 4 | 1 | 1 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 | 
| comforttemp | comfort temperature | uint8 (>=0<=127) | C | true | HC | 7 | 1 | 1/2 | 
| summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=0<=254) | C | true | HC | 9 | 1 | 1 | 
| offsettemp | offset temperature | int8 (>=-126<=126) | C | true | HC | 10 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 | 
| roominfluence | room influence | uint8 (>=0<=254) | C | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=25) |   | true | HC | 14 | 1 | 1/10 | 
| curroominfl | current room influence | int16 (>=-3199<=3199) | C | false | HC | 15 | 1 | 1/10 | 
| nofrostmode | nofrost mode | enum [room\|outdoor\|room outdoor] |   | true | HC | 16 | 1 | 1 | 
| nofrosttemp | nofrost temperature | int8 (>=-126<=126) | C | true | HC | 17 | 1 | 1 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| summersetmode | set summer mode | enum [summer\|auto\|winter] |   | true | HC | 20 | 1 | 1 | 
| hpoperatingmode | heatpump operating mode | enum [off\|auto\|heating\|cooling] |   | true | HC | 21 | 1 | 1 | 
| summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 | 
| hpoperatingstate | heatpump operating state | enum [heating\|off\|cooling] |   | false | HC | 23 | 1 | 1 | 
| controlmode | control mode | enum [optimized\|simple\|n/a\|room\|power] |   | true | HC | 24 | 1 | 1 | 
| program | program | enum [prog 1\|prog 2] |   | true | HC | 25 | 1 | 1 | 
| tempautotemp | temporary set temperature automode | int8 (>=-1<=30) | C | true | HC | 26 | 1 | 1/2 | 
| remoteseltemp | temporary set temperature from remote | int8 (>=-1<=30) | C | true | HC | 27 | 1 | 1/2 | 
| fastheatup | fast heatup | uint8 (>=0<=100) | % | true | HC | 28 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| reducemode | reduce mode | enum [outdoor\|room\|reduce] |   | true | HC | 30 | 1 | 1 | 
| noreducetemp | no reduce below temperature | int8 (>=-126<=126) | C | true | HC | 31 | 1 | 1 | 
| reducetemp | off/reduce switch temperature | int8 (>=-126<=126) | C | true | HC | 32 | 1 | 1 | 
| dhwprio | dhw priority | boolean |   | true | HC | 33 | 1 | 1 | 
| hpcooling | hp cooling | boolean |   | true | HC | 34 | 1 | 1 | 
| coolingon | cooling on | boolean |   | false | HC | 35 | 1 | 1 | 
| hpmode | HP Mode | enum [heating\|cooling\|heating & cooling] |   | true | HC | 36 | 1 | 1 | 
| dewoffset | dew point offset | uint8 (>=2<=10) | K | true | HC | 37 | 1 | 1 | 
| roomtempdiff | room temp difference | uint8 (>=0<=254) | K | true | HC | 38 | 1 | 1 | 
| hpminflowtemp | HP min. flow temp. | uint8 (>=0<=254) | C | true | HC | 39 | 1 | 1 | 
| control | control device | enum [RC310\|RC200\|RC100\|RC100H\|TC100] |   | true | HC | 40 | 1 | 1 | 
| heatondelay | heat-on delay | uint8 (>=1<=48) | hours | true | HC | 43 | 1 | 1 | 
| heatoffdelay | heat-off delay | uint8 (>=1<=48) | hours | true | HC | 44 | 1 | 1 | 
| instantstart | instant start | uint8 (>=1<=10) | K | true | HC | 45 | 1 | 1 | 
| boost | boost mode | boolean |   | true | HC | 46 | 1 | 1 | 
| boosttime | boost time | uint8 (>=0<=254) | hours | true | HC | 47 | 1 | 1 | 
| coolstart | cooling starttemp | uint8 (>=20<=35) | C | true | HC | 48 | 1 | 1 | 
| coolondelay | cooling on delay | uint8 (>=1<=48) | hours | true | HC | 49 | 1 | 1 | 
| cooloffdelay | cooling off delay | uint8 (>=1<=48) | hours | true | HC | 50 | 1 | 1 | 
| switchprogmode | switch program mode | enum [level\|absolute] |   | true | HC | 51 | 1 | 1 | 
| redthreshold | reduction threshold | int8 (>=12<=22) | C | true | HC | 52 | 1 | 1/2 | 
| mode | operating mode | enum [off\|normal\|comfort\|auto\|own prog] |   | true | DHW | 0 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | true | DHW | 1 | 1 | 1 | 
| settemplow | set low temperature | uint8 (>=0<=254) | C | true | DHW | 2 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 3 | 1 | 1 | 
| chargeduration | charge duration | uint8 (>=0<=3810) | minutes | true | DHW | 4 | 1 | 15 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 
| extra | extra | boolean |   | false | DHW | 6 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 7 | 1 | 1 | 
| disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 8 | 1 | 1 | 
| disinfecttime | disinfection time | uint8 (>=0<=1431) | minutes | true | DHW | 9 | 1 | 15 | 
| dailyheating | daily heating | boolean |   | true | DHW | 10 | 1 | 1 | 
| dailyheattime | daily heating time | uint8 (>=0<=1431) | minutes | true | DHW | 11 | 1 | 15 | 

### RC100H, CR10H
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 

### RC120
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 | 
| intoffset | internal temperature offset | int8 (>=-12<=12) | C | true | DEVICE_DATA | 46 | 1 | 1/10 | 
| floordry | floor drying | enum [off\|start\|heat\|hold\|cool\|end] |   | false | DEVICE_DATA | 47 | 1 | 1 | 
| dampedoutdoortemp | damped outdoor temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 48 | 1 | 1/10 | 
| floordrytemp | floor drying temperature | uint8 (>=0<=254) | C | false | DEVICE_DATA | 49 | 1 | 1 | 
| building | building type | enum [light\|medium\|heavy] |   | true | DEVICE_DATA | 50 | 1 | 1 | 
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 | 
| damping | damping outdoor temperature | boolean |   | true | DEVICE_DATA | 52 | 1 | 1 | 
| solar | solar | boolean |   | true | DEVICE_DATA | 53 | 1 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| pvenabledhw | enable raise dhw | boolean |   | true | DEVICE_DATA | 61 | 1 | 1 | 
| pvraiseheat | raise heating with PV | int8 (>=0<=5) | K | true | DEVICE_DATA | 62 | 1 | 1 | 
| pvlowercool | lower cooling with PV | int8 (>=-5<=0) | K | true | DEVICE_DATA | 63 | 1 | 1 | 
| absent | absent | boolean |   | true | DEVICE_DATA | 64 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [off\|manual\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [eco\|comfort] |   | false | HC | 4 | 1 | 1 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 | 
| comforttemp | comfort temperature | uint8 (>=0<=127) | C | true | HC | 7 | 1 | 1/2 | 
| summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=0<=254) | C | true | HC | 9 | 1 | 1 | 
| offsettemp | offset temperature | int8 (>=-126<=126) | C | true | HC | 10 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 | 
| roominfluence | room influence | uint8 (>=0<=254) | C | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=25) |   | true | HC | 14 | 1 | 1/10 | 
| curroominfl | current room influence | int16 (>=-3199<=3199) | C | false | HC | 15 | 1 | 1/10 | 
| nofrostmode | nofrost mode | enum [room\|outdoor\|room outdoor] |   | true | HC | 16 | 1 | 1 | 
| nofrosttemp | nofrost temperature | int8 (>=-126<=126) | C | true | HC | 17 | 1 | 1 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| summersetmode | set summer mode | enum [summer\|auto\|winter] |   | true | HC | 20 | 1 | 1 | 
| hpoperatingmode | heatpump operating mode | enum [off\|auto\|heating\|cooling] |   | true | HC | 21 | 1 | 1 | 
| summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 | 
| hpoperatingstate | heatpump operating state | enum [heating\|off\|cooling] |   | false | HC | 23 | 1 | 1 | 
| controlmode | control mode | enum [optimized\|simple\|n/a\|room\|power] |   | true | HC | 24 | 1 | 1 | 
| program | program | enum [prog 1\|prog 2] |   | true | HC | 25 | 1 | 1 | 
| tempautotemp | temporary set temperature automode | int8 (>=-1<=30) | C | true | HC | 26 | 1 | 1/2 | 
| remoteseltemp | temporary set temperature from remote | int8 (>=-1<=30) | C | true | HC | 27 | 1 | 1/2 | 
| fastheatup | fast heatup | uint8 (>=0<=100) | % | true | HC | 28 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| reducemode | reduce mode | enum [outdoor\|room\|reduce] |   | true | HC | 30 | 1 | 1 | 
| noreducetemp | no reduce below temperature | int8 (>=-126<=126) | C | true | HC | 31 | 1 | 1 | 
| reducetemp | off/reduce switch temperature | int8 (>=-126<=126) | C | true | HC | 32 | 1 | 1 | 
| dhwprio | dhw priority | boolean |   | true | HC | 33 | 1 | 1 | 
| hpcooling | hp cooling | boolean |   | true | HC | 34 | 1 | 1 | 
| coolingon | cooling on | boolean |   | false | HC | 35 | 1 | 1 | 
| hpmode | HP Mode | enum [heating\|cooling\|heating & cooling] |   | true | HC | 36 | 1 | 1 | 
| dewoffset | dew point offset | uint8 (>=2<=10) | K | true | HC | 37 | 1 | 1 | 
| roomtempdiff | room temp difference | uint8 (>=0<=254) | K | true | HC | 38 | 1 | 1 | 
| hpminflowtemp | HP min. flow temp. | uint8 (>=0<=254) | C | true | HC | 39 | 1 | 1 | 
| control | control device | enum [RC310\|RC200\|RC100\|RC100H\|TC100] |   | true | HC | 40 | 1 | 1 | 
| heatondelay | heat-on delay | uint8 (>=1<=48) | hours | true | HC | 43 | 1 | 1 | 
| heatoffdelay | heat-off delay | uint8 (>=1<=48) | hours | true | HC | 44 | 1 | 1 | 
| instantstart | instant start | uint8 (>=1<=10) | K | true | HC | 45 | 1 | 1 | 
| boost | boost mode | boolean |   | true | HC | 46 | 1 | 1 | 
| boosttime | boost time | uint8 (>=0<=254) | hours | true | HC | 47 | 1 | 1 | 
| coolstart | cooling starttemp | uint8 (>=20<=35) | C | true | HC | 48 | 1 | 1 | 
| coolondelay | cooling on delay | uint8 (>=1<=48) | hours | true | HC | 49 | 1 | 1 | 
| cooloffdelay | cooling off delay | uint8 (>=1<=48) | hours | true | HC | 50 | 1 | 1 | 
| switchprogmode | switch program mode | enum [level\|absolute] |   | true | HC | 51 | 1 | 1 | 
| redthreshold | reduction threshold | int8 (>=12<=22) | C | true | HC | 52 | 1 | 1/2 | 
| mode | operating mode | enum [off\|comfort\|auto] |   | true | DHW | 0 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | true | DHW | 1 | 1 | 1 | 
| settemplow | set low temperature | uint8 (>=0<=254) | C | true | DHW | 2 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 3 | 1 | 1 | 
| chargeduration | charge duration | uint8 (>=0<=3810) | minutes | true | DHW | 4 | 1 | 15 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 
| extra | extra | boolean |   | false | DHW | 6 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 7 | 1 | 1 | 
| disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 8 | 1 | 1 | 
| disinfecttime | disinfection time | uint8 (>=0<=1431) | minutes | true | DHW | 9 | 1 | 15 | 
| dailyheating | daily heating | boolean |   | true | DHW | 10 | 1 | 1 | 
| dailyheattime | daily heating time | uint8 (>=0<=1431) | minutes | true | DHW | 11 | 1 | 15 | 

### RC20, Moduline 300
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [off\|manual\|auto] |   | true | HC | 3 | 1 | 1 | 
| manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 | 
| offtemp | temperature when mode is off | uint8 (>=0<=127) | C | true | HC | 104 | 1 | 1/2 | 
| daytemp2 | day temperature T2 | uint8 (>=0<=127) | C | true | HC | 105 | 1 | 1/2 | 
| daytemp3 | day temperature T3 | uint8 (>=0<=127) | C | true | HC | 106 | 1 | 1/2 | 
| daytemp4 | day temperature T4 | uint8 (>=0<=127) | C | true | HC | 107 | 1 | 1/2 | 
| nighttemp | night temperature T1 | uint8 (>=0<=127) | C | true | HC | 54 | 1 | 1/2 | 
| switchtime | program switchtime | string |   | true | HC | 108 | 8 | 1 | 

### RC200, CW100, CR120, CR50
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 | 
| intoffset | internal temperature offset | int8 (>=-12<=12) | C | true | DEVICE_DATA | 46 | 1 | 1/10 | 
| floordry | floor drying | enum [off\|start\|heat\|hold\|cool\|end] |   | false | DEVICE_DATA | 47 | 1 | 1 | 
| dampedoutdoortemp | damped outdoor temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 48 | 1 | 1/10 | 
| floordrytemp | floor drying temperature | uint8 (>=0<=254) | C | false | DEVICE_DATA | 49 | 1 | 1 | 
| building | building type | enum [light\|medium\|heavy] |   | true | DEVICE_DATA | 50 | 1 | 1 | 
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 | 
| damping | damping outdoor temperature | boolean |   | true | DEVICE_DATA | 52 | 1 | 1 | 
| solar | solar | boolean |   | true | DEVICE_DATA | 53 | 1 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| pvenabledhw | enable raise dhw | boolean |   | true | DEVICE_DATA | 61 | 1 | 1 | 
| pvraiseheat | raise heating with PV | int8 (>=0<=5) | K | true | DEVICE_DATA | 62 | 1 | 1 | 
| pvlowercool | lower cooling with PV | int8 (>=-5<=0) | K | true | DEVICE_DATA | 63 | 1 | 1 | 
| absent | absent | boolean |   | true | DEVICE_DATA | 64 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [manual\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [eco\|comfort] |   | false | HC | 4 | 1 | 1 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 | 
| comforttemp | comfort temperature | uint8 (>=0<=127) | C | true | HC | 7 | 1 | 1/2 | 
| summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=0<=254) | C | true | HC | 9 | 1 | 1 | 
| offsettemp | offset temperature | int8 (>=-126<=126) | C | true | HC | 10 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 | 
| roominfluence | room influence | uint8 (>=0<=254) | C | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=25) |   | true | HC | 14 | 1 | 1/10 | 
| curroominfl | current room influence | int16 (>=-3199<=3199) | C | false | HC | 15 | 1 | 1/10 | 
| nofrostmode | nofrost mode | enum [room\|outdoor\|room outdoor] |   | true | HC | 16 | 1 | 1 | 
| nofrosttemp | nofrost temperature | int8 (>=-126<=126) | C | true | HC | 17 | 1 | 1 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| summersetmode | set summer mode | enum [summer\|auto\|winter] |   | true | HC | 20 | 1 | 1 | 
| hpoperatingmode | heatpump operating mode | enum [off\|auto\|heating\|cooling] |   | true | HC | 21 | 1 | 1 | 
| summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 | 
| hpoperatingstate | heatpump operating state | enum [heating\|off\|cooling] |   | false | HC | 23 | 1 | 1 | 
| controlmode | control mode | enum [optimized\|simple\|n/a\|room\|power] |   | true | HC | 24 | 1 | 1 | 
| program | program | enum [prog 1\|prog 2] |   | true | HC | 25 | 1 | 1 | 
| tempautotemp | temporary set temperature automode | int8 (>=-1<=30) | C | true | HC | 26 | 1 | 1/2 | 
| remoteseltemp | temporary set temperature from remote | int8 (>=-1<=30) | C | true | HC | 27 | 1 | 1/2 | 
| fastheatup | fast heatup | uint8 (>=0<=100) | % | true | HC | 28 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| reducemode | reduce mode | enum [outdoor\|room\|reduce] |   | true | HC | 30 | 1 | 1 | 
| noreducetemp | no reduce below temperature | int8 (>=-126<=126) | C | true | HC | 31 | 1 | 1 | 
| reducetemp | off/reduce switch temperature | int8 (>=-126<=126) | C | true | HC | 32 | 1 | 1 | 
| dhwprio | dhw priority | boolean |   | true | HC | 33 | 1 | 1 | 
| hpcooling | hp cooling | boolean |   | true | HC | 34 | 1 | 1 | 
| coolingon | cooling on | boolean |   | false | HC | 35 | 1 | 1 | 
| hpmode | HP Mode | enum [heating\|cooling\|heating & cooling] |   | true | HC | 36 | 1 | 1 | 
| dewoffset | dew point offset | uint8 (>=2<=10) | K | true | HC | 37 | 1 | 1 | 
| roomtempdiff | room temp difference | uint8 (>=0<=254) | K | true | HC | 38 | 1 | 1 | 
| hpminflowtemp | HP min. flow temp. | uint8 (>=0<=254) | C | true | HC | 39 | 1 | 1 | 
| control | control device | enum [RC310\|RC200\|RC100\|RC100H\|TC100] |   | true | HC | 40 | 1 | 1 | 
| heatondelay | heat-on delay | uint8 (>=1<=48) | hours | true | HC | 43 | 1 | 1 | 
| heatoffdelay | heat-off delay | uint8 (>=1<=48) | hours | true | HC | 44 | 1 | 1 | 
| instantstart | instant start | uint8 (>=1<=10) | K | true | HC | 45 | 1 | 1 | 
| boost | boost mode | boolean |   | true | HC | 46 | 1 | 1 | 
| boosttime | boost time | uint8 (>=0<=254) | hours | true | HC | 47 | 1 | 1 | 
| coolstart | cooling starttemp | uint8 (>=20<=35) | C | true | HC | 48 | 1 | 1 | 
| coolondelay | cooling on delay | uint8 (>=1<=48) | hours | true | HC | 49 | 1 | 1 | 
| cooloffdelay | cooling off delay | uint8 (>=1<=48) | hours | true | HC | 50 | 1 | 1 | 
| switchprogmode | switch program mode | enum [level\|absolute] |   | true | HC | 51 | 1 | 1 | 
| redthreshold | reduction threshold | int8 (>=12<=22) | C | true | HC | 52 | 1 | 1/2 | 
| mode | operating mode | enum [off\|normal\|comfort\|auto\|own prog] |   | true | DHW | 0 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | true | DHW | 1 | 1 | 1 | 
| settemplow | set low temperature | uint8 (>=0<=254) | C | true | DHW | 2 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 3 | 1 | 1 | 
| chargeduration | charge duration | uint8 (>=0<=3810) | minutes | true | DHW | 4 | 1 | 15 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 
| extra | extra | boolean |   | false | DHW | 6 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 7 | 1 | 1 | 
| disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 8 | 1 | 1 | 
| disinfecttime | disinfection time | uint8 (>=0<=1431) | minutes | true | DHW | 9 | 1 | 15 | 
| dailyheating | daily heating | boolean |   | true | DHW | 10 | 1 | 1 | 
| dailyheattime | daily heating time | uint8 (>=0<=1431) | minutes | true | DHW | 11 | 1 | 15 | 

### RC20RF
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [off\|manual\|auto] |   | true | HC | 3 | 1 | 1 | 
| manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 | 
| offtemp | temperature when mode is off | uint8 (>=0<=127) | C | true | HC | 104 | 1 | 1/2 | 
| daytemp2 | day temperature T2 | uint8 (>=0<=127) | C | true | HC | 105 | 1 | 1/2 | 
| daytemp3 | day temperature T3 | uint8 (>=0<=127) | C | true | HC | 106 | 1 | 1/2 | 
| daytemp4 | day temperature T4 | uint8 (>=0<=127) | C | true | HC | 107 | 1 | 1/2 | 
| nighttemp | night temperature T1 | uint8 (>=0<=127) | C | true | HC | 54 | 1 | 1/2 | 
| switchtime | program switchtime | string |   | true | HC | 108 | 8 | 1 | 

### RC25
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 | 
| daytemp | day temperature | uint8 (>=0<=127) | C | true | HC | 53 | 1 | 1/2 | 
| nighttemp | night temperature | uint8 (>=0<=127) | C | true | HC | 54 | 1 | 1/2 | 
| program | program | enum [family\|morning\|evening\|am\|pm\|midday\|singles\|seniors] |   | true | HC | 25 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 | 
| tempautotemp | temporary set temperature automode | uint8 (>=0<=127) | C | true | HC | 26 | 1 | 1/2 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1 | 
| summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 | 

### RC3*0, Moduline 3000/1010H, CW400, Sense II, HPC410
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 | 
| intoffset | internal temperature offset | int8 (>=-12<=12) | C | true | DEVICE_DATA | 46 | 1 | 1/10 | 
| floordry | floor drying | enum [off\|start\|heat\|hold\|cool\|end] |   | false | DEVICE_DATA | 47 | 1 | 1 | 
| dampedoutdoortemp | damped outdoor temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 48 | 1 | 1/10 | 
| floordrytemp | floor drying temperature | uint8 (>=0<=254) | C | false | DEVICE_DATA | 49 | 1 | 1 | 
| building | building type | enum [light\|medium\|heavy] |   | true | DEVICE_DATA | 50 | 1 | 1 | 
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 | 
| damping | damping outdoor temperature | boolean |   | true | DEVICE_DATA | 52 | 1 | 1 | 
| solar | solar | boolean |   | true | DEVICE_DATA | 53 | 1 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| pvenabledhw | enable raise dhw | boolean |   | true | DEVICE_DATA | 61 | 1 | 1 | 
| pvraiseheat | raise heating with PV | int8 (>=0<=5) | K | true | DEVICE_DATA | 62 | 1 | 1 | 
| pvlowercool | lower cooling with PV | int8 (>=-5<=0) | K | true | DEVICE_DATA | 63 | 1 | 1 | 
| absent | absent | boolean |   | true | DEVICE_DATA | 64 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [manual\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [eco\|comfort] |   | false | HC | 4 | 1 | 1 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 | 
| comforttemp | comfort temperature | uint8 (>=0<=127) | C | true | HC | 7 | 1 | 1/2 | 
| summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=0<=254) | C | true | HC | 9 | 1 | 1 | 
| offsettemp | offset temperature | int8 (>=-126<=126) | C | true | HC | 10 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 | 
| roominfluence | room influence | uint8 (>=0<=254) | C | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=25) |   | true | HC | 14 | 1 | 1/10 | 
| curroominfl | current room influence | int16 (>=-3199<=3199) | C | false | HC | 15 | 1 | 1/10 | 
| nofrostmode | nofrost mode | enum [room\|outdoor\|room outdoor] |   | true | HC | 16 | 1 | 1 | 
| nofrosttemp | nofrost temperature | int8 (>=-126<=126) | C | true | HC | 17 | 1 | 1 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| summersetmode | set summer mode | enum [summer\|auto\|winter] |   | true | HC | 20 | 1 | 1 | 
| hpoperatingmode | heatpump operating mode | enum [off\|auto\|heating\|cooling] |   | true | HC | 21 | 1 | 1 | 
| summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 | 
| hpoperatingstate | heatpump operating state | enum [heating\|off\|cooling] |   | false | HC | 23 | 1 | 1 | 
| controlmode | control mode | enum [weather compensated\|outside basepoint\|n/a\|room\|power\|constant] |   | true | HC | 24 | 1 | 1 | 
| program | program | enum [prog 1\|prog 2] |   | true | HC | 25 | 1 | 1 | 
| tempautotemp | temporary set temperature automode | int8 (>=-1<=30) | C | true | HC | 26 | 1 | 1/2 | 
| remoteseltemp | temporary set temperature from remote | int8 (>=-1<=30) | C | true | HC | 27 | 1 | 1/2 | 
| fastheatup | fast heatup | uint8 (>=0<=100) | % | true | HC | 28 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| reducemode | reduce mode | enum [outdoor\|room\|reduce] |   | true | HC | 30 | 1 | 1 | 
| noreducetemp | no reduce below temperature | int8 (>=-126<=126) | C | true | HC | 31 | 1 | 1 | 
| reducetemp | off/reduce switch temperature | int8 (>=-126<=126) | C | true | HC | 32 | 1 | 1 | 
| dhwprio | dhw priority | boolean |   | true | HC | 33 | 1 | 1 | 
| hpcooling | hp cooling | boolean |   | true | HC | 34 | 1 | 1 | 
| coolingon | cooling on | boolean |   | false | HC | 35 | 1 | 1 | 
| hpmode | HP Mode | enum [heating\|cooling\|heating & cooling] |   | true | HC | 36 | 1 | 1 | 
| dewoffset | dew point offset | uint8 (>=2<=10) | K | true | HC | 37 | 1 | 1 | 
| roomtempdiff | room temp difference | uint8 (>=0<=254) | K | true | HC | 38 | 1 | 1 | 
| hpminflowtemp | HP min. flow temp. | uint8 (>=0<=254) | C | true | HC | 39 | 1 | 1 | 
| control | control device | enum [RC310\|RC200\|RC100\|RC100H\|TC100] |   | true | HC | 40 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| remotehum | room humidity from remote | cmd [] (>=-1<=101) | % | true | HC | 42 | 1 | 1 | 
| heatondelay | heat-on delay | uint8 (>=1<=48) | hours | true | HC | 43 | 1 | 1 | 
| heatoffdelay | heat-off delay | uint8 (>=1<=48) | hours | true | HC | 44 | 1 | 1 | 
| instantstart | instant start | uint8 (>=1<=10) | K | true | HC | 45 | 1 | 1 | 
| boost | boost mode | boolean |   | true | HC | 46 | 1 | 1 | 
| boosttime | boost time | uint8 (>=0<=254) | hours | true | HC | 47 | 1 | 1 | 
| coolstart | cooling starttemp | uint8 (>=20<=35) | C | true | HC | 48 | 1 | 1 | 
| coolondelay | cooling on delay | uint8 (>=1<=48) | hours | true | HC | 49 | 1 | 1 | 
| cooloffdelay | cooling off delay | uint8 (>=1<=48) | hours | true | HC | 50 | 1 | 1 | 
| switchprogmode | switch program mode | enum [level\|absolute] |   | true | HC | 51 | 1 | 1 | 
| redthreshold | reduction threshold | int8 (>=12<=22) | C | true | HC | 52 | 1 | 1/2 | 
| mode | operating mode | enum [off\|normal\|comfort\|auto\|own prog] |   | true | DHW | 0 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | true | DHW | 1 | 1 | 1 | 
| settemplow | set low temperature | uint8 (>=0<=254) | C | true | DHW | 2 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 3 | 1 | 1 | 
| chargeduration | charge duration | uint8 (>=0<=3810) | minutes | true | DHW | 4 | 1 | 15 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 
| extra | extra | boolean |   | false | DHW | 6 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 7 | 1 | 1 | 
| disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 8 | 1 | 1 | 
| disinfecttime | disinfection time | uint8 (>=0<=1431) | minutes | true | DHW | 9 | 1 | 15 | 
| dailyheating | daily heating | boolean |   | true | DHW | 10 | 1 | 1 | 
| dailyheattime | daily heating time | uint8 (>=0<=1431) | minutes | true | DHW | 11 | 1 | 15 | 

### RC30
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| display | display | enum [internal temperature\|internal setpoint\|external temperature\|burner temperature\|dhw temperature\|functioning mode\|time\|date\|smoke temperature] |   | true | DEVICE_DATA | 65 | 1 | 1 | 
| language | language | enum [german\|dutch\|french\|italian] |   | false | DEVICE_DATA | 66 | 1 | 1 | 
| clockoffset | clock offset | int8 (>=-126<=126) | seconds | true | DEVICE_DATA | 67 | 1 | 1 | 
| intoffset | internal temperature offset | int8 (>=-12<=12) | C | true | DEVICE_DATA | 46 | 1 | 1/10 | 
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 | 
| inttemp1 | temperature sensor 1 | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 68 | 1 | 1/10 | 
| inttemp2 | temperature sensor 2 | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 69 | 1 | 1/10 | 
| damping | damping outdoor temperature | boolean |   | true | DEVICE_DATA | 52 | 1 | 1 | 
| dampedoutdoortemp | damped outdoor temperature | int8 (>=-126<=126) | C | false | DEVICE_DATA | 48 | 1 | 1 | 
| building | building type | enum [light\|medium\|heavy] |   | true | DEVICE_DATA | 50 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 | 
| daytemp | day temperature | uint8 (>=10<=30) | C | true | HC | 53 | 1 | 1/2 | 
| nighttemp | night temperature | uint8 (>=10<=30) | C | true | HC | 54 | 1 | 1/2 | 
| designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 | 
| offsettemp | offset temperature | int8 (>=-5<=5) | C | true | HC | 10 | 1 | 1/2 | 
| holidaytemp | holiday temperature | uint8 (>=5<=30) | C | true | HC | 55 | 1 | 1/2 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1 | 
| summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 | 
| holidaymode | holiday mode | boolean |   | false | HC | 56 | 1 | 1 | 
| nofrosttemp | nofrost temperature | int8 (>=-20<=10) | C | true | HC | 17 | 1 | 1 | 
| nofrostmode | nofrost mode | enum [off\|outdoor\|room] |   | true | HC | 16 | 1 | 1 | 
| roominfluence | room influence | uint8 (>=0<=10) | C | true | HC | 13 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 | 
| flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | C | true | HC | 57 | 1 | 1 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| reducemode | reduce mode | enum [nofrost\|reduce\|room\|outdoor] |   | true | HC | 30 | 1 | 1 | 
| controlmode | control mode | enum [outdoor\|room] |   | true | HC | 24 | 1 | 1 | 
| control | control device | enum [off\|RC20\|RC3x] |   | true | HC | 40 | 1 | 1 | 
| holidays | holiday dates | string |   | true | HC | 58 | 13 | 1 | 
| vacations | vacation dates | string |   | true | HC | 71 | 13 | 1 | 
| program | program | enum [own 1\|family\|morning\|evening\|am\|pm\|midday\|singles\|seniors\|new\|own 2] |   | true | HC | 25 | 1 | 1 | 
| pause | pause time | uint8 (>=0<=99) | hours | true | HC | 84 | 1 | 1 | 
| party | party time | uint8 (>=0<=99) | hours | true | HC | 85 | 1 | 1 | 
| tempautotemp | temporary set temperature automode | uint8 (>=0<=30) | C | true | HC | 26 | 1 | 1/2 | 
| noreducetemp | no reduce below temperature | int8 (>=-31<=10) | C | true | HC | 31 | 1 | 1 | 
| reducetemp | off/reduce switch temperature | int8 (>=-20<=10) | C | true | HC | 32 | 1 | 1 | 
| vacreducetemp | vacations off/reduce switch temperature | int8 (>=-20<=10) | C | true | HC | 86 | 1 | 1 | 
| vacreducemode | vacations reduce mode | enum [nofrost\|reduce\|room\|outdoor] |   | true | HC | 87 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| dhwprio | dhw priority | boolean |   | true | HC | 33 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| switchtime1 | own1 program switchtime | string |   | true | HC | 88 | 8 | 1 | 
| switchtime2 | own2 program switchtime | string |   | true | HC | 96 | 8 | 1 | 
| mode | operating mode | enum [off\|on\|auto] |   | true | DHW | 0 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|on\|auto] |   | true | DHW | 3 | 1 | 1 | 
| progmode | program | enum [std prog\|own prog] |   | true | DHW | 12 | 1 | 1 | 
| circprog | circulation program | enum [std prog\|own prog] |   | true | DHW | 13 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 7 | 1 | 1 | 
| disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 8 | 1 | 1 | 
| disinfecthour | disinfection hour | uint8 (>=0<=23) |   | true | DHW | 14 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=254) | C | true | DHW | 15 | 1 | 1 | 
| onetimekey | one time key function | boolean |   | true | DHW | 16 | 1 | 1 | 
| switchtimeWW | program switchtime warm water | string |   | true | DHW | 17 | 11 | 1 | 
| circswitchtime | circulation program switchtime | string |   | true | DHW | 28 | 8 | 1 | 
| holidays | holiday dates | string |   | true | DHW | 36 | 13 | 1 | 
| vacations | vacation dates | string |   | true | DHW | 49 | 13 | 1 | 

### RC35
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 | 
| intoffset | internal temperature offset | int8 (>=-5<=5) | C | true | DEVICE_DATA | 46 | 1 | 1/10 | 
| minexttemp | minimal external temperature | int8 (>=-30<=0) | C | true | DEVICE_DATA | 51 | 1 | 1 | 
| inttemp1 | temperature sensor 1 | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 68 | 1 | 1/10 | 
| inttemp2 | temperature sensor 2 | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 69 | 1 | 1/10 | 
| damping | damping outdoor temperature | boolean |   | true | DEVICE_DATA | 52 | 1 | 1 | 
| dampedoutdoortemp | damped outdoor temperature | int8 (>=-126<=126) | C | false | DEVICE_DATA | 48 | 1 | 1 | 
| building | building type | enum [light\|medium\|heavy] |   | true | DEVICE_DATA | 50 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 | 
| daytemp | day temperature | uint8 (>=10<=30) | C | true | HC | 53 | 1 | 1/2 | 
| nighttemp | night temperature | uint8 (>=10<=30) | C | true | HC | 54 | 1 | 1/2 | 
| designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 | 
| offsettemp | offset temperature | int8 (>=-5<=5) | C | true | HC | 10 | 1 | 1/2 | 
| holidaytemp | holiday temperature | uint8 (>=5<=30) | C | true | HC | 55 | 1 | 1/2 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1 | 
| summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 | 
| holidaymode | holiday mode | boolean |   | false | HC | 56 | 1 | 1 | 
| nofrosttemp | nofrost temperature | int8 (>=-20<=10) | C | true | HC | 17 | 1 | 1 | 
| nofrostmode | nofrost mode | enum [off\|outdoor\|room] |   | true | HC | 16 | 1 | 1 | 
| roominfluence | room influence | uint8 (>=0<=10) | C | true | HC | 13 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 | 
| flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | C | true | HC | 57 | 1 | 1 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| reducemode | reduce mode | enum [nofrost\|reduce\|room\|outdoor] |   | true | HC | 30 | 1 | 1 | 
| controlmode | control mode | enum [outdoor\|room] |   | true | HC | 24 | 1 | 1 | 
| control | control device | enum [off\|RC20\|RC3x] |   | true | HC | 40 | 1 | 1 | 
| holidays | holiday dates | string |   | true | HC | 58 | 13 | 1 | 
| vacations | vacation dates | string |   | true | HC | 71 | 13 | 1 | 
| program | program | enum [own 1\|family\|morning\|evening\|am\|pm\|midday\|singles\|seniors\|new\|own 2] |   | true | HC | 25 | 1 | 1 | 
| pause | pause time | uint8 (>=0<=99) | hours | true | HC | 84 | 1 | 1 | 
| party | party time | uint8 (>=0<=99) | hours | true | HC | 85 | 1 | 1 | 
| tempautotemp | temporary set temperature automode | uint8 (>=0<=30) | C | true | HC | 26 | 1 | 1/2 | 
| noreducetemp | no reduce below temperature | int8 (>=-31<=10) | C | true | HC | 31 | 1 | 1 | 
| reducetemp | off/reduce switch temperature | int8 (>=-20<=10) | C | true | HC | 32 | 1 | 1 | 
| vacreducetemp | vacations off/reduce switch temperature | int8 (>=-20<=10) | C | true | HC | 86 | 1 | 1 | 
| vacreducemode | vacations reduce mode | enum [nofrost\|reduce\|room\|outdoor] |   | true | HC | 87 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| dhwprio | dhw priority | boolean |   | true | HC | 33 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| switchtime1 | own1 program switchtime | string |   | true | HC | 88 | 8 | 1 | 
| switchtime2 | own2 program switchtime | string |   | true | HC | 96 | 8 | 1 | 
| mode | operating mode | enum [off\|on\|auto] |   | true | DHW | 0 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|on\|auto] |   | true | DHW | 3 | 1 | 1 | 
| progmode | program | enum [std prog\|own prog] |   | true | DHW | 12 | 1 | 1 | 
| circprog | circulation program | enum [std prog\|own prog] |   | true | DHW | 13 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 7 | 1 | 1 | 
| disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 8 | 1 | 1 | 
| disinfecthour | disinfection hour | uint8 (>=0<=23) |   | true | DHW | 14 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=60<=80) | C | true | DHW | 15 | 1 | 1 | 
| onetimekey | one time key function | boolean |   | true | DHW | 16 | 1 | 1 | 
| switchtimeWW | program switchtime warm water | string |   | true | DHW | 17 | 11 | 1 | 
| circswitchtime | circulation program switchtime | string |   | true | DHW | 28 | 8 | 1 | 
| holidays | holiday dates | string |   | true | DHW | 36 | 13 | 1 | 
| vacations | vacation dates | string |   | true | DHW | 49 | 13 | 1 | 

### RFM20 Remote
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 

### RT800, RC220
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 

### Rego 2000/3000
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 | 
| intoffset | internal temperature offset | int8 (>=-12<=12) | C | true | DEVICE_DATA | 46 | 1 | 1/10 | 
| floordry | floor drying | enum [off\|start\|heat\|hold\|cool\|end] |   | false | DEVICE_DATA | 47 | 1 | 1 | 
| dampedoutdoortemp | damped outdoor temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 48 | 1 | 1/10 | 
| floordrytemp | floor drying temperature | uint8 (>=0<=254) | C | false | DEVICE_DATA | 49 | 1 | 1 | 
| building | building type | enum [light\|medium\|heavy] |   | true | DEVICE_DATA | 50 | 1 | 1 | 
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 | 
| damping | damping outdoor temperature | boolean |   | true | DEVICE_DATA | 52 | 1 | 1 | 
| solar | solar | boolean |   | true | DEVICE_DATA | 53 | 1 | 1 | 
| vacations | vacation dates | string |   | true | DEVICE_DATA | 76 | 13 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| pvenabledhw | enable raise dhw | boolean |   | true | DEVICE_DATA | 61 | 1 | 1 | 
| pvraiseheat | raise heating with PV | int8 (>=0<=5) | K | true | DEVICE_DATA | 62 | 1 | 1 | 
| pvlowercool | lower cooling with PV | int8 (>=-5<=0) | K | true | DEVICE_DATA | 63 | 1 | 1 | 
| absent | absent | boolean |   | true | DEVICE_DATA | 64 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [manual\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [eco\|comfort] |   | false | HC | 4 | 1 | 1 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 | 
| comforttemp | comfort temperature | uint8 (>=0<=127) | C | true | HC | 7 | 1 | 1/2 | 
| summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=0<=254) | C | true | HC | 9 | 1 | 1 | 
| offsettemp | offset temperature | int8 (>=-126<=126) | C | true | HC | 10 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 | 
| roominfluence | room influence | uint8 (>=0<=254) | C | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=25) |   | true | HC | 14 | 1 | 1/10 | 
| curroominfl | current room influence | int16 (>=-3199<=3199) | C | false | HC | 15 | 1 | 1/10 | 
| nofrostmode | nofrost mode | enum [room\|outdoor\|room outdoor] |   | true | HC | 16 | 1 | 1 | 
| nofrosttemp | nofrost temperature | int8 (>=-126<=126) | C | true | HC | 17 | 1 | 1 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| summersetmode | set summer mode | enum [summer\|auto\|winter] |   | true | HC | 20 | 1 | 1 | 
| hpoperatingmode | heatpump operating mode | enum [off\|auto\|heating\|cooling] |   | true | HC | 21 | 1 | 1 | 
| summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 | 
| hpoperatingstate | heatpump operating state | enum [heating\|off\|cooling] |   | false | HC | 23 | 1 | 1 | 
| controlmode | control mode | enum [weather compensated\|outside basepoint\|n/a\|room\|power\|constant] |   | true | HC | 24 | 1 | 1 | 
| program | program | enum [prog 1\|prog 2] |   | true | HC | 25 | 1 | 1 | 
| tempautotemp | temporary set temperature automode | int8 (>=-1<=30) | C | true | HC | 26 | 1 | 1/2 | 
| remoteseltemp | temporary set temperature from remote | int8 (>=-1<=30) | C | true | HC | 27 | 1 | 1/2 | 
| fastheatup | fast heatup | uint8 (>=0<=100) | % | true | HC | 28 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| reducemode | reduce mode | enum [outdoor\|room\|reduce] |   | true | HC | 30 | 1 | 1 | 
| noreducetemp | no reduce below temperature | int8 (>=-126<=126) | C | true | HC | 31 | 1 | 1 | 
| reducetemp | off/reduce switch temperature | int8 (>=-126<=126) | C | true | HC | 32 | 1 | 1 | 
| dhwprio | dhw priority | boolean |   | true | HC | 33 | 1 | 1 | 
| hpcooling | hp cooling | boolean |   | true | HC | 34 | 1 | 1 | 
| coolingon | cooling on | boolean |   | false | HC | 35 | 1 | 1 | 
| hpmode | HP Mode | enum [heating\|cooling\|heating & cooling] |   | true | HC | 36 | 1 | 1 | 
| dewoffset | dew point offset | uint8 (>=2<=10) | K | true | HC | 37 | 1 | 1 | 
| roomtempdiff | room temp difference | uint8 (>=0<=254) | K | true | HC | 38 | 1 | 1 | 
| hpminflowtemp | HP min. flow temp. | uint8 (>=0<=254) | C | true | HC | 39 | 1 | 1 | 
| control | control device | enum [RC310\|RC200\|RC100\|RC100H\|TC100] |   | true | HC | 40 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| remotehum | room humidity from remote | cmd [] (>=-1<=101) | % | true | HC | 42 | 1 | 1 | 
| heatondelay | heat-on delay | uint8 (>=1<=48) | hours | true | HC | 43 | 1 | 1 | 
| heatoffdelay | heat-off delay | uint8 (>=1<=48) | hours | true | HC | 44 | 1 | 1 | 
| instantstart | instant start | uint8 (>=1<=10) | K | true | HC | 45 | 1 | 1 | 
| boost | boost mode | boolean |   | true | HC | 46 | 1 | 1 | 
| boosttime | boost time | uint8 (>=0<=254) | hours | true | HC | 47 | 1 | 1 | 
| coolstart | cooling starttemp | uint8 (>=20<=35) | C | true | HC | 48 | 1 | 1 | 
| coolondelay | cooling on delay | uint8 (>=1<=48) | hours | true | HC | 49 | 1 | 1 | 
| cooloffdelay | cooling off delay | uint8 (>=1<=48) | hours | true | HC | 50 | 1 | 1 | 
| switchprogmode | switch program mode | enum [level\|absolute] |   | true | HC | 51 | 1 | 1 | 
| redthreshold | reduction threshold | int8 (>=12<=22) | C | true | HC | 52 | 1 | 1/2 | 
| mode | operating mode | enum [normal\|comfort\|eco+] |   | true | DHW | 0 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | true | DHW | 1 | 1 | 1 | 
| settemplow | set low temperature | uint8 (>=0<=254) | C | true | DHW | 2 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 3 | 1 | 1 | 
| chargeduration | charge duration | uint8 (>=0<=3810) | minutes | true | DHW | 4 | 1 | 15 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 
| extra | extra | boolean |   | false | DHW | 6 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 7 | 1 | 1 | 
| disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 8 | 1 | 1 | 
| disinfecttime | disinfection time | uint8 (>=0<=1431) | minutes | true | DHW | 9 | 1 | 15 | 
| dailyheating | daily heating | boolean |   | true | DHW | 10 | 1 | 1 | 
| dailyheattime | daily heating time | uint8 (>=0<=1431) | minutes | true | DHW | 11 | 1 | 15 | 

### Rego 3000, UI800, Logamatic BC400
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 | 
| intoffset | internal temperature offset | int8 (>=-12<=12) | C | true | DEVICE_DATA | 46 | 1 | 1/10 | 
| floordry | floor drying | enum [off\|start\|heat\|hold\|cool\|end] |   | false | DEVICE_DATA | 47 | 1 | 1 | 
| dampedoutdoortemp | damped outdoor temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 48 | 1 | 1/10 | 
| floordrytemp | floor drying temperature | uint8 (>=0<=254) | C | false | DEVICE_DATA | 49 | 1 | 1 | 
| building | building type | enum [light\|medium\|heavy] |   | true | DEVICE_DATA | 50 | 1 | 1 | 
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 | 
| damping | damping outdoor temperature | boolean |   | true | DEVICE_DATA | 52 | 1 | 1 | 
| solar | solar | boolean |   | true | DEVICE_DATA | 53 | 1 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| pvenabledhw | enable raise dhw | boolean |   | true | DEVICE_DATA | 61 | 1 | 1 | 
| pvraiseheat | raise heating with PV | int8 (>=0<=5) | K | true | DEVICE_DATA | 62 | 1 | 1 | 
| pvlowercool | lower cooling with PV | int8 (>=-5<=0) | K | true | DEVICE_DATA | 63 | 1 | 1 | 
| absent | absent | boolean |   | true | DEVICE_DATA | 64 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [off\|manual\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [eco\|comfort] |   | false | HC | 4 | 1 | 1 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 | 
| comforttemp | comfort temperature | uint8 (>=0<=127) | C | true | HC | 7 | 1 | 1/2 | 
| summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=0<=254) | C | true | HC | 9 | 1 | 1 | 
| offsettemp | offset temperature | int8 (>=-126<=126) | C | true | HC | 10 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 | 
| roominfluence | room influence | uint8 (>=0<=254) | C | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=25) |   | true | HC | 14 | 1 | 1/10 | 
| curroominfl | current room influence | int16 (>=-3199<=3199) | C | false | HC | 15 | 1 | 1/10 | 
| nofrostmode | nofrost mode | enum [room\|outdoor\|room outdoor] |   | true | HC | 16 | 1 | 1 | 
| nofrosttemp | nofrost temperature | int8 (>=-126<=126) | C | true | HC | 17 | 1 | 1 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| summersetmode | set summer mode | enum [summer\|auto\|winter] |   | true | HC | 20 | 1 | 1 | 
| hpoperatingmode | heatpump operating mode | enum [off\|auto\|heating\|cooling] |   | true | HC | 21 | 1 | 1 | 
| summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 | 
| hpoperatingstate | heatpump operating state | enum [heating\|off\|cooling] |   | false | HC | 23 | 1 | 1 | 
| controlmode | control mode | enum [weather compensated\|outside basepoint\|n/a\|room\|power\|constant] |   | true | HC | 24 | 1 | 1 | 
| program | program | enum [prog 1\|prog 2] |   | true | HC | 25 | 1 | 1 | 
| tempautotemp | temporary set temperature automode | int8 (>=-1<=30) | C | true | HC | 26 | 1 | 1/2 | 
| remoteseltemp | temporary set temperature from remote | int8 (>=-1<=30) | C | true | HC | 27 | 1 | 1/2 | 
| fastheatup | fast heatup | uint8 (>=0<=100) | % | true | HC | 28 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| reducemode | reduce mode | enum [outdoor\|room\|reduce] |   | true | HC | 30 | 1 | 1 | 
| noreducetemp | no reduce below temperature | int8 (>=-126<=126) | C | true | HC | 31 | 1 | 1 | 
| reducetemp | off/reduce switch temperature | int8 (>=-126<=126) | C | true | HC | 32 | 1 | 1 | 
| dhwprio | dhw priority | boolean |   | true | HC | 33 | 1 | 1 | 
| hpcooling | hp cooling | boolean |   | true | HC | 34 | 1 | 1 | 
| coolingon | cooling on | boolean |   | false | HC | 35 | 1 | 1 | 
| hpmode | HP Mode | enum [heating\|cooling\|heating & cooling] |   | true | HC | 36 | 1 | 1 | 
| dewoffset | dew point offset | uint8 (>=2<=10) | K | true | HC | 37 | 1 | 1 | 
| roomtempdiff | room temp difference | uint8 (>=0<=254) | K | true | HC | 38 | 1 | 1 | 
| hpminflowtemp | HP min. flow temp. | uint8 (>=0<=254) | C | true | HC | 39 | 1 | 1 | 
| control | control device | enum [off\|-\|RC100\|RC100H\|-\|RC120RF\|RC220/RT800\|single] |   | true | HC | 40 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| remotehum | room humidity from remote | cmd [] (>=-1<=101) | % | true | HC | 42 | 1 | 1 | 
| heatondelay | heat-on delay | uint8 (>=1<=48) | hours | true | HC | 43 | 1 | 1 | 
| heatoffdelay | heat-off delay | uint8 (>=1<=48) | hours | true | HC | 44 | 1 | 1 | 
| instantstart | instant start | uint8 (>=1<=10) | K | true | HC | 45 | 1 | 1 | 
| boost | boost mode | boolean |   | true | HC | 46 | 1 | 1 | 
| boosttime | boost time | uint8 (>=0<=254) | hours | true | HC | 47 | 1 | 1 | 
| coolstart | cooling starttemp | uint8 (>=20<=35) | C | true | HC | 48 | 1 | 1 | 
| coolondelay | cooling on delay | uint8 (>=1<=48) | hours | true | HC | 49 | 1 | 1 | 
| cooloffdelay | cooling off delay | uint8 (>=1<=48) | hours | true | HC | 50 | 1 | 1 | 
| switchprogmode | switch program mode | enum [level\|absolute] |   | true | HC | 51 | 1 | 1 | 
| redthreshold | reduction threshold | int8 (>=12<=22) | C | true | HC | 52 | 1 | 1/2 | 
| mode | operating mode | enum [off\|eco+\|eco\|comfort\|auto] |   | true | DHW | 0 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | true | DHW | 1 | 1 | 1 | 
| settemplow | set low temperature | uint8 (>=0<=254) | C | true | DHW | 2 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 3 | 1 | 1 | 
| chargeduration | charge duration | uint8 (>=0<=3810) | minutes | true | DHW | 4 | 1 | 15 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 
| extra | extra | boolean |   | false | DHW | 6 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 7 | 1 | 1 | 
| disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 8 | 1 | 1 | 
| disinfecttime | disinfection time | uint8 (>=0<=1431) | minutes | true | DHW | 9 | 1 | 15 | 
| dailyheating | daily heating | boolean |   | true | DHW | 10 | 1 | 1 | 
| dailyheattime | daily heating time | uint8 (>=0<=1431) | minutes | true | DHW | 11 | 1 | 15 | 

### TR120RF, CR20RF
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 

### UI800, BC400
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 | 
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 | 
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 | 
| intoffset | internal temperature offset | int8 (>=-12<=12) | C | true | DEVICE_DATA | 46 | 1 | 1/10 | 
| floordry | floor drying | enum [off\|start\|heat\|hold\|cool\|end] |   | false | DEVICE_DATA | 47 | 1 | 1 | 
| dampedoutdoortemp | damped outdoor temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 48 | 1 | 1/10 | 
| floordrytemp | floor drying temperature | uint8 (>=0<=254) | C | false | DEVICE_DATA | 49 | 1 | 1 | 
| building | building type | enum [light\|medium\|heavy] |   | true | DEVICE_DATA | 50 | 1 | 1 | 
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 | 
| damping | damping outdoor temperature | boolean |   | true | DEVICE_DATA | 52 | 1 | 1 | 
| solar | solar | boolean |   | true | DEVICE_DATA | 53 | 1 | 1 | 
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 | 
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 | 
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 | 
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 | 
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 | 
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 | 
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 | 
| pvenabledhw | enable raise dhw | boolean |   | true | DEVICE_DATA | 61 | 1 | 1 | 
| pvraiseheat | raise heating with PV | int8 (>=0<=5) | K | true | DEVICE_DATA | 62 | 1 | 1 | 
| pvlowercool | lower cooling with PV | int8 (>=-5<=0) | K | true | DEVICE_DATA | 63 | 1 | 1 | 
| absent | absent | boolean |   | true | DEVICE_DATA | 64 | 1 | 1 | 
| seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 | 
| currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 | 
| haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 | 
| mode | operating mode | enum [off\|manual\|auto] |   | true | HC | 3 | 1 | 1 | 
| modetype | mode type | enum [eco\|comfort] |   | false | HC | 4 | 1 | 1 | 
| ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 | 
| manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 | 
| comforttemp | comfort temperature | uint8 (>=0<=127) | C | true | HC | 7 | 1 | 1/2 | 
| summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 | 
| designtemp | design temperature | uint8 (>=0<=254) | C | true | HC | 9 | 1 | 1 | 
| offsettemp | offset temperature | int8 (>=-126<=126) | C | true | HC | 10 | 1 | 1 | 
| minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 | 
| maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 | 
| roominfluence | room influence | uint8 (>=0<=254) | C | true | HC | 13 | 1 | 1 | 
| roominflfactor | room influence factor | uint8 (>=0<=25) |   | true | HC | 14 | 1 | 1/10 | 
| curroominfl | current room influence | int16 (>=-3199<=3199) | C | false | HC | 15 | 1 | 1/10 | 
| nofrostmode | nofrost mode | enum [room\|outdoor\|room outdoor] |   | true | HC | 16 | 1 | 1 | 
| nofrosttemp | nofrost temperature | int8 (>=-126<=126) | C | true | HC | 17 | 1 | 1 | 
| targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 | 
| heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 | 
| summersetmode | set summer mode | enum [summer\|auto\|winter] |   | true | HC | 20 | 1 | 1 | 
| hpoperatingmode | heatpump operating mode | enum [off\|auto\|heating\|cooling] |   | true | HC | 21 | 1 | 1 | 
| summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 | 
| hpoperatingstate | heatpump operating state | enum [heating\|off\|cooling] |   | false | HC | 23 | 1 | 1 | 
| controlmode | control mode | enum [weather compensated\|outside basepoint\|n/a\|room\|power\|constant] |   | true | HC | 24 | 1 | 1 | 
| program | program | enum [prog 1\|prog 2] |   | true | HC | 25 | 1 | 1 | 
| tempautotemp | temporary set temperature automode | int8 (>=-1<=30) | C | true | HC | 26 | 1 | 1/2 | 
| remoteseltemp | temporary set temperature from remote | int8 (>=-1<=30) | C | true | HC | 27 | 1 | 1/2 | 
| fastheatup | fast heatup | uint8 (>=0<=100) | % | true | HC | 28 | 1 | 1 | 
| switchonoptimization | switch-on optimization | boolean |   | true | HC | 29 | 1 | 1 | 
| reducemode | reduce mode | enum [outdoor\|room\|reduce] |   | true | HC | 30 | 1 | 1 | 
| noreducetemp | no reduce below temperature | int8 (>=-126<=126) | C | true | HC | 31 | 1 | 1 | 
| reducetemp | off/reduce switch temperature | int8 (>=-126<=126) | C | true | HC | 32 | 1 | 1 | 
| dhwprio | dhw priority | boolean |   | true | HC | 33 | 1 | 1 | 
| hpcooling | hp cooling | boolean |   | true | HC | 34 | 1 | 1 | 
| coolingon | cooling on | boolean |   | false | HC | 35 | 1 | 1 | 
| hpmode | HP Mode | enum [heating\|cooling\|heating & cooling] |   | true | HC | 36 | 1 | 1 | 
| dewoffset | dew point offset | uint8 (>=2<=10) | K | true | HC | 37 | 1 | 1 | 
| roomtempdiff | room temp difference | uint8 (>=0<=254) | K | true | HC | 38 | 1 | 1 | 
| hpminflowtemp | HP min. flow temp. | uint8 (>=0<=254) | C | true | HC | 39 | 1 | 1 | 
| control | control device | enum [off\|-\|RC100\|RC100H\|-\|RC120RF\|RC220/RT800\|single] |   | true | HC | 40 | 1 | 1 | 
| remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 41 | 1 | 1/10 | 
| remotehum | room humidity from remote | cmd [] (>=-1<=101) | % | true | HC | 42 | 1 | 1 | 
| heatondelay | heat-on delay | uint8 (>=1<=48) | hours | true | HC | 43 | 1 | 1 | 
| heatoffdelay | heat-off delay | uint8 (>=1<=48) | hours | true | HC | 44 | 1 | 1 | 
| instantstart | instant start | uint8 (>=1<=10) | K | true | HC | 45 | 1 | 1 | 
| boost | boost mode | boolean |   | true | HC | 46 | 1 | 1 | 
| boosttime | boost time | uint8 (>=0<=254) | hours | true | HC | 47 | 1 | 1 | 
| coolstart | cooling starttemp | uint8 (>=20<=35) | C | true | HC | 48 | 1 | 1 | 
| coolondelay | cooling on delay | uint8 (>=1<=48) | hours | true | HC | 49 | 1 | 1 | 
| cooloffdelay | cooling off delay | uint8 (>=1<=48) | hours | true | HC | 50 | 1 | 1 | 
| switchprogmode | switch program mode | enum [level\|absolute] |   | true | HC | 51 | 1 | 1 | 
| redthreshold | reduction threshold | int8 (>=12<=22) | C | true | HC | 52 | 1 | 1/2 | 
| mode | operating mode | enum [off\|eco+\|eco\|comfort\|auto] |   | true | DHW | 0 | 1 | 1 | 
| settemp | set temperature | uint8 (>=0<=254) | C | true | DHW | 1 | 1 | 1 | 
| settemplow | set low temperature | uint8 (>=0<=254) | C | true | DHW | 2 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 3 | 1 | 1 | 
| chargeduration | charge duration | uint8 (>=0<=3810) | minutes | true | DHW | 4 | 1 | 15 | 
| charge | charge | boolean |   | true | DHW | 5 | 1 | 1 | 
| extra | extra | boolean |   | false | DHW | 6 | 1 | 1 | 
| disinfecting | disinfecting | boolean |   | true | DHW | 7 | 1 | 1 | 
| disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 8 | 1 | 1 | 
| disinfecttime | disinfection time | uint8 (>=0<=1431) | minutes | true | DHW | 9 | 1 | 15 | 
| dailyheating | daily heating | boolean |   | true | DHW | 10 | 1 | 1 | 
| dailyheattime | daily heating time | uint8 (>=0<=1431) | minutes | true | DHW | 11 | 1 | 15 | 

## Devices of type *ventilation*
### Logavent HRV176
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| outfresh | outdoor fresh air | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 0 | 1 | 1/10 | 
| infresh | indoor fresh air | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 1 | 1 | 1/10 | 
| outexhaust | outdoor exhaust air | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 2 | 1 | 1/10 | 
| inexhaust | indoor exhaust air | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 3 | 1 | 1/10 | 
| ventinspeed | in blower speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 4 | 1 | 1 | 
| ventoutspeed | out blower speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 5 | 1 | 1 | 
| ventmode | ventilation mode | enum [auto\|off\|L1\|L2\|L3\|L4\|demand\|sleep\|intense\|bypass\|party\|fireplace] |   | true | DEVICE_DATA | 6 | 1 | 1 | 
| airquality | air quality (voc) | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 7 | 1 | 1 | 
| airhumidity | relative air humidity | uint8 (>=0<=100) | % | false | DEVICE_DATA | 8 | 1 | 1 | 

## Devices of type *water*
### IPM
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 0 | 1 | 1 | 
| temp | current temperature | uint16 (>=0<=3199) | C | false | DHW | 1 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 2 | 1 | 1/10 | 
| hydrTemp | hydraulic header temperature | uint16 (>=0<=3199) | C | false | DHW | 3 | 1 | 1/10 | 
| pump | pump | boolean |   | false | DHW | 4 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=254) | C | true | DHW | 5 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 6 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 7 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=0<=254) | C | true | DHW | 8 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 9 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 10 | 1 | 1 | 

### IPM2
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 0 | 1 | 1 | 
| temp | current temperature | uint16 (>=0<=3199) | C | false | DHW | 1 | 1 | 1/10 | 
| curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 2 | 1 | 1/10 | 
| hydrTemp | hydraulic header temperature | uint16 (>=0<=3199) | C | false | DHW | 3 | 1 | 1/10 | 
| pump | pump | boolean |   | false | DHW | 4 | 1 | 1 | 
| flowtempoffset | flow temperature offset | uint8 (>=0<=254) | C | true | DHW | 5 | 1 | 1 | 
| hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 6 | 1 | 1 | 
| hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 7 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=0<=254) | C | true | DHW | 8 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 9 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 10 | 1 | 1 | 

### MM100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| temp | current temperature | uint16 (>=0<=3199) | C | false | DHW | 1 | 1 | 1/10 | 
| tempstatus | temperature switch in assigned dhw (MC1) | int8 (>=-126<=126) |   | false | DHW | 11 | 1 | 1 | 
| pump | pump | boolean |   | false | DHW | 4 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=254) | C | true | DHW | 12 | 1 | 1 | 
| difftemp | start differential temperature | int8 (>=-126<=126) | C | true | DHW | 13 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=0<=254) | C | true | DHW | 8 | 1 | 1 | 
| redtemp | reduced temperature | uint8 (>=0<=254) | C | true | DHW | 14 | 1 | 1 | 
| requiredtemp | required temperature | uint8 (>=0<=254) | C | true | DHW | 15 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 9 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 10 | 1 | 1 | 

### MM200
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| temp | current temperature | uint16 (>=0<=3199) | C | false | DHW | 1 | 1 | 1/10 | 
| tempstatus | temperature switch in assigned dhw (MC1) | int8 (>=-126<=126) |   | false | DHW | 11 | 1 | 1 | 
| pump | pump | boolean |   | false | DHW | 4 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=254) | C | true | DHW | 12 | 1 | 1 | 
| difftemp | start differential temperature | int8 (>=-126<=126) | C | true | DHW | 13 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=0<=254) | C | true | DHW | 8 | 1 | 1 | 
| redtemp | reduced temperature | uint8 (>=0<=254) | C | true | DHW | 14 | 1 | 1 | 
| requiredtemp | required temperature | uint8 (>=0<=254) | C | true | DHW | 15 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 9 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 10 | 1 | 1 | 

### SM100, MS100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| temp | current temperature | uint16 (>=0<=3199) | C | false | DHW | 1 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 16 | 1 | 1/10 | 
| coldtemp | cold water | uint16 (>=0<=3199) | C | false | DHW | 17 | 1 | 1/10 | 
| temp5 | temperature 5 | uint16 (>=0<=3199) | C | false | DHW | 18 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DHW | 19 | 1 | 1/10 | 
| pump | pump | boolean |   | false | DHW | 4 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=254) | C | true | DHW | 12 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 0 | 1 | 1 | 
| redtemp | reduced temperature | uint8 (>=0<=254) | C | true | DHW | 14 | 1 | 1 | 
| hottemp | extra hot temperature | uint8 (>=0<=254) | C | true | DHW | 20 | 1 | 1 | 
| dailytemp | daily temperature | uint8 (>=0<=254) | C | true | DHW | 21 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=0<=254) | C | true | DHW | 8 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 9 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 10 | 1 | 1 | 
| circtc | circulation time controled | boolean |   | true | DHW | 22 | 1 | 1 | 
| keepwarm | keep warm | boolean |   | true | DHW | 23 | 1 | 1 | 
| status2 | status 2 | enum [\|\|\|no heat\|\|\|heat request\|\|disinfecting\|hold] |   | false | DHW | 24 | 1 | 1 | 
| pumpmod | pump modulation | uint8 (>=0<=100) | % | false | DHW | 25 | 1 | 1 | 
| flow | flow rate | uint16 (>=0<=3199) | l/min | false | DHW | 26 | 1 | 1/10 | 
| valvereturn | return valve | uint8 (>=0<=100) | % | true | DHW | 27 | 1 | 1 | 
| deltatret | temp. diff. return valve | uint8 (>=0<=254) | K | true | DHW | 28 | 1 | 1 | 
| errordisp | error display | enum [off\|normal\|inverted] |   | true | DHW | 29 | 1 | 1 | 

### SM200, MS200
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| temp | current temperature | uint16 (>=0<=3199) | C | false | DHW | 1 | 1 | 1/10 | 
| storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 16 | 1 | 1/10 | 
| coldtemp | cold water | uint16 (>=0<=3199) | C | false | DHW | 17 | 1 | 1/10 | 
| temp5 | temperature 5 | uint16 (>=0<=3199) | C | false | DHW | 18 | 1 | 1/10 | 
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DHW | 19 | 1 | 1/10 | 
| pump | pump | boolean |   | false | DHW | 4 | 1 | 1 | 
| maxtemp | maximum temperature | uint8 (>=0<=254) | C | true | DHW | 12 | 1 | 1 | 
| seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 0 | 1 | 1 | 
| redtemp | reduced temperature | uint8 (>=0<=254) | C | true | DHW | 14 | 1 | 1 | 
| hottemp | extra hot temperature | uint8 (>=0<=254) | C | true | DHW | 20 | 1 | 1 | 
| dailytemp | daily temperature | uint8 (>=0<=254) | C | true | DHW | 21 | 1 | 1 | 
| disinfectiontemp | disinfection temperature | uint8 (>=0<=254) | C | true | DHW | 8 | 1 | 1 | 
| circ | circulation active | boolean |   | true | DHW | 9 | 1 | 1 | 
| circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 10 | 1 | 1 | 
| circtc | circulation time controled | boolean |   | true | DHW | 22 | 1 | 1 | 
| keepwarm | keep warm | boolean |   | true | DHW | 23 | 1 | 1 | 
| status2 | status 2 | enum [\|\|\|no heat\|\|\|heat request\|\|disinfecting\|hold] |   | false | DHW | 24 | 1 | 1 | 
| pumpmod | pump modulation | uint8 (>=0<=100) | % | false | DHW | 25 | 1 | 1 | 
| flow | flow rate | uint16 (>=0<=3199) | l/min | false | DHW | 26 | 1 | 1/10 | 
| valvereturn | return valve | uint8 (>=0<=100) | % | true | DHW | 27 | 1 | 1 | 
| deltatret | temp. diff. return valve | uint8 (>=0<=254) | K | true | DHW | 28 | 1 | 1 | 
| errordisp | error display | enum [off\|normal\|inverted] |   | true | DHW | 29 | 1 | 1 | 

