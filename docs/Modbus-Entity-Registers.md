<!-- Use full browser width for this page, the tables are wide -->
<style>
.md-grid {
    max-width: 100%; /* or 100%, if you want to stretch to full-width */
}
</style>

# Entity/Register Mapping

!!! note

    This file has been auto-generated. Do not modify.

## Devices of type *boiler*
### CS5800i, CS6800i, WLW176i, WLW186i
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| mandefrost | manual defrost | cmd [off\|on] |   | true | DEVICE_DATA | 84 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=167772) | kWh | false | DHW | 0 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 87 | 2 | 1/100 |
| nrgcool | energy cooling | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 89 | 2 | 1/100 |
| metertotal | meter total | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 91 | 2 | 1/100 |
| metercomp | meter compressor | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 93 | 2 | 1/100 |
| metereheat | meter e-heater | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 95 | 2 | 1/100 |
| meterheat | meter heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 97 | 2 | 1/100 |
| metercool | meter cooling | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 99 | 2 | 1/100 |
| dhw.meter | meter | uint24 (>=0<=167772) | kWh | false | DHW | 2 | 2 | 1/100 |
| uptimetotal | heatpump total uptime | time (>=0<=279620) | minutes | false | DEVICE_DATA | 101 | 2 | 1/60 |
| uptimecontrol | total operating time heat | time (>=0<=279620) | minutes | false | DEVICE_DATA | 103 | 2 | 1/60 |
| uptimecompheating | operating time compressor heating | time (>=0<=279620) | minutes | false | DEVICE_DATA | 105 | 2 | 1/60 |
| uptimecompcooling | operating time compressor cooling | time (>=0<=279620) | minutes | false | DEVICE_DATA | 107 | 2 | 1/60 |
| dhw.uptimecomp | operating time compressor | time (>=0<=279620) | minutes | false | DHW | 4 | 2 | 1/60 |
| uptimecomppool | operating time compressor pool | time (>=0<=279620) | minutes | false | DEVICE_DATA | 109 | 2 | 1/60 |
| totalcompstarts | total compressor control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 111 | 2 | 1 |
| heatingstarts | heating control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 113 | 2 | 1 |
| coolingstarts | cooling control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 115 | 2 | 1 |
| dhw.startshp | starts hp | uint24 (>=0<=16777214) |   | false | DHW | 6 | 2 | 1 |
| poolstarts | pool control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 117 | 2 | 1 |
| nrgconstotal | total energy consumption | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 119 | 2 | 1 |
| nrgconscomptotal | total energy consumption compressor | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 121 | 2 | 1 |
| nrgconscompheating | energy consumption compressor heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 123 | 2 | 1 |
| dhw.nrgconscomp | energy consumption compressor | uint24 (>=0<=16777214) | kWh | false | DHW | 8 | 2 | 1 |
| nrgconscompcooling | energy consumption compressor cooling | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 125 | 2 | 1 |
| nrgconscomppool | energy consumption compressor pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 127 | 2 | 1 |
| auxelecheatnrgconstotal | total aux elec. heater energy consumption | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 129 | 2 | 1 |
| auxelecheatnrgconsheating | aux elec. heater energy consumption heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 131 | 2 | 1 |
| dhw.auxelecheatnrgcons | aux elec. heater energy consumption | uint24 (>=0<=16777214) | kWh | false | DHW | 10 | 2 | 1 |
| auxelecheatnrgconspool | aux elec. heater energy consumption pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 133 | 2 | 1 |
| nrgsupptotal | total energy supplied | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 135 | 2 | 1 |
| nrgsuppheating | total energy supplied heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 137 | 2 | 1 |
| dhw.nrgsupp | total energy warm supplied | uint24 (>=0<=16777214) | kWh | false | DHW | 12 | 2 | 1 |
| nrgsuppcooling | total energy supplied cooling | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 139 | 2 | 1 |
| nrgsupppool | total energy supplied pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 141 | 2 | 1 |
| hppower | compressor power output | uint16 (>=0<=3199) | kW | false | DEVICE_DATA | 143 | 1 | 1/10 |
| hpmaxpower | compressor max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 144 | 1 | 1 |
| pvmaxcomp | pv compressor max power | uint8 (>=0<=25) | kW | true | DEVICE_DATA | 145 | 1 | 1/10 |
| powerreduction | power reduction | uint8 (>=30<=60) | % | true | DEVICE_DATA | 146 | 1 | 10 |
| hpsetdiffpress | set differential pressure | uint8 (>=150<=750) | mbar | true | DEVICE_DATA | 147 | 1 | 50 |
| hpcompon | hp compressor | boolean |   | false | DEVICE_DATA | 148 | 1 | 1 |
| hpactivity | compressor activity | enum [off\|heating\|cooling\|hot water\|pool\|pool heating\|defrost\|compressor alarm] |   | false | DEVICE_DATA | 149 | 1 | 1 |
| hpbrinepumpspd | brine pump speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 150 | 1 | 1 |
| hpswitchvalve | switch valve | boolean |   | false | DEVICE_DATA | 151 | 1 | 1 |
| hpcompspd | compressor speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 152 | 1 | 1 |
| hptargetspd | compressor target speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 153 | 1 | 1 |
| hpcircspd | circulation pump speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 154 | 1 | 1 |
| recvalve | receiver valve VR0 | uint8 (>=0<=100) | % | false | DEVICE_DATA | 155 | 1 | 1 |
| expvalve | expansion valve VR1 | uint8 (>=0<=100) | % | false | DEVICE_DATA | 156 | 1 | 1 |
| hpbrinein | brine in/evaporator | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 157 | 1 | 1/10 |
| hpbrineout | brine out/condenser | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 158 | 1 | 1/10 |
| hptc0 | heat carrier return (TC0) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 159 | 1 | 1/10 |
| hptc1 | heat carrier forward (TC1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 160 | 1 | 1/10 |
| hptc3 | condenser temperature (TC3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 161 | 1 | 1/10 |
| hptr1 | compressor temperature (TR1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 162 | 1 | 1/10 |
| hptr3 | refrigerant temperature liquid side (condenser output) (TR3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 163 | 1 | 1/10 |
| hptr4 | evaporator inlet temperature (TR4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 164 | 1 | 1/10 |
| hptr5 | compressor inlet temperature (TR5) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 165 | 1 | 1/10 |
| hptr6 | compressor outlet temperature (TR6) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 166 | 1 | 1/10 |
| hptr7 | refrigerant temperature gas side (condenser input) (TR7) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 167 | 1 | 1/10 |
| hptl2 | air inlet temperature (TL2) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 168 | 1 | 1/10 |
| hppl1 | low pressure side temperature (PL1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 169 | 1 | 1/10 |
| hpph1 | high pressure side temperature (PH1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 170 | 1 | 1/10 |
| hpta4 | drain pan temp (TA4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 171 | 1 | 1/10 |
| hptw1 | reservoir temp (TW1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 172 | 1 | 1/10 |
| poolsettemp | pool set temperature | uint8 (>=0<=127) | C | true | DEVICE_DATA | 173 | 1 | 1/2 |
| hp4way | 4-way valve (VR4) | enum [cooling & defrost\|heating & dhw] |   | false | DEVICE_DATA | 174 | 1 | 1 |
| hpin1 | input 1 state | boolean |   | false | DEVICE_DATA | 175 | 1 | 1 |
| hpin1opt | input 1 options | string |   | true | DEVICE_DATA | 176 | 8 | 1 |
| hpin2 | input 2 state | boolean |   | false | DEVICE_DATA | 184 | 1 | 1 |
| hpin2opt | input 2 options | string |   | true | DEVICE_DATA | 185 | 8 | 1 |
| hpin3 | input 3 state | boolean |   | false | DEVICE_DATA | 193 | 1 | 1 |
| hpin3opt | input 3 options | string |   | true | DEVICE_DATA | 194 | 8 | 1 |
| hpin4 | input 4 state | boolean |   | false | DEVICE_DATA | 202 | 1 | 1 |
| hpin4opt | input 4 options | string |   | true | DEVICE_DATA | 203 | 8 | 1 |
| maxheatcomp | heat limit compressor | enum [0 kW\|3 kW\|6 kW\|9 kW] |   | true | DEVICE_DATA | 211 | 1 | 1 |
| maxheatheat | heat limit heating | enum [3 kW\|6 kW\|9 kW] |   | true | DEVICE_DATA | 212 | 1 | 1 |
| dhw.maxheat | heat limit | enum [3 kW\|6 kW\|9 kW] |   | true | DHW | 14 | 1 | 1 |
| auxheatersource | aux heater source | enum [not installed\|electric heater\|exclusive\|parallel\|hybrid] |   | true | DEVICE_DATA | 213 | 1 | 1 |
| pvcooling | cooling only with PV | boolean |   | true | DEVICE_DATA | 214 | 1 | 1 |
| auxheateronly | aux heater only | boolean |   | true | DEVICE_DATA | 215 | 1 | 1 |
| auxheateroff | disable aux heater | boolean |   | true | DEVICE_DATA | 216 | 1 | 1 |
| auxheaterstatus | aux heater status | enum [off\|heating\|cooling\|hot water\|pool\|pool heating\|defrost\|compressor alarm] |   | false | DEVICE_DATA | 217 | 1 | 1 |
| auxheaterlevel | aux heater level | uint8 (>=0<=100) | % | false | DEVICE_DATA | 218 | 1 | 1 |
| auxheaterdelay | aux heater on delay | uint16 (>=10<=1000) | K*min | true | DEVICE_DATA | 219 | 1 | 10 |
| auxmaxlimit | aux heater max limit | uint8 (>=0<=10) | K | true | DEVICE_DATA | 220 | 1 | 1/10 |
| auxlimitstart | aux heater limit start | uint8 (>=0<=10) | K | true | DEVICE_DATA | 221 | 1 | 1/10 |
| auxheatrmode | aux heater mode | enum [eco\|comfort] |   | true | DEVICE_DATA | 222 | 1 | 1 |
| hphystheat | on/off hyst heat | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 223 | 1 | 5 |
| hphystcool | on/off hyst cool | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 224 | 1 | 5 |
| hphystpool | on/off hyst pool | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 225 | 1 | 5 |
| silentmode | silent mode | enum [off\|auto\|on] |   | true | DEVICE_DATA | 226 | 1 | 1 |
| silentfrom | silent mode from | uint8 (>=0<=3810) | minutes | true | DEVICE_DATA | 227 | 1 | 15 |
| silentto | silent mode to | uint8 (>=0<=3810) | minutes | true | DEVICE_DATA | 228 | 1 | 15 |
| mintempsilent | min outside temp for silent mode | int8 (>=-126<=126) | C | true | DEVICE_DATA | 229 | 1 | 1 |
| tempparmode | outside temp parallel mode | int8 (>=-126<=126) | C | true | DEVICE_DATA | 230 | 1 | 1 |
| auxheatmix | aux heater mixing valve | int8 (>=-100<=100) | % | false | DEVICE_DATA | 231 | 1 | 1 |
| tempdiffheat | temp diff TC3/TC0 heat | uint8 (>=2<=10) | K | true | DEVICE_DATA | 232 | 1 | 1/10 |
| tempdiffcool | temp diff TC3/TC0 cool | uint8 (>=2<=10) | K | true | DEVICE_DATA | 233 | 1 | 1/10 |
| vpcooling | valve/pump cooling | boolean |   | true | DEVICE_DATA | 234 | 1 | 1 |
| heatcable | heating cable | boolean |   | true | DEVICE_DATA | 235 | 1 | 1 |
| vc0valve | VC0 valve | boolean |   | true | DEVICE_DATA | 236 | 1 | 1 |
| primepump | primary heatpump | boolean |   | true | DEVICE_DATA | 237 | 1 | 1 |
| primepumpmod | primary heatpump modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 238 | 1 | 1 |
| hp3way | 3-way valve | boolean |   | true | DEVICE_DATA | 239 | 1 | 1 |
| elheatstep1 | el. heater step 1 | boolean |   | true | DEVICE_DATA | 240 | 1 | 1 |
| elheatstep2 | el. heater step 2 | boolean |   | true | DEVICE_DATA | 241 | 1 | 1 |
| elheatstep3 | el. heater step 3 | boolean |   | true | DEVICE_DATA | 242 | 1 | 1 |
| hpea0 | condensate reservoir heating (EA0) | boolean |   | false | DEVICE_DATA | 243 | 1 | 1 |
| hppumpmode | primary heatpump mode | enum [auto\|continuous] |   | true | DEVICE_DATA | 244 | 1 | 1 |
| fan | fan | uint8 (>=20<=100) | % | true | DEVICE_DATA | 245 | 1 | 1 |
| fanspd | fan speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 246 | 1 | 1 |
| shutdown | shutdown | cmd [off\|on] |   | true | DEVICE_DATA | 247 | 1 | 1 |
| hpcurrpower | compressor current power | uint16 (>=0<=31999) | W | false | DEVICE_DATA | 248 | 1 | 1 |
| hppowerlimit | power limit | uint16 (>=0<=31999) | W | true | DEVICE_DATA | 249 | 1 | 1 |
| dhw.alternatingop | alternating operation | boolean |   | true | DHW | 15 | 1 | 1 |
| dhw.altopprioheat | prioritise heating during dhw | uint8 (>=20<=120) | minutes | true | DHW | 16 | 1 | 1 |
| dhw.altopprio | prioritise dhw during heating | uint8 (>=30<=120) | minutes | true | DHW | 17 | 1 | 1 |
| dhw.comfoff | comfort switch off | uint8 (>=15<=65) | C | true | DHW | 18 | 1 | 1 |
| dhw.ecooff | eco switch off | uint8 (>=15<=65) | C | true | DHW | 19 | 1 | 1 |
| dhw.ecoplusoff | eco+ switch off | uint8 (>=48<=63) | C | true | DHW | 20 | 1 | 1 |
| dhw.comfdiff | comfort diff | uint8 (>=4<=15) | K | true | DHW | 21 | 1 | 1 |
| dhw.ecodiff | eco diff | uint8 (>=4<=15) | K | true | DHW | 22 | 1 | 1 |
| dhw.ecoplusdiff | eco+ diff | uint8 (>=4<=15) | K | true | DHW | 23 | 1 | 1 |
| dhw.comfstop | comfort stop temp | uint8 (>=0<=254) | C | true | DHW | 24 | 1 | 1 |
| dhw.ecostop | eco stop temp | uint8 (>=0<=254) | C | true | DHW | 25 | 1 | 1 |
| dhw.ecoplusstop | eco+ stop temp | uint8 (>=0<=254) | C | true | DHW | 26 | 1 | 1 |
| dhw.hpcircpump | circulation pump available during dhw | boolean |   | true | DHW | 27 | 1 | 1 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |

### Greenstar 2000
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### C1200W
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### CS5800iG
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### BK13/BK15, Smartline, GB1*2
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Logano GB1*5, Logamatic MC10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Cascade CM10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Logamax Plus GB022
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Condens, Logamax/Logomatic, Cerapur Top, Greenstar, Generic HT3
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [pumpstep\|const.high\|const.medium\|const.low\|prop.high\|propo.low] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Topline, GB162
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Cascade MCM10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Proline
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### GB*72, Trendline, Cerapur, Greenstar Si
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### GB212
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### GC7000F
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Logano GB125/KB195i, Logamatic MC110
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Greenstar 30Ri Compact
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Suprapur-o
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Cerapur Aero
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Hybrid Heatpump
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Logano GB212
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Enviline, Compress 6000AW, Hybrid 3000-7000iAW, SupraEco/Geo 5xx, WLW196i/WSW196i
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| mandefrost | manual defrost | cmd [off\|on] |   | true | DEVICE_DATA | 84 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=167772) | kWh | false | DHW | 0 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 87 | 2 | 1/100 |
| nrgcool | energy cooling | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 89 | 2 | 1/100 |
| metertotal | meter total | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 91 | 2 | 1/100 |
| metercomp | meter compressor | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 93 | 2 | 1/100 |
| metereheat | meter e-heater | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 95 | 2 | 1/100 |
| meterheat | meter heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 97 | 2 | 1/100 |
| metercool | meter cooling | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 99 | 2 | 1/100 |
| dhw.meter | meter | uint24 (>=0<=167772) | kWh | false | DHW | 2 | 2 | 1/100 |
| uptimetotal | heatpump total uptime | time (>=0<=279620) | minutes | false | DEVICE_DATA | 101 | 2 | 1/60 |
| uptimecontrol | total operating time heat | time (>=0<=279620) | minutes | false | DEVICE_DATA | 103 | 2 | 1/60 |
| uptimecompheating | operating time compressor heating | time (>=0<=279620) | minutes | false | DEVICE_DATA | 105 | 2 | 1/60 |
| uptimecompcooling | operating time compressor cooling | time (>=0<=279620) | minutes | false | DEVICE_DATA | 107 | 2 | 1/60 |
| dhw.uptimecomp | operating time compressor | time (>=0<=279620) | minutes | false | DHW | 4 | 2 | 1/60 |
| uptimecomppool | operating time compressor pool | time (>=0<=279620) | minutes | false | DEVICE_DATA | 109 | 2 | 1/60 |
| totalcompstarts | total compressor control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 111 | 2 | 1 |
| heatingstarts | heating control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 113 | 2 | 1 |
| coolingstarts | cooling control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 115 | 2 | 1 |
| dhw.startshp | starts hp | uint24 (>=0<=16777214) |   | false | DHW | 6 | 2 | 1 |
| poolstarts | pool control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 117 | 2 | 1 |
| nrgconstotal | total energy consumption | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 119 | 2 | 1 |
| nrgconscomptotal | total energy consumption compressor | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 121 | 2 | 1 |
| nrgconscompheating | energy consumption compressor heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 123 | 2 | 1 |
| dhw.nrgconscomp | energy consumption compressor | uint24 (>=0<=16777214) | kWh | false | DHW | 8 | 2 | 1 |
| nrgconscompcooling | energy consumption compressor cooling | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 125 | 2 | 1 |
| nrgconscomppool | energy consumption compressor pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 127 | 2 | 1 |
| auxelecheatnrgconstotal | total aux elec. heater energy consumption | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 129 | 2 | 1 |
| auxelecheatnrgconsheating | aux elec. heater energy consumption heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 131 | 2 | 1 |
| dhw.auxelecheatnrgcons | aux elec. heater energy consumption | uint24 (>=0<=16777214) | kWh | false | DHW | 10 | 2 | 1 |
| auxelecheatnrgconspool | aux elec. heater energy consumption pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 133 | 2 | 1 |
| nrgsupptotal | total energy supplied | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 135 | 2 | 1 |
| nrgsuppheating | total energy supplied heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 137 | 2 | 1 |
| dhw.nrgsupp | total energy warm supplied | uint24 (>=0<=16777214) | kWh | false | DHW | 12 | 2 | 1 |
| nrgsuppcooling | total energy supplied cooling | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 139 | 2 | 1 |
| nrgsupppool | total energy supplied pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 141 | 2 | 1 |
| hppower | compressor power output | uint16 (>=0<=3199) | kW | false | DEVICE_DATA | 143 | 1 | 1/10 |
| hpmaxpower | compressor max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 144 | 1 | 1 |
| pvmaxcomp | pv compressor max power | uint8 (>=0<=25) | kW | true | DEVICE_DATA | 145 | 1 | 1/10 |
| powerreduction | power reduction | uint8 (>=30<=60) | % | true | DEVICE_DATA | 146 | 1 | 10 |
| hpsetdiffpress | set differential pressure | uint8 (>=150<=750) | mbar | true | DEVICE_DATA | 147 | 1 | 50 |
| hpcompon | hp compressor | boolean |   | false | DEVICE_DATA | 148 | 1 | 1 |
| hpactivity | compressor activity | enum [off\|heating\|cooling\|hot water\|pool\|pool heating\|defrost\|compressor alarm] |   | false | DEVICE_DATA | 149 | 1 | 1 |
| hpbrinepumpspd | brine pump speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 150 | 1 | 1 |
| hpswitchvalve | switch valve | boolean |   | false | DEVICE_DATA | 151 | 1 | 1 |
| hpcompspd | compressor speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 152 | 1 | 1 |
| hptargetspd | compressor target speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 153 | 1 | 1 |
| hpcircspd | circulation pump speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 154 | 1 | 1 |
| recvalve | receiver valve VR0 | uint8 (>=0<=100) | % | false | DEVICE_DATA | 155 | 1 | 1 |
| expvalve | expansion valve VR1 | uint8 (>=0<=100) | % | false | DEVICE_DATA | 156 | 1 | 1 |
| hpbrinein | brine in/evaporator | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 157 | 1 | 1/10 |
| hpbrineout | brine out/condenser | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 158 | 1 | 1/10 |
| hptc0 | heat carrier return (TC0) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 159 | 1 | 1/10 |
| hptc1 | heat carrier forward (TC1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 160 | 1 | 1/10 |
| hptc3 | condenser temperature (TC3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 161 | 1 | 1/10 |
| hptr1 | compressor temperature (TR1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 162 | 1 | 1/10 |
| hptr3 | refrigerant temperature liquid side (condenser output) (TR3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 163 | 1 | 1/10 |
| hptr4 | evaporator inlet temperature (TR4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 164 | 1 | 1/10 |
| hptr5 | compressor inlet temperature (TR5) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 165 | 1 | 1/10 |
| hptr6 | compressor outlet temperature (TR6) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 166 | 1 | 1/10 |
| hptr7 | refrigerant temperature gas side (condenser input) (TR7) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 167 | 1 | 1/10 |
| hptl2 | air inlet temperature (TL2) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 168 | 1 | 1/10 |
| hppl1 | low pressure side temperature (PL1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 169 | 1 | 1/10 |
| hpph1 | high pressure side temperature (PH1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 170 | 1 | 1/10 |
| hpta4 | drain pan temp (TA4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 171 | 1 | 1/10 |
| hptw1 | reservoir temp (TW1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 172 | 1 | 1/10 |
| poolsettemp | pool set temperature | uint8 (>=0<=127) | C | true | DEVICE_DATA | 173 | 1 | 1/2 |
| hp4way | 4-way valve (VR4) | enum [cooling & defrost\|heating & dhw] |   | false | DEVICE_DATA | 174 | 1 | 1 |
| hpin1 | input 1 state | boolean |   | false | DEVICE_DATA | 175 | 1 | 1 |
| hpin1opt | input 1 options | string |   | true | DEVICE_DATA | 176 | 8 | 1 |
| hpin2 | input 2 state | boolean |   | false | DEVICE_DATA | 184 | 1 | 1 |
| hpin2opt | input 2 options | string |   | true | DEVICE_DATA | 185 | 8 | 1 |
| hpin3 | input 3 state | boolean |   | false | DEVICE_DATA | 193 | 1 | 1 |
| hpin3opt | input 3 options | string |   | true | DEVICE_DATA | 194 | 8 | 1 |
| hpin4 | input 4 state | boolean |   | false | DEVICE_DATA | 202 | 1 | 1 |
| hpin4opt | input 4 options | string |   | true | DEVICE_DATA | 203 | 8 | 1 |
| maxheatcomp | heat limit compressor | enum [0 kW\|2 kW\|3 kW\|4 kW\|6 kW\|9 kW] |   | true | DEVICE_DATA | 211 | 1 | 1 |
| maxheatheat | heat limit heating | enum [0 kW\|2 kW\|3 kW\|4 kW\|6 kW\|9 kW] |   | true | DEVICE_DATA | 212 | 1 | 1 |
| dhw.maxheat | heat limit | enum [0 kW\|2 kW\|3 kW\|4 kW\|6 kW\|9 kW] |   | true | DHW | 14 | 1 | 1 |
| auxheatersource | aux heater source | enum [not installed\|electric heater\|exclusive\|parallel\|hybrid] |   | true | DEVICE_DATA | 213 | 1 | 1 |
| pvcooling | cooling only with PV | boolean |   | true | DEVICE_DATA | 214 | 1 | 1 |
| auxheateronly | aux heater only | boolean |   | true | DEVICE_DATA | 215 | 1 | 1 |
| auxheateroff | disable aux heater | boolean |   | true | DEVICE_DATA | 216 | 1 | 1 |
| auxheaterstatus | aux heater status | enum [off\|heating\|cooling\|hot water\|pool\|pool heating\|defrost\|compressor alarm] |   | false | DEVICE_DATA | 217 | 1 | 1 |
| auxheaterlevel | aux heater level | uint8 (>=0<=100) | % | false | DEVICE_DATA | 218 | 1 | 1 |
| auxheaterdelay | aux heater on delay | uint16 (>=10<=1000) | K*min | true | DEVICE_DATA | 219 | 1 | 10 |
| auxmaxlimit | aux heater max limit | uint8 (>=0<=10) | K | true | DEVICE_DATA | 220 | 1 | 1/10 |
| auxlimitstart | aux heater limit start | uint8 (>=0<=10) | K | true | DEVICE_DATA | 221 | 1 | 1/10 |
| auxheatrmode | aux heater mode | enum [eco\|comfort] |   | true | DEVICE_DATA | 222 | 1 | 1 |
| hphystheat | on/off hyst heat | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 223 | 1 | 5 |
| hphystcool | on/off hyst cool | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 224 | 1 | 5 |
| hphystpool | on/off hyst pool | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 225 | 1 | 5 |
| silentmode | silent mode | enum [off\|auto\|on] |   | true | DEVICE_DATA | 226 | 1 | 1 |
| silentfrom | silent mode from | uint8 (>=0<=3810) | minutes | true | DEVICE_DATA | 227 | 1 | 15 |
| silentto | silent mode to | uint8 (>=0<=3810) | minutes | true | DEVICE_DATA | 228 | 1 | 15 |
| mintempsilent | min outside temp for silent mode | int8 (>=-126<=126) | C | true | DEVICE_DATA | 229 | 1 | 1 |
| tempparmode | outside temp parallel mode | int8 (>=-126<=126) | C | true | DEVICE_DATA | 230 | 1 | 1 |
| auxheatmix | aux heater mixing valve | int8 (>=-100<=100) | % | false | DEVICE_DATA | 231 | 1 | 1 |
| tempdiffheat | temp diff TC3/TC0 heat | uint8 (>=2<=10) | K | true | DEVICE_DATA | 232 | 1 | 1/10 |
| tempdiffcool | temp diff TC3/TC0 cool | uint8 (>=2<=10) | K | true | DEVICE_DATA | 233 | 1 | 1/10 |
| vpcooling | valve/pump cooling | boolean |   | true | DEVICE_DATA | 234 | 1 | 1 |
| heatcable | heating cable | boolean |   | true | DEVICE_DATA | 235 | 1 | 1 |
| vc0valve | VC0 valve | boolean |   | true | DEVICE_DATA | 236 | 1 | 1 |
| primepump | primary heatpump | boolean |   | true | DEVICE_DATA | 237 | 1 | 1 |
| primepumpmod | primary heatpump modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 238 | 1 | 1 |
| hp3way | 3-way valve | boolean |   | true | DEVICE_DATA | 239 | 1 | 1 |
| elheatstep1 | el. heater step 1 | boolean |   | true | DEVICE_DATA | 240 | 1 | 1 |
| elheatstep2 | el. heater step 2 | boolean |   | true | DEVICE_DATA | 241 | 1 | 1 |
| elheatstep3 | el. heater step 3 | boolean |   | true | DEVICE_DATA | 242 | 1 | 1 |
| hpea0 | condensate reservoir heating (EA0) | boolean |   | false | DEVICE_DATA | 243 | 1 | 1 |
| hppumpmode | primary heatpump mode | enum [auto\|continuous] |   | true | DEVICE_DATA | 244 | 1 | 1 |
| fan | fan | uint8 (>=20<=100) | % | true | DEVICE_DATA | 245 | 1 | 1 |
| fanspd | fan speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 246 | 1 | 1 |
| shutdown | shutdown | cmd [off\|on] |   | true | DEVICE_DATA | 247 | 1 | 1 |
| hpcurrpower | compressor current power | uint16 (>=0<=31999) | W | false | DEVICE_DATA | 248 | 1 | 1 |
| hppowerlimit | power limit | uint16 (>=0<=31999) | W | true | DEVICE_DATA | 249 | 1 | 1 |
| dhw.alternatingop | alternating operation | boolean |   | true | DHW | 15 | 1 | 1 |
| dhw.altopprioheat | prioritise heating during dhw | uint8 (>=20<=120) | minutes | true | DHW | 16 | 1 | 1 |
| dhw.altopprio | prioritise dhw during heating | uint8 (>=30<=120) | minutes | true | DHW | 17 | 1 | 1 |
| dhw.comfoff | comfort switch off | uint8 (>=15<=65) | C | true | DHW | 18 | 1 | 1 |
| dhw.ecooff | eco switch off | uint8 (>=15<=65) | C | true | DHW | 19 | 1 | 1 |
| dhw.ecoplusoff | eco+ switch off | uint8 (>=48<=63) | C | true | DHW | 20 | 1 | 1 |
| dhw.comfdiff | comfort diff | uint8 (>=4<=15) | K | true | DHW | 21 | 1 | 1 |
| dhw.ecodiff | eco diff | uint8 (>=4<=15) | K | true | DHW | 22 | 1 | 1 |
| dhw.ecoplusdiff | eco+ diff | uint8 (>=4<=15) | K | true | DHW | 23 | 1 | 1 |
| dhw.comfstop | comfort stop temp | uint8 (>=0<=254) | C | true | DHW | 24 | 1 | 1 |
| dhw.ecostop | eco stop temp | uint8 (>=0<=254) | C | true | DHW | 25 | 1 | 1 |
| dhw.ecoplusstop | eco+ stop temp | uint8 (>=0<=254) | C | true | DHW | 26 | 1 | 1 |
| dhw.hpcircpump | circulation pump available during dhw | boolean |   | true | DHW | 27 | 1 | 1 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |

### Geo 5xx
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| mandefrost | manual defrost | cmd [off\|on] |   | true | DEVICE_DATA | 84 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=167772) | kWh | false | DHW | 0 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 87 | 2 | 1/100 |
| nrgcool | energy cooling | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 89 | 2 | 1/100 |
| metertotal | meter total | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 91 | 2 | 1/100 |
| metercomp | meter compressor | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 93 | 2 | 1/100 |
| metereheat | meter e-heater | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 95 | 2 | 1/100 |
| meterheat | meter heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 97 | 2 | 1/100 |
| metercool | meter cooling | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 99 | 2 | 1/100 |
| dhw.meter | meter | uint24 (>=0<=167772) | kWh | false | DHW | 2 | 2 | 1/100 |
| uptimetotal | heatpump total uptime | time (>=0<=279620) | minutes | false | DEVICE_DATA | 101 | 2 | 1/60 |
| uptimecontrol | total operating time heat | time (>=0<=279620) | minutes | false | DEVICE_DATA | 103 | 2 | 1/60 |
| uptimecompheating | operating time compressor heating | time (>=0<=279620) | minutes | false | DEVICE_DATA | 105 | 2 | 1/60 |
| uptimecompcooling | operating time compressor cooling | time (>=0<=279620) | minutes | false | DEVICE_DATA | 107 | 2 | 1/60 |
| dhw.uptimecomp | operating time compressor | time (>=0<=279620) | minutes | false | DHW | 4 | 2 | 1/60 |
| uptimecomppool | operating time compressor pool | time (>=0<=279620) | minutes | false | DEVICE_DATA | 109 | 2 | 1/60 |
| totalcompstarts | total compressor control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 111 | 2 | 1 |
| heatingstarts | heating control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 113 | 2 | 1 |
| coolingstarts | cooling control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 115 | 2 | 1 |
| dhw.startshp | starts hp | uint24 (>=0<=16777214) |   | false | DHW | 6 | 2 | 1 |
| poolstarts | pool control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 117 | 2 | 1 |
| nrgconstotal | total energy consumption | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 119 | 2 | 1 |
| nrgconscomptotal | total energy consumption compressor | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 121 | 2 | 1 |
| nrgconscompheating | energy consumption compressor heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 123 | 2 | 1 |
| dhw.nrgconscomp | energy consumption compressor | uint24 (>=0<=16777214) | kWh | false | DHW | 8 | 2 | 1 |
| nrgconscompcooling | energy consumption compressor cooling | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 125 | 2 | 1 |
| nrgconscomppool | energy consumption compressor pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 127 | 2 | 1 |
| auxelecheatnrgconstotal | total aux elec. heater energy consumption | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 129 | 2 | 1 |
| auxelecheatnrgconsheating | aux elec. heater energy consumption heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 131 | 2 | 1 |
| dhw.auxelecheatnrgcons | aux elec. heater energy consumption | uint24 (>=0<=16777214) | kWh | false | DHW | 10 | 2 | 1 |
| auxelecheatnrgconspool | aux elec. heater energy consumption pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 133 | 2 | 1 |
| nrgsupptotal | total energy supplied | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 135 | 2 | 1 |
| nrgsuppheating | total energy supplied heating | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 137 | 2 | 1 |
| dhw.nrgsupp | total energy warm supplied | uint24 (>=0<=16777214) | kWh | false | DHW | 12 | 2 | 1 |
| nrgsuppcooling | total energy supplied cooling | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 139 | 2 | 1 |
| nrgsupppool | total energy supplied pool | uint24 (>=0<=16777214) | kWh | false | DEVICE_DATA | 141 | 2 | 1 |
| hppower | compressor power output | uint16 (>=0<=3199) | kW | false | DEVICE_DATA | 143 | 1 | 1/10 |
| hpmaxpower | compressor max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 144 | 1 | 1 |
| pvmaxcomp | pv compressor max power | uint8 (>=0<=25) | kW | true | DEVICE_DATA | 145 | 1 | 1/10 |
| powerreduction | power reduction | uint8 (>=30<=60) | % | true | DEVICE_DATA | 146 | 1 | 10 |
| hpsetdiffpress | set differential pressure | uint8 (>=150<=750) | mbar | true | DEVICE_DATA | 147 | 1 | 50 |
| hpcompon | hp compressor | boolean |   | false | DEVICE_DATA | 148 | 1 | 1 |
| hpactivity | compressor activity | enum [off\|heating\|cooling\|hot water\|pool\|pool heating\|defrost\|compressor alarm] |   | false | DEVICE_DATA | 149 | 1 | 1 |
| hpbrinepumpspd | brine pump speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 150 | 1 | 1 |
| hpswitchvalve | switch valve | boolean |   | false | DEVICE_DATA | 151 | 1 | 1 |
| hpcompspd | compressor speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 152 | 1 | 1 |
| hptargetspd | compressor target speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 153 | 1 | 1 |
| hpcircspd | circulation pump speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 154 | 1 | 1 |
| recvalve | receiver valve VR0 | uint8 (>=0<=100) | % | false | DEVICE_DATA | 155 | 1 | 1 |
| expvalve | expansion valve VR1 | uint8 (>=0<=100) | % | false | DEVICE_DATA | 156 | 1 | 1 |
| hpbrinein | brine in/evaporator | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 157 | 1 | 1/10 |
| hpbrineout | brine out/condenser | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 158 | 1 | 1/10 |
| hptc0 | heat carrier return (TC0) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 159 | 1 | 1/10 |
| hptc1 | heat carrier forward (TC1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 160 | 1 | 1/10 |
| hptc3 | condenser temperature (TC3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 161 | 1 | 1/10 |
| hptr1 | compressor temperature (TR1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 162 | 1 | 1/10 |
| hptr3 | refrigerant temperature liquid side (condenser output) (TR3) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 163 | 1 | 1/10 |
| hptr4 | evaporator inlet temperature (TR4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 164 | 1 | 1/10 |
| hptr5 | compressor inlet temperature (TR5) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 165 | 1 | 1/10 |
| hptr6 | compressor outlet temperature (TR6) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 166 | 1 | 1/10 |
| hptr7 | refrigerant temperature gas side (condenser input) (TR7) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 167 | 1 | 1/10 |
| hptl2 | air inlet temperature (TL2) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 168 | 1 | 1/10 |
| hppl1 | low pressure side temperature (PL1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 169 | 1 | 1/10 |
| hpph1 | high pressure side temperature (PH1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 170 | 1 | 1/10 |
| hpta4 | drain pan temp (TA4) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 171 | 1 | 1/10 |
| hptw1 | reservoir temp (TW1) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 172 | 1 | 1/10 |
| poolsettemp | pool set temperature | uint8 (>=0<=127) | C | true | DEVICE_DATA | 173 | 1 | 1/2 |
| hp4way | 4-way valve (VR4) | enum [cooling & defrost\|heating & dhw] |   | false | DEVICE_DATA | 174 | 1 | 1 |
| hpin1 | input 1 state | boolean |   | false | DEVICE_DATA | 175 | 1 | 1 |
| hpin1opt | input 1 options | string |   | true | DEVICE_DATA | 176 | 8 | 1 |
| hpin2 | input 2 state | boolean |   | false | DEVICE_DATA | 184 | 1 | 1 |
| hpin2opt | input 2 options | string |   | true | DEVICE_DATA | 185 | 8 | 1 |
| hpin3 | input 3 state | boolean |   | false | DEVICE_DATA | 193 | 1 | 1 |
| hpin3opt | input 3 options | string |   | true | DEVICE_DATA | 194 | 8 | 1 |
| hpin4 | input 4 state | boolean |   | false | DEVICE_DATA | 202 | 1 | 1 |
| hpin4opt | input 4 options | string |   | true | DEVICE_DATA | 203 | 8 | 1 |
| maxheatcomp | heat limit compressor | enum [0 kW\|2 kW\|3 kW\|4 kW\|6 kW\|9 kW] |   | true | DEVICE_DATA | 211 | 1 | 1 |
| maxheatheat | heat limit heating | enum [0 kW\|2 kW\|3 kW\|4 kW\|6 kW\|9 kW] |   | true | DEVICE_DATA | 212 | 1 | 1 |
| dhw.maxheat | heat limit | enum [0 kW\|2 kW\|3 kW\|4 kW\|6 kW\|9 kW] |   | true | DHW | 14 | 1 | 1 |
| auxheatersource | aux heater source | enum [not installed\|electric heater\|exclusive\|parallel\|hybrid] |   | true | DEVICE_DATA | 213 | 1 | 1 |
| pvcooling | cooling only with PV | boolean |   | true | DEVICE_DATA | 214 | 1 | 1 |
| auxheateronly | aux heater only | boolean |   | true | DEVICE_DATA | 215 | 1 | 1 |
| auxheateroff | disable aux heater | boolean |   | true | DEVICE_DATA | 216 | 1 | 1 |
| auxheaterstatus | aux heater status | enum [off\|heating\|cooling\|hot water\|pool\|pool heating\|defrost\|compressor alarm] |   | false | DEVICE_DATA | 217 | 1 | 1 |
| auxheaterlevel | aux heater level | uint8 (>=0<=100) | % | false | DEVICE_DATA | 218 | 1 | 1 |
| auxheaterdelay | aux heater on delay | uint16 (>=10<=1000) | K*min | true | DEVICE_DATA | 219 | 1 | 10 |
| auxmaxlimit | aux heater max limit | uint8 (>=0<=10) | K | true | DEVICE_DATA | 220 | 1 | 1/10 |
| auxlimitstart | aux heater limit start | uint8 (>=0<=10) | K | true | DEVICE_DATA | 221 | 1 | 1/10 |
| auxheatrmode | aux heater mode | enum [eco\|comfort] |   | true | DEVICE_DATA | 222 | 1 | 1 |
| hphystheat | on/off hyst heat | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 223 | 1 | 5 |
| hphystcool | on/off hyst cool | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 224 | 1 | 5 |
| hphystpool | on/off hyst pool | uint16 (>=50<=1500) | K*min | true | DEVICE_DATA | 225 | 1 | 5 |
| silentmode | silent mode | enum [off\|auto\|on] |   | true | DEVICE_DATA | 226 | 1 | 1 |
| silentfrom | silent mode from | uint8 (>=0<=3810) | minutes | true | DEVICE_DATA | 227 | 1 | 15 |
| silentto | silent mode to | uint8 (>=0<=3810) | minutes | true | DEVICE_DATA | 228 | 1 | 15 |
| mintempsilent | min outside temp for silent mode | int8 (>=-126<=126) | C | true | DEVICE_DATA | 229 | 1 | 1 |
| tempparmode | outside temp parallel mode | int8 (>=-126<=126) | C | true | DEVICE_DATA | 230 | 1 | 1 |
| auxheatmix | aux heater mixing valve | int8 (>=-100<=100) | % | false | DEVICE_DATA | 231 | 1 | 1 |
| tempdiffheat | temp diff TC3/TC0 heat | uint8 (>=2<=10) | K | true | DEVICE_DATA | 232 | 1 | 1/10 |
| tempdiffcool | temp diff TC3/TC0 cool | uint8 (>=2<=10) | K | true | DEVICE_DATA | 233 | 1 | 1/10 |
| vpcooling | valve/pump cooling | boolean |   | true | DEVICE_DATA | 234 | 1 | 1 |
| heatcable | heating cable | boolean |   | true | DEVICE_DATA | 235 | 1 | 1 |
| vc0valve | VC0 valve | boolean |   | true | DEVICE_DATA | 236 | 1 | 1 |
| primepump | primary heatpump | boolean |   | true | DEVICE_DATA | 237 | 1 | 1 |
| primepumpmod | primary heatpump modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 238 | 1 | 1 |
| hp3way | 3-way valve | boolean |   | true | DEVICE_DATA | 239 | 1 | 1 |
| elheatstep1 | el. heater step 1 | boolean |   | true | DEVICE_DATA | 240 | 1 | 1 |
| elheatstep2 | el. heater step 2 | boolean |   | true | DEVICE_DATA | 241 | 1 | 1 |
| elheatstep3 | el. heater step 3 | boolean |   | true | DEVICE_DATA | 242 | 1 | 1 |
| hpea0 | condensate reservoir heating (EA0) | boolean |   | false | DEVICE_DATA | 243 | 1 | 1 |
| hppumpmode | primary heatpump mode | enum [auto\|continuous] |   | true | DEVICE_DATA | 244 | 1 | 1 |
| fan | fan | uint8 (>=20<=100) | % | true | DEVICE_DATA | 245 | 1 | 1 |
| fanspd | fan speed | uint8 (>=0<=100) | % | false | DEVICE_DATA | 246 | 1 | 1 |
| shutdown | shutdown | cmd [off\|on] |   | true | DEVICE_DATA | 247 | 1 | 1 |
| hpcurrpower | compressor current power | uint16 (>=0<=31999) | W | false | DEVICE_DATA | 248 | 1 | 1 |
| hppowerlimit | power limit | uint16 (>=0<=31999) | W | true | DEVICE_DATA | 249 | 1 | 1 |
| dhw.alternatingop | alternating operation | boolean |   | true | DHW | 15 | 1 | 1 |
| dhw.altopprioheat | prioritise heating during dhw | uint8 (>=20<=120) | minutes | true | DHW | 16 | 1 | 1 |
| dhw.altopprio | prioritise dhw during heating | uint8 (>=30<=120) | minutes | true | DHW | 17 | 1 | 1 |
| dhw.comfoff | comfort switch off | uint8 (>=15<=65) | C | true | DHW | 18 | 1 | 1 |
| dhw.ecooff | eco switch off | uint8 (>=15<=65) | C | true | DHW | 19 | 1 | 1 |
| dhw.ecoplusoff | eco+ switch off | uint8 (>=48<=63) | C | true | DHW | 20 | 1 | 1 |
| dhw.comfdiff | comfort diff | uint8 (>=4<=15) | K | true | DHW | 21 | 1 | 1 |
| dhw.ecodiff | eco diff | uint8 (>=4<=15) | K | true | DHW | 22 | 1 | 1 |
| dhw.ecoplusdiff | eco+ diff | uint8 (>=4<=15) | K | true | DHW | 23 | 1 | 1 |
| dhw.comfstop | comfort stop temp | uint8 (>=0<=254) | C | true | DHW | 24 | 1 | 1 |
| dhw.ecostop | eco stop temp | uint8 (>=0<=254) | C | true | DHW | 25 | 1 | 1 |
| dhw.ecoplusstop | eco+ stop temp | uint8 (>=0<=254) | C | true | DHW | 26 | 1 | 1 |
| dhw.hpcircpump | circulation pump available during dhw | boolean |   | true | DHW | 27 | 1 | 1 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |

### Condens 5000i, Greenstar 8000/GC9800IW, GB192i*2
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Logamax U122, Cerapur
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Ecomline Excellent
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Logamax Plus, GB192, Condens GC9000, Greenstar ErP
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Cascade MC400
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### EasyControl Adapter
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

### Greenstar HIU, Logamax kompakt WS170
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| netflowtemp | heat network flow temp | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 277 | 1 | 1/10 |
| heatvalve | heating valve | uint8 (>=0<=100) | % | false | DEVICE_DATA | 278 | 1 | 1 |
| dhw.dhwvalve | valve | uint8 (>=0<=100) | % | false | DHW | 75 | 1 | 1 |
| keepwarmtemp | keep warm temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 279 | 1 | 1 |
| setreturntemp | set temp return | uint8 (>=0<=254) | C | true | DEVICE_DATA | 280 | 1 | 1 |
| heating | heating | boolean |   | false | DEVICE_DATA | 281 | 1 | 1 |
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |

### Logamax Plus GB122, Condense 2300, Junkers Cerapur GC2200W
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| reset | reset | cmd [-\|maintenance\|error\|history\|message] |   | true | DEVICE_DATA | 0 | 1 | 1 |
| chimneysweeper | chimney sweeper | cmd [off\|on] |   | true | DEVICE_DATA | 1 | 1 | 1 |
| heatingoff | force heating off | boolean |   | true | DEVICE_DATA | 2 | 1 | 1 |
| heatingactive | heating active | boolean |   | false | DEVICE_DATA | 3 | 1 | 1 |
| tapwateractive | tapwater active | boolean |   | false | DEVICE_DATA | 4 | 1 | 1 |
| selflowtemp | selected flow temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 5 | 1 | 1 |
| heatingpumpmod | heating pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 6 | 1 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 7 | 1 | 1/10 |
| curflowtemp | current flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 8 | 1 | 1/10 |
| rettemp | return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 9 | 1 | 1/10 |
| switchtemp | mixing switch temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 10 | 1 | 1/10 |
| syspress | system pressure | uint8 (>=0<=25) | bar | false | DEVICE_DATA | 11 | 1 | 1/10 |
| boiltemp | actual boiler temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 12 | 1 | 1/10 |
| headertemp | low loss header | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| exhausttemp | exhaust temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 250 | 1 | 1/10 |
| burngas | gas | boolean |   | false | DEVICE_DATA | 251 | 1 | 1 |
| burngas2 | gas stage 2 | boolean |   | false | DEVICE_DATA | 252 | 1 | 1 |
| flamecurr | flame current | uint16 (>=0<=3199) | µA | false | DEVICE_DATA | 253 | 1 | 1/10 |
| fanwork | fan | boolean |   | false | DEVICE_DATA | 254 | 1 | 1 |
| ignwork | ignition | boolean |   | false | DEVICE_DATA | 255 | 1 | 1 |
| oilpreheat | oil preheating | boolean |   | false | DEVICE_DATA | 256 | 1 | 1 |
| burnminpower | burner min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 257 | 1 | 1 |
| burnmaxpower | burner max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 258 | 1 | 1 |
| burnminperiod | burner min period | uint8 (>=0<=120) | minutes | true | DEVICE_DATA | 259 | 1 | 1 |
| absburnpow | burner current power (absolute) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 260 | 1 | 1 |
| heatblock | heating block | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 261 | 1 | 1/10 |
| boilhyston | hysteresis on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 262 | 1 | 1 |
| boilhystoff | hysteresis off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 263 | 1 | 1 |
| boil2hyston | hysteresis stage 2 on temperature | int8 (>=-20<=0) | C | true | DEVICE_DATA | 264 | 1 | 1 |
| boil2hystoff | hysteresis stage 2 off temperature | int8 (>=0<=20) | C | true | DEVICE_DATA | 265 | 1 | 1 |
| curveon | heatingcurve on | boolean |   | true | DEVICE_DATA | 266 | 1 | 1 |
| curvebase | heatingcurve base | uint8 (>=20<=90) | C | true | DEVICE_DATA | 267 | 1 | 1 |
| curveend | heatingcurve end | uint8 (>=20<=90) | C | true | DEVICE_DATA | 268 | 1 | 1 |
| summertemp | summer temperature | uint8 (>=0<=45) | C | true | DEVICE_DATA | 269 | 1 | 1 |
| nofrostmode | nofrost mode | boolean |   | true | DEVICE_DATA | 270 | 1 | 1 |
| nofrosttemp | nofrost temperature | uint8 (>=0<=10) | C | true | DEVICE_DATA | 271 | 1 | 1 |
| heatingactivated | heating activated | boolean |   | true | DEVICE_DATA | 14 | 1 | 1 |
| heatingtemp | heating temperature | uint8 (>=0<=90) | C | true | DEVICE_DATA | 15 | 1 | 1 |
| heatingpump | heating pump | boolean |   | false | DEVICE_DATA | 16 | 1 | 1 |
| pumpmodmax | boiler pump max power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 17 | 1 | 1 |
| pumpmodmin | boiler pump min power | uint8 (>=0<=100) | % | true | DEVICE_DATA | 18 | 1 | 1 |
| pumpmode | boiler pump mode | enum [proportional\|deltaP-1\|deltaP-2\|deltaP-3\|deltaP-4] |   | true | DEVICE_DATA | 19 | 1 | 1 |
| pumpcharacter | boiler pump characteristic | enum [proportional\|150mbar\|200mbar\|250mbar\|300mbar\|350mbar\|400mbar] |   | true | DEVICE_DATA | 20 | 1 | 1 |
| pumpdelay | pump delay | uint8 (>=0<=60) | minutes | true | DEVICE_DATA | 21 | 1 | 1 |
| pumpontemp | pump logic temperature | uint8 (>=0<=60) | C | true | DEVICE_DATA | 22 | 1 | 1 |
| selburnpow | burner selected max power | uint8 (>=0<=254) | % | true | DEVICE_DATA | 23 | 1 | 1 |
| curburnpow | burner current power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 24 | 1 | 1 |
| burnstarts | burner starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 25 | 2 | 1 |
| burnworkmin | total burner operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 27 | 2 | 1 |
| burn2workmin | burner stage 2 operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 29 | 2 | 1 |
| heatworkmin | total heat operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 31 | 2 | 1 |
| heatstarts | burner starts heating | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 33 | 2 | 1 |
| ubauptime | total UBA operating time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 35 | 2 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 37 | 28 | 1 |
| servicecode | service code | string |   | false | DEVICE_DATA | 65 | 2 | 1 |
| servicecodenumber | service code number | uint16 (>=0<=31999) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| maintenancemessage | maintenance message | string |   | false | DEVICE_DATA | 68 | 2 | 1 |
| maintenance | maintenance scheduled | enum [off\|time\|date\|manual] |   | true | DEVICE_DATA | 70 | 1 | 1 |
| maintenancetime | time to next maintenance | uint16 (>=0<=31999) | hours | true | DEVICE_DATA | 71 | 1 | 1 |
| maintenancedate | next maintenance date | string |   | true | DEVICE_DATA | 72 | 6 | 1 |
| emergencyops | emergency operation | boolean |   | true | DEVICE_DATA | 78 | 1 | 1 |
| emergencytemp | emergency temperature | uint8 (>=15<=70) | C | true | DEVICE_DATA | 79 | 1 | 1 |
| pc0flow | Flow PC0 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 80 | 1 | 1 |
| pc1flow | Flow PC1 | int16 (>=-31999<=31999) | l/h | false | DEVICE_DATA | 81 | 1 | 1 |
| pc1on | PC1 | boolean |   | false | DEVICE_DATA | 82 | 1 | 1 |
| pc1rate | PC1 rate | uint8 (>=0<=100) | % | false | DEVICE_DATA | 83 | 1 | 1 |
| meterheat | meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 97 | 2 | 1/10 |
| dhw.meter | meter | uint24 (>=0<=1677721) | kWh | false | DHW | 2 | 2 | 1/10 |
| gasmeterheat | gas meter heating | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 272 | 2 | 1/10 |
| dhw.gasmeter | gas meter | uint24 (>=0<=1677721) | kWh | false | DHW | 71 | 2 | 1/10 |
| nrgheat2 | energy heating 2 | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 274 | 2 | 1/10 |
| dhw.nrg2 | energy 2 | uint24 (>=0<=1677721) | kWh | false | DHW | 73 | 2 | 1/10 |
| dhw.tapactivated | turn on/off | boolean |   | true | DHW | 28 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | false | DHW | 29 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 30 | 1 | 1 |
| dhw.seltemplow | selected lower temperature | uint8 (>=0<=254) | C | true | DHW | 31 | 1 | 1 |
| dhw.tempecoplus | selected eco+ temperature | uint8 (>=0<=254) | C | true | DHW | 32 | 1 | 1 |
| dhw.seltempoff | selected temperature for off | uint8 (>=0<=254) | C | false | DHW | 33 | 1 | 1 |
| dhw.seltempsingle | single charge temperature | uint8 (>=0<=254) | C | true | DHW | 34 | 1 | 1 |
| dhw.solartemp | solar boiler temperature | uint16 (>=0<=3199) | C | false | DHW | 35 | 1 | 1/10 |
| dhw.type | type | enum [off\|flow\|buffered flow\|buffer\|layered buffer] |   | false | DHW | 36 | 1 | 1 |
| dhw.comfort | comfort | enum [hot\|eco\|intelligent] |   | true | DHW | 37 | 1 | 1 |
| dhw.comfort1 | comfort mode | enum [high comfort\|eco] |   | true | DHW | 38 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=100) | C | true | DHW | 39 | 1 | 1 |
| dhw.chargeoptimization | charge optimization | boolean |   | true | DHW | 40 | 1 | 1 |
| dhw.maxpower | max power | uint8 (>=0<=254) | % | true | DHW | 41 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=80) | C | true | DHW | 42 | 1 | 1 |
| dhw.circpump | circulation pump available | boolean |   | true | DHW | 43 | 1 | 1 |
| dhw.chargetype | charging type | enum [chargepump\|3-way valve] |   | false | DHW | 44 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 45 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 46 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=60<=80) | C | true | DHW | 47 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 48 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 49 | 1 | 1 |
| dhw.curtemp | current intern temperature | uint16 (>=0<=3199) | C | false | DHW | 50 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 51 | 1 | 1/10 |
| dhw.curflow | current tap water flow | uint8 (>=0<=25) | l/min | false | DHW | 52 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 53 | 1 | 1/10 |
| dhw.storagetemp2 | storage extern temperature | uint16 (>=0<=3199) | C | false | DHW | 54 | 1 | 1/10 |
| dhw.activated | activated | boolean |   | true | DHW | 55 | 1 | 1 |
| dhw.onetime | one time charging | boolean |   | true | DHW | 56 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 57 | 1 | 1 |
| dhw.charging | charging | boolean |   | false | DHW | 58 | 1 | 1 |
| dhw.recharging | recharging | boolean |   | false | DHW | 59 | 1 | 1 |
| dhw.tempok | temperature ok | boolean |   | false | DHW | 60 | 1 | 1 |
| dhw.active | active | boolean |   | false | DHW | 61 | 1 | 1 |
| dhw.3wayvalve | 3-way valve active | boolean |   | false | DHW | 62 | 1 | 1 |
| dhw.chargepump | charge pump | boolean |   | false | DHW | 63 | 1 | 1 |
| dhw.mixertemp | mixer temperature | uint16 (>=0<=3199) | C | false | DHW | 64 | 1 | 1/10 |
| dhw.cylmiddletemp | cylinder middle temperature (TS3) | uint16 (>=0<=3199) | C | false | DHW | 65 | 1 | 1/10 |
| dhw.dhwprio | dhw priority | boolean |   | true | DHW | 66 | 1 | 1 |
| dhw.starts | starts | uint24 (>=0<=16777214) |   | false | DHW | 67 | 2 | 1 |
| dhw.workm | active time | time (>=0<=16777214) | minutes | false | DHW | 69 | 2 | 1 |
| nompower | nominal Power | uint8 (>=0<=254) | kW | true | DEVICE_DATA | 276 | 1 | 1 |
| nrgtotal | total energy | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 85 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=10000000) | kWh | true | DEVICE_DATA | 87 | 2 | 1/100 |
| dhw.nrg | energy | uint24 (>=0<=10000000) | kWh | true | DHW | 0 | 2 | 1/100 |

## Devices of type *thermostat*
### Logamatic TC100, Moduline Easy
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=-319<=319) | C | false | HC | 0 | 1 | 1/100 |
| hc1.currtemp | current room temperature | int16 (>=-319<=319) | C | false | HC | 1 | 1 | 1/100 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |

### EasyControl, CT200
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=-319<=319) | C | false | HC | 0 | 1 | 1/100 |
| hc1.currtemp | current room temperature | int16 (>=-319<=319) | C | false | HC | 1 | 1 | 1/100 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |

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
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [off\|manual\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [eco\|comfort] |   | false | HC | 4 | 1 | 1 |
| hc1.ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 |
| hc1.manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 |
| hc1.comforttemp | comfort temperature | uint8 (>=0<=127) | C | true | HC | 7 | 1 | 1/2 |
| hc1.summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 |
| hc1.designtemp | design temperature | uint8 (>=0<=254) | C | true | HC | 9 | 1 | 1 |
| hc1.offsettemp | offset temperature | int8 (>=-126<=126) | C | true | HC | 10 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 |
| hc1.roominfluence | room influence | uint8 (>=0<=254) | C | true | HC | 13 | 1 | 1 |
| hc1.roominflfactor | room influence factor | uint8 (>=0<=25) |   | true | HC | 14 | 1 | 1/10 |
| hc1.curroominfl | current room influence | int16 (>=-3199<=3199) | C | false | HC | 15 | 1 | 1/10 |
| hc1.nofrostmode | nofrost mode | enum [room\|outdoor\|room outdoor] |   | true | HC | 16 | 1 | 1 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-126<=126) | C | true | HC | 17 | 1 | 1 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.summersetmode | set summer mode | enum [summer\|auto\|winter] |   | true | HC | 20 | 1 | 1 |
| hc1.hpoperatingmode | heatpump operating mode | enum [off\|auto\|heating\|cooling] |   | true | HC | 21 | 1 | 1 |
| hc1.summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 |
| hc1.hpoperatingstate | heatpump operating state | enum [heating\|off\|cooling] |   | false | HC | 23 | 1 | 1 |
| hc1.vacationmode | vacation mode | boolean |   | false | HC | 24 | 1 | 1 |
| hc1.controlmode | control mode | enum [weather compensated\|outside basepoint\|n/a\|room\|power\|constant] |   | true | HC | 25 | 1 | 1 |
| hc1.program | program | enum [prog 1\|prog 2] |   | true | HC | 26 | 1 | 1 |
| hc1.tempautotemp | temporary set temperature automode | int8 (>=-1<=30) | C | true | HC | 27 | 1 | 1/2 |
| hc1.cooltemp | cooling temperature | int8 (>=-1<=30) | C | true | HC | 28 | 1 | 1/2 |
| hc1.fastheatup | fast heatup | uint8 (>=0<=100) | % | true | HC | 29 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.reducemode | reduce mode | enum [outdoor\|room\|reduce] |   | true | HC | 31 | 1 | 1 |
| hc1.noreducetemp | no reduce below temperature | int8 (>=-126<=126) | C | true | HC | 32 | 1 | 1 |
| hc1.reducetemp | off/reduce switch temperature | int8 (>=-126<=126) | C | true | HC | 33 | 1 | 1 |
| hc1.dhwprio | dhw priority | boolean |   | true | HC | 34 | 1 | 1 |
| hc1.hpcooling | hp cooling | boolean |   | true | HC | 35 | 1 | 1 |
| hc1.coolingon | cooling on | boolean |   | false | HC | 36 | 1 | 1 |
| hc1.hpmode | HP Mode | enum [heating\|cooling\|heating & cooling] |   | true | HC | 37 | 1 | 1 |
| hc1.dewoffset | dew point offset | uint8 (>=2<=10) | K | true | HC | 38 | 1 | 1 |
| hc1.roomtempdiff | room temp difference | uint8 (>=0<=254) | K | true | HC | 39 | 1 | 1 |
| hc1.hpminflowtemp | HP min. flow temp. | uint8 (>=0<=254) | C | true | HC | 40 | 1 | 1 |
| hc1.control | control device | enum [off\|-\|RC100\|RC100H\|-\|RC120RF\|RC220/RT800\|single] |   | true | HC | 41 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.remotehum | room humidity from remote | cmd [] (>=-1<=101) | % | true | HC | 43 | 1 | 1 |
| hc1.heatondelay | heat-on delay | uint8 (>=1<=48) | hours | true | HC | 44 | 1 | 1 |
| hc1.heatoffdelay | heat-off delay | uint8 (>=1<=48) | hours | true | HC | 45 | 1 | 1 |
| hc1.instantstart | instant start | uint8 (>=1<=10) | K | true | HC | 46 | 1 | 1 |
| hc1.boost | boost mode | boolean |   | true | HC | 47 | 1 | 1 |
| hc1.boosttime | boost time | uint8 (>=0<=254) | hours | true | HC | 48 | 1 | 1 |
| hc1.coolstart | cooling starttemp | uint8 (>=20<=35) | C | true | HC | 49 | 1 | 1 |
| hc1.coolondelay | cooling on delay | uint8 (>=1<=48) | hours | true | HC | 50 | 1 | 1 |
| hc1.cooloffdelay | cooling off delay | uint8 (>=1<=48) | hours | true | HC | 51 | 1 | 1 |
| hc1.switchprogmode | switch program mode | enum [level\|absolute] |   | true | HC | 52 | 1 | 1 |
| hc1.redthreshold | reduction threshold | int8 (>=12<=22) | C | true | HC | 53 | 1 | 1/2 |
| hc1.solarinfl | solar influence | uint8 (>=-5<=4294967295) | C | true | HC | 54 | 1 | 1 |
| hc1.currsolarinfl | curent solar influence | uint8 (>=0<=25) | C | false | HC | 55 | 1 | 1/10 |
| hc1.heatingpid | heating PID | enum [fast\|medium\|slow] |   | true | HC | 56 | 1 | 1 |
| hc1.pumpopt | pump optimization | boolean |   | true | HC | 57 | 1 | 1 |
| hc1.inttimefloor | integral time floor | uint8 (>=160<=320) | minutes | true | HC | 58 | 1 | 10 |
| hc1.inttime | integral time | uint8 (>=3<=80) | minutes | true | HC | 59 | 1 | 1 |
| dhw.mode | operating mode | enum [off\|eco+\|eco\|comfort\|auto] |   | true | DHW | 0 | 1 | 1 |
| dhw.modetype | mode type | enum [off\|eco\|comfort\|eco+] |   | false | DHW | 1 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | true | DHW | 2 | 1 | 1 |
| dhw.settemplow | set low temperature | uint8 (>=0<=254) | C | true | DHW | 3 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 4 | 1 | 1 |
| dhw.chargeduration | charge duration | uint8 (>=0<=3810) | minutes | true | DHW | 5 | 1 | 15 |
| dhw.charge | charge | boolean |   | true | DHW | 6 | 1 | 1 |
| dhw.extra | extra | boolean |   | false | DHW | 7 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 8 | 1 | 1 |
| dhw.disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 9 | 1 | 1 |
| dhw.disinfecttime | disinfection time | uint8 (>=0<=1431) | minutes | true | DHW | 10 | 1 | 15 |
| dhw.dailyheating | daily heating | boolean |   | true | DHW | 11 | 1 | 1 |
| dhw.dailyheattime | daily heating time | uint8 (>=0<=1431) | minutes | true | DHW | 12 | 1 | 15 |

### CR11
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [off\|manual] |   | true | HC | 3 | 1 | 1 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |

### RC10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 |
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 |
| hc1.daytemp | day temperature | uint8 (>=0<=127) | C | true | HC | 60 | 1 | 1/2 |
| hc1.nighttemp | night temperature | uint8 (>=0<=127) | C | true | HC | 61 | 1 | 1/2 |
| hc1.program | program | enum [family\|morning\|evening\|am\|pm\|midday\|singles\|seniors] |   | true | HC | 26 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 |
| hc1.tempautotemp | temporary set temperature automode | uint8 (>=0<=127) | C | true | HC | 27 | 1 | 1/2 |
| hc1.heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 |
| hc1.summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 |

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
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 |
| hc1.daytemp | day temperature | uint8 (>=10<=30) | C | true | HC | 60 | 1 | 1/2 |
| hc1.nighttemp | night temperature | uint8 (>=10<=30) | C | true | HC | 61 | 1 | 1/2 |
| hc1.designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 |
| hc1.offsettemp | offset temperature | int8 (>=-5<=5) | C | true | HC | 10 | 1 | 1/2 |
| hc1.holidaytemp | holiday temperature | uint8 (>=5<=30) | C | true | HC | 62 | 1 | 1/2 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1 |
| hc1.summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 |
| hc1.holidaymode | holiday mode | boolean |   | false | HC | 63 | 1 | 1 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-20<=10) | C | true | HC | 17 | 1 | 1 |
| hc1.nofrostmode | nofrost mode | enum [off\|outdoor\|room] |   | true | HC | 16 | 1 | 1 |
| hc1.roominfluence | room influence | uint8 (>=0<=10) | C | true | HC | 13 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 |
| hc1.flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | C | true | HC | 64 | 1 | 1 |
| hc1.heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.reducemode | reduce mode | enum [nofrost\|reduce\|room\|outdoor] |   | true | HC | 31 | 1 | 1 |
| hc1.controlmode | control mode | enum [outdoor\|room] |   | true | HC | 25 | 1 | 1 |
| hc1.control | control device | enum [off\|RC20\|RC3x] |   | true | HC | 41 | 1 | 1 |
| hc1.holidays | holiday dates | string |   | true | HC | 65 | 13 | 1 |
| hc1.vacations | vacation dates | string |   | true | HC | 78 | 13 | 1 |
| hc1.program | program | enum [own 1\|family\|morning\|evening\|am\|pm\|midday\|singles\|seniors\|new\|own 2] |   | true | HC | 26 | 1 | 1 |
| hc1.pause | pause time | uint8 (>=0<=99) | hours | true | HC | 91 | 1 | 1 |
| hc1.party | party time | uint8 (>=0<=99) | hours | true | HC | 92 | 1 | 1 |
| hc1.tempautotemp | temporary set temperature automode | uint8 (>=0<=30) | C | true | HC | 27 | 1 | 1/2 |
| hc1.noreducetemp | no reduce below temperature | int8 (>=-31<=10) | C | true | HC | 32 | 1 | 1 |
| hc1.reducetemp | off/reduce switch temperature | int8 (>=-20<=10) | C | true | HC | 33 | 1 | 1 |
| hc1.vacreducetemp | vacations off/reduce switch temperature | int8 (>=-20<=10) | C | true | HC | 93 | 1 | 1 |
| hc1.vacreducemode | vacations reduce mode | enum [nofrost\|reduce\|room\|outdoor] |   | true | HC | 94 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.dhwprio | dhw priority | boolean |   | true | HC | 34 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.switchtime1 | own1 program switchtime | string |   | true | HC | 95 | 8 | 1 |
| hc1.switchtime2 | own2 program switchtime | string |   | true | HC | 103 | 8 | 1 |
| dhw.mode | operating mode | enum [off\|on\|auto] |   | true | DHW | 0 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|on\|auto] |   | true | DHW | 4 | 1 | 1 |
| dhw.progmode | program | enum [std prog\|own prog] |   | true | DHW | 13 | 1 | 1 |
| dhw.circprog | circulation program | enum [std prog\|own prog] |   | true | DHW | 14 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 8 | 1 | 1 |
| dhw.disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 9 | 1 | 1 |
| dhw.disinfecthour | disinfection hour | uint8 (>=0<=23) |   | true | DHW | 15 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=254) | C | true | DHW | 16 | 1 | 1 |
| dhw.onetimekey | one time key function | boolean |   | true | DHW | 17 | 1 | 1 |
| dhw.switchtime | program switchtime | string |   | true | DHW | 18 | 8 | 1 |
| dhw.circswitchtime | circulation program switchtime | string |   | true | DHW | 26 | 8 | 1 |
| dhw.holidays | holiday dates | string |   | true | DHW | 34 | 13 | 1 |
| dhw.vacations | vacation dates | string |   | true | DHW | 47 | 13 | 1 |

### RC20, Moduline 300
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [off\|manual\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 |
| hc1.offtemp | temperature when mode is off | uint8 (>=0<=127) | C | true | HC | 111 | 1 | 1/2 |
| hc1.daytemp2 | day temperature T2 | uint8 (>=0<=127) | C | true | HC | 112 | 1 | 1/2 |
| hc1.daytemp3 | day temperature T3 | uint8 (>=0<=127) | C | true | HC | 113 | 1 | 1/2 |
| hc1.daytemp4 | day temperature T4 | uint8 (>=0<=127) | C | true | HC | 114 | 1 | 1/2 |
| hc1.nighttemp | night temperature T1 | uint8 (>=0<=127) | C | true | HC | 61 | 1 | 1/2 |
| hc1.switchtime | program switchtime | string |   | true | HC | 115 | 8 | 1 |

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
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [off\|manual\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.pause | pause time | uint8 (>=0<=254) | hours | true | HC | 91 | 1 | 1 |
| hc1.party | party time | uint8 (>=0<=254) | hours | true | HC | 92 | 1 | 1 |
| hc1.vacations1 | vacation dates 1 | string |   | true | HC | 123 | 11 | 1 |
| hc1.vacations2 | vacation dates 2 | string |   | true | HC | 134 | 11 | 1 |
| hc1.vacations3 | vacation dates 3 | string |   | true | HC | 145 | 11 | 1 |
| hc1.vacations4 | vacation dates 4 | string |   | true | HC | 156 | 11 | 1 |
| hc1.vacations5 | vacation dates 5 | string |   | true | HC | 167 | 11 | 1 |
| hc1.vacations6 | vacation dates 6 | string |   | true | HC | 178 | 11 | 1 |
| hc1.vacations7 | vacation dates 7 | string |   | true | HC | 189 | 11 | 1 |
| hc1.program | program | enum [own 1\|family\|morning\|evening\|am\|pm\|midday\|singles\|seniors\|new\|own 2] |   | true | HC | 26 | 1 | 1 |
| hc1.switchtime1 | own1 program switchtime | string |   | true | HC | 95 | 8 | 1 |
| hc1.heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.controlmode | control mode | enum [outdoor\|room] |   | true | HC | 25 | 1 | 1 |
| hc1.holidaytemp | holiday temperature | uint8 (>=0<=127) | C | true | HC | 62 | 1 | 1/2 |
| hc1.nighttemp | night temperature T1 | uint8 (>=0<=127) | C | true | HC | 61 | 1 | 1/2 |
| hc1.daytemp2 | day temperature T2 | uint8 (>=0<=127) | C | true | HC | 112 | 1 | 1/2 |
| hc1.daytemp3 | day temperature T3 | uint8 (>=0<=127) | C | true | HC | 113 | 1 | 1/2 |
| hc1.daytemp4 | day temperature T4 | uint8 (>=0<=127) | C | true | HC | 114 | 1 | 1/2 |
| hc1.manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 |
| hc1.offtemp | temperature when mode is off | uint8 (>=0<=127) | C | true | HC | 111 | 1 | 1/2 |
| dhw.mode | operating mode | enum [on\|off\|auto] |   | true | DHW | 0 | 1 | 1 |
| dhw.whenmodeoff | when thermostat mode off | boolean |   | true | DHW | 60 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 8 | 1 | 1 |
| dhw.disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 9 | 1 | 1 |
| dhw.disinfecthour | disinfection hour | uint8 (>=0<=23) |   | true | DHW | 15 | 1 | 1 |

### RC10, Moduline 100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| intoffset | internal temperature offset | int8 (>=-12<=12) | C | true | DEVICE_DATA | 46 | 1 | 1/10 |
| heatingpid | heating PID | enum [fast\|medium\|slow] |   | true | DEVICE_DATA | 74 | 1 | 1 |
| backlight | key backlight | boolean |   | true | DEVICE_DATA | 71 | 1 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [nofrost\|night\|day] |   | true | HC | 3 | 1 | 1 |
| hc1.daytemp | day temperature | uint8 (>=0<=127) | C | true | HC | 60 | 1 | 1/2 |
| hc1.nighttemp | night temperature | uint8 (>=0<=127) | C | true | HC | 61 | 1 | 1/2 |
| hc1.reducehours | duration for nighttemp | uint8 (>=0<=254) | hours | true | HC | 200 | 1 | 1 |
| hc1.reduceminutes | remaining time for nightmode | uint16 (>=0<=31999) | minutes | false | HC | 201 | 1 | 1 |
| dhw.mode | operating mode | enum [on\|off\|auto] |   | true | DHW | 0 | 1 | 1 |
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 |
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 |
| hc1.daytemp | day temperature | uint8 (>=0<=127) | C | true | HC | 60 | 1 | 1/2 |
| hc1.nighttemp | night temperature | uint8 (>=0<=127) | C | true | HC | 61 | 1 | 1/2 |
| hc1.program | program | enum [family\|morning\|evening\|am\|pm\|midday\|singles\|seniors] |   | true | HC | 26 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 |
| hc1.tempautotemp | temporary set temperature automode | uint8 (>=0<=127) | C | true | HC | 27 | 1 | 1/2 |
| hc1.heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 |
| hc1.summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 |

### Moduline 200
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| intoffset | internal temperature offset | int8 (>=-12<=12) | C | true | DEVICE_DATA | 46 | 1 | 1/10 |
| heatingpid | heating PID | enum [fast\|medium\|slow] |   | true | DEVICE_DATA | 74 | 1 | 1 |
| backlight | key backlight | boolean |   | true | DEVICE_DATA | 71 | 1 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [nofrost\|night\|day] |   | true | HC | 3 | 1 | 1 |
| hc1.daytemp | day temperature | uint8 (>=0<=127) | C | true | HC | 60 | 1 | 1/2 |
| hc1.nighttemp | night temperature | uint8 (>=0<=127) | C | true | HC | 61 | 1 | 1/2 |
| hc1.reducehours | duration for nighttemp | uint8 (>=0<=254) | hours | true | HC | 200 | 1 | 1 |
| hc1.reduceminutes | remaining time for nightmode | uint16 (>=0<=31999) | minutes | false | HC | 201 | 1 | 1 |
| dhw.mode | operating mode | enum [on\|off\|auto] |   | true | DHW | 0 | 1 | 1 |

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
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 |
| hc1.daytemp | day temperature | uint8 (>=10<=30) | C | true | HC | 60 | 1 | 1/2 |
| hc1.nighttemp | night temperature | uint8 (>=10<=30) | C | true | HC | 61 | 1 | 1/2 |
| hc1.designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 |
| hc1.offsettemp | offset temperature | int8 (>=-5<=5) | C | true | HC | 10 | 1 | 1/2 |
| hc1.holidaytemp | holiday temperature | uint8 (>=5<=30) | C | true | HC | 62 | 1 | 1/2 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1 |
| hc1.summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 |
| hc1.holidaymode | holiday mode | boolean |   | false | HC | 63 | 1 | 1 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-20<=10) | C | true | HC | 17 | 1 | 1 |
| hc1.nofrostmode | nofrost mode | enum [off\|outdoor\|room] |   | true | HC | 16 | 1 | 1 |
| hc1.roominfluence | room influence | uint8 (>=0<=10) | C | true | HC | 13 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 |
| hc1.flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | C | true | HC | 64 | 1 | 1 |
| hc1.heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.reducemode | reduce mode | enum [nofrost\|reduce\|room\|outdoor] |   | true | HC | 31 | 1 | 1 |
| hc1.controlmode | control mode | enum [outdoor\|room] |   | true | HC | 25 | 1 | 1 |
| hc1.control | control device | enum [off\|RC20\|RC3x] |   | true | HC | 41 | 1 | 1 |
| hc1.holidays | holiday dates | string |   | true | HC | 65 | 13 | 1 |
| hc1.vacations | vacation dates | string |   | true | HC | 78 | 13 | 1 |
| hc1.program | program | enum [own 1\|family\|morning\|evening\|am\|pm\|midday\|singles\|seniors\|new\|own 2] |   | true | HC | 26 | 1 | 1 |
| hc1.pause | pause time | uint8 (>=0<=99) | hours | true | HC | 91 | 1 | 1 |
| hc1.party | party time | uint8 (>=0<=99) | hours | true | HC | 92 | 1 | 1 |
| hc1.tempautotemp | temporary set temperature automode | uint8 (>=0<=30) | C | true | HC | 27 | 1 | 1/2 |
| hc1.noreducetemp | no reduce below temperature | int8 (>=-31<=10) | C | true | HC | 32 | 1 | 1 |
| hc1.reducetemp | off/reduce switch temperature | int8 (>=-20<=10) | C | true | HC | 33 | 1 | 1 |
| hc1.vacreducetemp | vacations off/reduce switch temperature | int8 (>=-20<=10) | C | true | HC | 93 | 1 | 1 |
| hc1.vacreducemode | vacations reduce mode | enum [nofrost\|reduce\|room\|outdoor] |   | true | HC | 94 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.dhwprio | dhw priority | boolean |   | true | HC | 34 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.switchtime1 | own1 program switchtime | string |   | true | HC | 95 | 8 | 1 |
| hc1.switchtime2 | own2 program switchtime | string |   | true | HC | 103 | 8 | 1 |
| dhw.mode | operating mode | enum [off\|on\|auto] |   | true | DHW | 0 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|on\|auto] |   | true | DHW | 4 | 1 | 1 |
| dhw.progmode | program | enum [std prog\|own prog] |   | true | DHW | 13 | 1 | 1 |
| dhw.circprog | circulation program | enum [std prog\|own prog] |   | true | DHW | 14 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 8 | 1 | 1 |
| dhw.disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 9 | 1 | 1 |
| dhw.disinfecthour | disinfection hour | uint8 (>=0<=23) |   | true | DHW | 15 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=60<=80) | C | true | DHW | 16 | 1 | 1 |
| dhw.onetimekey | one time key function | boolean |   | true | DHW | 17 | 1 | 1 |
| dhw.switchtime | program switchtime | string |   | true | DHW | 18 | 8 | 1 |
| dhw.circswitchtime | circulation program switchtime | string |   | true | DHW | 26 | 8 | 1 |
| dhw.holidays | holiday dates | string |   | true | DHW | 34 | 13 | 1 |
| dhw.vacations | vacation dates | string |   | true | DHW | 47 | 13 | 1 |

### RC20RF
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [off\|manual\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 |
| hc1.offtemp | temperature when mode is off | uint8 (>=0<=127) | C | true | HC | 111 | 1 | 1/2 |
| hc1.daytemp2 | day temperature T2 | uint8 (>=0<=127) | C | true | HC | 112 | 1 | 1/2 |
| hc1.daytemp3 | day temperature T3 | uint8 (>=0<=127) | C | true | HC | 113 | 1 | 1/2 |
| hc1.daytemp4 | day temperature T4 | uint8 (>=0<=127) | C | true | HC | 114 | 1 | 1/2 |
| hc1.nighttemp | night temperature T1 | uint8 (>=0<=127) | C | true | HC | 61 | 1 | 1/2 |
| hc1.switchtime | program switchtime | string |   | true | HC | 115 | 8 | 1 |

### RFM20 Remote
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |

### RC25
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 |
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 |
| hc1.daytemp | day temperature | uint8 (>=0<=127) | C | true | HC | 60 | 1 | 1/2 |
| hc1.nighttemp | night temperature | uint8 (>=0<=127) | C | true | HC | 61 | 1 | 1/2 |
| hc1.program | program | enum [family\|morning\|evening\|am\|pm\|midday\|singles\|seniors] |   | true | HC | 26 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 |
| hc1.tempautotemp | temporary set temperature automode | uint8 (>=0<=127) | C | true | HC | 27 | 1 | 1/2 |
| hc1.heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1 |
| hc1.summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 |

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
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [manual\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [eco\|comfort] |   | false | HC | 4 | 1 | 1 |
| hc1.ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 |
| hc1.manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 |
| hc1.comforttemp | comfort temperature | uint8 (>=0<=127) | C | true | HC | 7 | 1 | 1/2 |
| hc1.summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 |
| hc1.designtemp | design temperature | uint8 (>=0<=254) | C | true | HC | 9 | 1 | 1 |
| hc1.offsettemp | offset temperature | int8 (>=-126<=126) | C | true | HC | 10 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 |
| hc1.roominfluence | room influence | uint8 (>=0<=254) | C | true | HC | 13 | 1 | 1 |
| hc1.roominflfactor | room influence factor | uint8 (>=0<=25) |   | true | HC | 14 | 1 | 1/10 |
| hc1.curroominfl | current room influence | int16 (>=-3199<=3199) | C | false | HC | 15 | 1 | 1/10 |
| hc1.nofrostmode | nofrost mode | enum [room\|outdoor\|room outdoor] |   | true | HC | 16 | 1 | 1 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-126<=126) | C | true | HC | 17 | 1 | 1 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.summersetmode | set summer mode | enum [summer\|auto\|winter] |   | true | HC | 20 | 1 | 1 |
| hc1.hpoperatingmode | heatpump operating mode | enum [off\|auto\|heating\|cooling] |   | true | HC | 21 | 1 | 1 |
| hc1.summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 |
| hc1.hpoperatingstate | heatpump operating state | enum [heating\|off\|cooling] |   | false | HC | 23 | 1 | 1 |
| hc1.vacationmode | vacation mode | boolean |   | false | HC | 24 | 1 | 1 |
| hc1.controlmode | control mode | enum [optimized\|simple\|n/a\|room\|power] |   | true | HC | 25 | 1 | 1 |
| hc1.program | program | enum [prog 1\|prog 2] |   | true | HC | 26 | 1 | 1 |
| hc1.tempautotemp | temporary set temperature automode | int8 (>=-1<=30) | C | true | HC | 27 | 1 | 1/2 |
| hc1.cooltemp | cooling temperature | int8 (>=-1<=30) | C | true | HC | 28 | 1 | 1/2 |
| hc1.fastheatup | fast heatup | uint8 (>=0<=100) | % | true | HC | 29 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.reducemode | reduce mode | enum [outdoor\|room\|reduce] |   | true | HC | 31 | 1 | 1 |
| hc1.noreducetemp | no reduce below temperature | int8 (>=-126<=126) | C | true | HC | 32 | 1 | 1 |
| hc1.reducetemp | off/reduce switch temperature | int8 (>=-126<=126) | C | true | HC | 33 | 1 | 1 |
| hc1.dhwprio | dhw priority | boolean |   | true | HC | 34 | 1 | 1 |
| hc1.hpcooling | hp cooling | boolean |   | true | HC | 35 | 1 | 1 |
| hc1.coolingon | cooling on | boolean |   | false | HC | 36 | 1 | 1 |
| hc1.hpmode | HP Mode | enum [heating\|cooling\|heating & cooling] |   | true | HC | 37 | 1 | 1 |
| hc1.dewoffset | dew point offset | uint8 (>=2<=10) | K | true | HC | 38 | 1 | 1 |
| hc1.roomtempdiff | room temp difference | uint8 (>=0<=254) | K | true | HC | 39 | 1 | 1 |
| hc1.hpminflowtemp | HP min. flow temp. | uint8 (>=0<=254) | C | true | HC | 40 | 1 | 1 |
| hc1.control | control device | enum [RC310\|RC200\|RC100\|RC100H\|TC100] |   | true | HC | 41 | 1 | 1 |
| hc1.heatondelay | heat-on delay | uint8 (>=1<=48) | hours | true | HC | 44 | 1 | 1 |
| hc1.heatoffdelay | heat-off delay | uint8 (>=1<=48) | hours | true | HC | 45 | 1 | 1 |
| hc1.instantstart | instant start | uint8 (>=1<=10) | K | true | HC | 46 | 1 | 1 |
| hc1.boost | boost mode | boolean |   | true | HC | 47 | 1 | 1 |
| hc1.boosttime | boost time | uint8 (>=0<=254) | hours | true | HC | 48 | 1 | 1 |
| hc1.coolstart | cooling starttemp | uint8 (>=20<=35) | C | true | HC | 49 | 1 | 1 |
| hc1.coolondelay | cooling on delay | uint8 (>=1<=48) | hours | true | HC | 50 | 1 | 1 |
| hc1.cooloffdelay | cooling off delay | uint8 (>=1<=48) | hours | true | HC | 51 | 1 | 1 |
| hc1.switchprogmode | switch program mode | enum [level\|absolute] |   | true | HC | 52 | 1 | 1 |
| hc1.redthreshold | reduction threshold | int8 (>=12<=22) | C | true | HC | 53 | 1 | 1/2 |
| hc1.solarinfl | solar influence | uint8 (>=-5<=4294967295) | C | true | HC | 54 | 1 | 1 |
| hc1.currsolarinfl | curent solar influence | uint8 (>=0<=25) | C | false | HC | 55 | 1 | 1/10 |
| hc1.heatingpid | heating PID | enum [fast\|medium\|slow] |   | true | HC | 56 | 1 | 1 |
| hc1.pumpopt | pump optimization | boolean |   | true | HC | 57 | 1 | 1 |
| hc1.inttimefloor | integral time floor | uint8 (>=160<=320) | minutes | true | HC | 58 | 1 | 10 |
| hc1.inttime | integral time | uint8 (>=3<=80) | minutes | true | HC | 59 | 1 | 1 |
| dhw.mode | operating mode | enum [off\|on\|auto] |   | true | DHW | 0 | 1 | 1 |
| dhw.modetype | mode type | enum [off\|eco\|comfort\|eco+] |   | false | DHW | 1 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | true | DHW | 2 | 1 | 1 |
| dhw.settemplow | set low temperature | uint8 (>=0<=254) | C | true | DHW | 3 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 4 | 1 | 1 |
| dhw.chargeduration | charge duration | uint8 (>=0<=3810) | minutes | true | DHW | 5 | 1 | 15 |
| dhw.charge | charge | boolean |   | true | DHW | 6 | 1 | 1 |
| dhw.extra | extra | boolean |   | false | DHW | 7 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 8 | 1 | 1 |
| dhw.disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 9 | 1 | 1 |
| dhw.disinfecttime | disinfection time | uint8 (>=0<=1431) | minutes | true | DHW | 10 | 1 | 15 |
| dhw.dailyheating | daily heating | boolean |   | true | DHW | 11 | 1 | 1 |
| dhw.dailyheattime | daily heating time | uint8 (>=0<=1431) | minutes | true | DHW | 12 | 1 | 15 |

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
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [manual\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [eco\|comfort] |   | false | HC | 4 | 1 | 1 |
| hc1.ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 |
| hc1.manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 |
| hc1.comforttemp | comfort temperature | uint8 (>=0<=127) | C | true | HC | 7 | 1 | 1/2 |
| hc1.summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 |
| hc1.designtemp | design temperature | uint8 (>=0<=254) | C | true | HC | 9 | 1 | 1 |
| hc1.offsettemp | offset temperature | int8 (>=-126<=126) | C | true | HC | 10 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 |
| hc1.roominfluence | room influence | uint8 (>=0<=254) | C | true | HC | 13 | 1 | 1 |
| hc1.roominflfactor | room influence factor | uint8 (>=0<=25) |   | true | HC | 14 | 1 | 1/10 |
| hc1.curroominfl | current room influence | int16 (>=-3199<=3199) | C | false | HC | 15 | 1 | 1/10 |
| hc1.nofrostmode | nofrost mode | enum [room\|outdoor\|room outdoor] |   | true | HC | 16 | 1 | 1 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-126<=126) | C | true | HC | 17 | 1 | 1 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.summersetmode | set summer mode | enum [summer\|auto\|winter] |   | true | HC | 20 | 1 | 1 |
| hc1.hpoperatingmode | heatpump operating mode | enum [off\|auto\|heating\|cooling] |   | true | HC | 21 | 1 | 1 |
| hc1.summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 |
| hc1.hpoperatingstate | heatpump operating state | enum [heating\|off\|cooling] |   | false | HC | 23 | 1 | 1 |
| hc1.vacationmode | vacation mode | boolean |   | false | HC | 24 | 1 | 1 |
| hc1.controlmode | control mode | enum [weather compensated\|outside basepoint\|n/a\|room\|power\|constant] |   | true | HC | 25 | 1 | 1 |
| hc1.program | program | enum [prog 1\|prog 2] |   | true | HC | 26 | 1 | 1 |
| hc1.tempautotemp | temporary set temperature automode | int8 (>=-1<=30) | C | true | HC | 27 | 1 | 1/2 |
| hc1.cooltemp | cooling temperature | int8 (>=-1<=30) | C | true | HC | 28 | 1 | 1/2 |
| hc1.fastheatup | fast heatup | uint8 (>=0<=100) | % | true | HC | 29 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.reducemode | reduce mode | enum [outdoor\|room\|reduce] |   | true | HC | 31 | 1 | 1 |
| hc1.noreducetemp | no reduce below temperature | int8 (>=-126<=126) | C | true | HC | 32 | 1 | 1 |
| hc1.reducetemp | off/reduce switch temperature | int8 (>=-126<=126) | C | true | HC | 33 | 1 | 1 |
| hc1.dhwprio | dhw priority | boolean |   | true | HC | 34 | 1 | 1 |
| hc1.hpcooling | hp cooling | boolean |   | true | HC | 35 | 1 | 1 |
| hc1.coolingon | cooling on | boolean |   | false | HC | 36 | 1 | 1 |
| hc1.hpmode | HP Mode | enum [heating\|cooling\|heating & cooling] |   | true | HC | 37 | 1 | 1 |
| hc1.dewoffset | dew point offset | uint8 (>=2<=10) | K | true | HC | 38 | 1 | 1 |
| hc1.roomtempdiff | room temp difference | uint8 (>=0<=254) | K | true | HC | 39 | 1 | 1 |
| hc1.hpminflowtemp | HP min. flow temp. | uint8 (>=0<=254) | C | true | HC | 40 | 1 | 1 |
| hc1.control | control device | enum [RC310\|RC200\|RC100\|RC100H\|TC100] |   | true | HC | 41 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.remotehum | room humidity from remote | cmd [] (>=-1<=101) | % | true | HC | 43 | 1 | 1 |
| hc1.heatondelay | heat-on delay | uint8 (>=1<=48) | hours | true | HC | 44 | 1 | 1 |
| hc1.heatoffdelay | heat-off delay | uint8 (>=1<=48) | hours | true | HC | 45 | 1 | 1 |
| hc1.instantstart | instant start | uint8 (>=1<=10) | K | true | HC | 46 | 1 | 1 |
| hc1.boost | boost mode | boolean |   | true | HC | 47 | 1 | 1 |
| hc1.boosttime | boost time | uint8 (>=0<=254) | hours | true | HC | 48 | 1 | 1 |
| hc1.coolstart | cooling starttemp | uint8 (>=20<=35) | C | true | HC | 49 | 1 | 1 |
| hc1.coolondelay | cooling on delay | uint8 (>=1<=48) | hours | true | HC | 50 | 1 | 1 |
| hc1.cooloffdelay | cooling off delay | uint8 (>=1<=48) | hours | true | HC | 51 | 1 | 1 |
| hc1.switchprogmode | switch program mode | enum [level\|absolute] |   | true | HC | 52 | 1 | 1 |
| hc1.redthreshold | reduction threshold | int8 (>=12<=22) | C | true | HC | 53 | 1 | 1/2 |
| hc1.solarinfl | solar influence | uint8 (>=-5<=4294967295) | C | true | HC | 54 | 1 | 1 |
| hc1.currsolarinfl | curent solar influence | uint8 (>=0<=25) | C | false | HC | 55 | 1 | 1/10 |
| hc1.heatingpid | heating PID | enum [fast\|medium\|slow] |   | true | HC | 56 | 1 | 1 |
| hc1.pumpopt | pump optimization | boolean |   | true | HC | 57 | 1 | 1 |
| hc1.inttimefloor | integral time floor | uint8 (>=160<=320) | minutes | true | HC | 58 | 1 | 10 |
| hc1.inttime | integral time | uint8 (>=3<=80) | minutes | true | HC | 59 | 1 | 1 |
| dhw.mode | operating mode | enum [off\|normal\|comfort\|auto\|own prog] |   | true | DHW | 0 | 1 | 1 |
| dhw.modetype | mode type | enum [off\|eco\|comfort\|eco+] |   | false | DHW | 1 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | true | DHW | 2 | 1 | 1 |
| dhw.settemplow | set low temperature | uint8 (>=0<=254) | C | true | DHW | 3 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 4 | 1 | 1 |
| dhw.chargeduration | charge duration | uint8 (>=0<=3810) | minutes | true | DHW | 5 | 1 | 15 |
| dhw.charge | charge | boolean |   | true | DHW | 6 | 1 | 1 |
| dhw.extra | extra | boolean |   | false | DHW | 7 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 8 | 1 | 1 |
| dhw.disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 9 | 1 | 1 |
| dhw.disinfecttime | disinfection time | uint8 (>=0<=1431) | minutes | true | DHW | 10 | 1 | 15 |
| dhw.dailyheating | daily heating | boolean |   | true | DHW | 11 | 1 | 1 |
| dhw.dailyheattime | daily heating time | uint8 (>=0<=1431) | minutes | true | DHW | 12 | 1 | 15 |

### RC100, CR10, Moduline 1000/1010
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [off\|manual] |   | true | HC | 3 | 1 | 1 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |

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
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [manual\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [eco\|comfort] |   | false | HC | 4 | 1 | 1 |
| hc1.ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 |
| hc1.manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 |
| hc1.comforttemp | comfort temperature | uint8 (>=0<=127) | C | true | HC | 7 | 1 | 1/2 |
| hc1.summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 |
| hc1.designtemp | design temperature | uint8 (>=0<=254) | C | true | HC | 9 | 1 | 1 |
| hc1.offsettemp | offset temperature | int8 (>=-126<=126) | C | true | HC | 10 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 |
| hc1.roominfluence | room influence | uint8 (>=0<=254) | C | true | HC | 13 | 1 | 1 |
| hc1.roominflfactor | room influence factor | uint8 (>=0<=25) |   | true | HC | 14 | 1 | 1/10 |
| hc1.curroominfl | current room influence | int16 (>=-3199<=3199) | C | false | HC | 15 | 1 | 1/10 |
| hc1.nofrostmode | nofrost mode | enum [room\|outdoor\|room outdoor] |   | true | HC | 16 | 1 | 1 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-126<=126) | C | true | HC | 17 | 1 | 1 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.summersetmode | set summer mode | enum [summer\|auto\|winter] |   | true | HC | 20 | 1 | 1 |
| hc1.hpoperatingmode | heatpump operating mode | enum [off\|auto\|heating\|cooling] |   | true | HC | 21 | 1 | 1 |
| hc1.summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 |
| hc1.hpoperatingstate | heatpump operating state | enum [heating\|off\|cooling] |   | false | HC | 23 | 1 | 1 |
| hc1.vacationmode | vacation mode | boolean |   | false | HC | 24 | 1 | 1 |
| hc1.controlmode | control mode | enum [weather compensated\|outside basepoint\|n/a\|room\|power\|constant] |   | true | HC | 25 | 1 | 1 |
| hc1.program | program | enum [prog 1\|prog 2] |   | true | HC | 26 | 1 | 1 |
| hc1.tempautotemp | temporary set temperature automode | int8 (>=-1<=30) | C | true | HC | 27 | 1 | 1/2 |
| hc1.cooltemp | cooling temperature | int8 (>=-1<=30) | C | true | HC | 28 | 1 | 1/2 |
| hc1.fastheatup | fast heatup | uint8 (>=0<=100) | % | true | HC | 29 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.reducemode | reduce mode | enum [outdoor\|room\|reduce] |   | true | HC | 31 | 1 | 1 |
| hc1.noreducetemp | no reduce below temperature | int8 (>=-126<=126) | C | true | HC | 32 | 1 | 1 |
| hc1.reducetemp | off/reduce switch temperature | int8 (>=-126<=126) | C | true | HC | 33 | 1 | 1 |
| hc1.dhwprio | dhw priority | boolean |   | true | HC | 34 | 1 | 1 |
| hc1.hpcooling | hp cooling | boolean |   | true | HC | 35 | 1 | 1 |
| hc1.coolingon | cooling on | boolean |   | false | HC | 36 | 1 | 1 |
| hc1.hpmode | HP Mode | enum [heating\|cooling\|heating & cooling] |   | true | HC | 37 | 1 | 1 |
| hc1.dewoffset | dew point offset | uint8 (>=2<=10) | K | true | HC | 38 | 1 | 1 |
| hc1.roomtempdiff | room temp difference | uint8 (>=0<=254) | K | true | HC | 39 | 1 | 1 |
| hc1.hpminflowtemp | HP min. flow temp. | uint8 (>=0<=254) | C | true | HC | 40 | 1 | 1 |
| hc1.control | control device | enum [RC310\|RC200\|RC100\|RC100H\|TC100] |   | true | HC | 41 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.remotehum | room humidity from remote | cmd [] (>=-1<=101) | % | true | HC | 43 | 1 | 1 |
| hc1.heatondelay | heat-on delay | uint8 (>=1<=48) | hours | true | HC | 44 | 1 | 1 |
| hc1.heatoffdelay | heat-off delay | uint8 (>=1<=48) | hours | true | HC | 45 | 1 | 1 |
| hc1.instantstart | instant start | uint8 (>=1<=10) | K | true | HC | 46 | 1 | 1 |
| hc1.boost | boost mode | boolean |   | true | HC | 47 | 1 | 1 |
| hc1.boosttime | boost time | uint8 (>=0<=254) | hours | true | HC | 48 | 1 | 1 |
| hc1.coolstart | cooling starttemp | uint8 (>=20<=35) | C | true | HC | 49 | 1 | 1 |
| hc1.coolondelay | cooling on delay | uint8 (>=1<=48) | hours | true | HC | 50 | 1 | 1 |
| hc1.cooloffdelay | cooling off delay | uint8 (>=1<=48) | hours | true | HC | 51 | 1 | 1 |
| hc1.switchprogmode | switch program mode | enum [level\|absolute] |   | true | HC | 52 | 1 | 1 |
| hc1.redthreshold | reduction threshold | int8 (>=12<=22) | C | true | HC | 53 | 1 | 1/2 |
| hc1.solarinfl | solar influence | uint8 (>=-5<=4294967295) | C | true | HC | 54 | 1 | 1 |
| hc1.currsolarinfl | curent solar influence | uint8 (>=0<=25) | C | false | HC | 55 | 1 | 1/10 |
| hc1.heatingpid | heating PID | enum [fast\|medium\|slow] |   | true | HC | 56 | 1 | 1 |
| hc1.pumpopt | pump optimization | boolean |   | true | HC | 57 | 1 | 1 |
| hc1.inttimefloor | integral time floor | uint8 (>=160<=320) | minutes | true | HC | 58 | 1 | 10 |
| hc1.inttime | integral time | uint8 (>=3<=80) | minutes | true | HC | 59 | 1 | 1 |
| dhw.mode | operating mode | enum [normal\|comfort\|eco+] |   | true | DHW | 0 | 1 | 1 |
| dhw.modetype | mode type | enum [off\|eco\|comfort\|eco+] |   | false | DHW | 1 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | true | DHW | 2 | 1 | 1 |
| dhw.settemplow | set low temperature | uint8 (>=0<=254) | C | true | DHW | 3 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 4 | 1 | 1 |
| dhw.chargeduration | charge duration | uint8 (>=0<=3810) | minutes | true | DHW | 5 | 1 | 15 |
| dhw.charge | charge | boolean |   | true | DHW | 6 | 1 | 1 |
| dhw.extra | extra | boolean |   | false | DHW | 7 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 8 | 1 | 1 |
| dhw.disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 9 | 1 | 1 |
| dhw.disinfecttime | disinfection time | uint8 (>=0<=1431) | minutes | true | DHW | 10 | 1 | 15 |
| dhw.dailyheating | daily heating | boolean |   | true | DHW | 11 | 1 | 1 |
| dhw.dailyheattime | daily heating time | uint8 (>=0<=1431) | minutes | true | DHW | 12 | 1 | 15 |

### Comfort RF
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=-15999<=15999) | C | false | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [auto\|off] |   | false | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [off\|on] |   | false | HC | 4 | 1 | 1 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |

### CRF200S
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=-15999<=15999) | C | false | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [auto\|off] |   | false | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [off\|on] |   | false | HC | 4 | 1 | 1 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |

### Comfort+2RF
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=-15999<=15999) | C | false | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [auto\|off] |   | false | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [off\|on] |   | false | HC | 4 | 1 | 1 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |

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
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [off\|manual\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [eco\|comfort] |   | false | HC | 4 | 1 | 1 |
| hc1.ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 |
| hc1.manualtemp | manual temperature | uint8 (>=0<=127) | C | true | HC | 6 | 1 | 1/2 |
| hc1.comforttemp | comfort temperature | uint8 (>=0<=127) | C | true | HC | 7 | 1 | 1/2 |
| hc1.summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 |
| hc1.designtemp | design temperature | uint8 (>=0<=254) | C | true | HC | 9 | 1 | 1 |
| hc1.offsettemp | offset temperature | int8 (>=-126<=126) | C | true | HC | 10 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 |
| hc1.roominfluence | room influence | uint8 (>=0<=254) | C | true | HC | 13 | 1 | 1 |
| hc1.roominflfactor | room influence factor | uint8 (>=0<=25) |   | true | HC | 14 | 1 | 1/10 |
| hc1.curroominfl | current room influence | int16 (>=-3199<=3199) | C | false | HC | 15 | 1 | 1/10 |
| hc1.nofrostmode | nofrost mode | enum [room\|outdoor\|room outdoor] |   | true | HC | 16 | 1 | 1 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-126<=126) | C | true | HC | 17 | 1 | 1 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.summersetmode | set summer mode | enum [summer\|auto\|winter] |   | true | HC | 20 | 1 | 1 |
| hc1.hpoperatingmode | heatpump operating mode | enum [off\|auto\|heating\|cooling] |   | true | HC | 21 | 1 | 1 |
| hc1.summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 |
| hc1.hpoperatingstate | heatpump operating state | enum [heating\|off\|cooling] |   | false | HC | 23 | 1 | 1 |
| hc1.vacationmode | vacation mode | boolean |   | false | HC | 24 | 1 | 1 |
| hc1.controlmode | control mode | enum [weather compensated\|outside basepoint\|n/a\|room\|power\|constant] |   | true | HC | 25 | 1 | 1 |
| hc1.program | program | enum [prog 1\|prog 2] |   | true | HC | 26 | 1 | 1 |
| hc1.tempautotemp | temporary set temperature automode | int8 (>=-1<=30) | C | true | HC | 27 | 1 | 1/2 |
| hc1.cooltemp | cooling temperature | int8 (>=-1<=30) | C | true | HC | 28 | 1 | 1/2 |
| hc1.fastheatup | fast heatup | uint8 (>=0<=100) | % | true | HC | 29 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.reducemode | reduce mode | enum [outdoor\|room\|reduce] |   | true | HC | 31 | 1 | 1 |
| hc1.noreducetemp | no reduce below temperature | int8 (>=-126<=126) | C | true | HC | 32 | 1 | 1 |
| hc1.reducetemp | off/reduce switch temperature | int8 (>=-126<=126) | C | true | HC | 33 | 1 | 1 |
| hc1.dhwprio | dhw priority | boolean |   | true | HC | 34 | 1 | 1 |
| hc1.hpcooling | hp cooling | boolean |   | true | HC | 35 | 1 | 1 |
| hc1.coolingon | cooling on | boolean |   | false | HC | 36 | 1 | 1 |
| hc1.hpmode | HP Mode | enum [heating\|cooling\|heating & cooling] |   | true | HC | 37 | 1 | 1 |
| hc1.dewoffset | dew point offset | uint8 (>=2<=10) | K | true | HC | 38 | 1 | 1 |
| hc1.roomtempdiff | room temp difference | uint8 (>=0<=254) | K | true | HC | 39 | 1 | 1 |
| hc1.hpminflowtemp | HP min. flow temp. | uint8 (>=0<=254) | C | true | HC | 40 | 1 | 1 |
| hc1.control | control device | enum [off\|-\|RC100\|RC100H\|-\|RC120RF\|RC220/RT800\|single] |   | true | HC | 41 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.remotehum | room humidity from remote | cmd [] (>=-1<=101) | % | true | HC | 43 | 1 | 1 |
| hc1.heatondelay | heat-on delay | uint8 (>=1<=48) | hours | true | HC | 44 | 1 | 1 |
| hc1.heatoffdelay | heat-off delay | uint8 (>=1<=48) | hours | true | HC | 45 | 1 | 1 |
| hc1.instantstart | instant start | uint8 (>=1<=10) | K | true | HC | 46 | 1 | 1 |
| hc1.boost | boost mode | boolean |   | true | HC | 47 | 1 | 1 |
| hc1.boosttime | boost time | uint8 (>=0<=254) | hours | true | HC | 48 | 1 | 1 |
| hc1.coolstart | cooling starttemp | uint8 (>=20<=35) | C | true | HC | 49 | 1 | 1 |
| hc1.coolondelay | cooling on delay | uint8 (>=1<=48) | hours | true | HC | 50 | 1 | 1 |
| hc1.cooloffdelay | cooling off delay | uint8 (>=1<=48) | hours | true | HC | 51 | 1 | 1 |
| hc1.switchprogmode | switch program mode | enum [level\|absolute] |   | true | HC | 52 | 1 | 1 |
| hc1.redthreshold | reduction threshold | int8 (>=12<=22) | C | true | HC | 53 | 1 | 1/2 |
| hc1.solarinfl | solar influence | uint8 (>=-5<=4294967295) | C | true | HC | 54 | 1 | 1 |
| hc1.currsolarinfl | curent solar influence | uint8 (>=0<=25) | C | false | HC | 55 | 1 | 1/10 |
| hc1.heatingpid | heating PID | enum [fast\|medium\|slow] |   | true | HC | 56 | 1 | 1 |
| hc1.pumpopt | pump optimization | boolean |   | true | HC | 57 | 1 | 1 |
| hc1.inttimefloor | integral time floor | uint8 (>=160<=320) | minutes | true | HC | 58 | 1 | 10 |
| hc1.inttime | integral time | uint8 (>=3<=80) | minutes | true | HC | 59 | 1 | 1 |
| dhw.mode | operating mode | enum [off\|eco+\|eco\|comfort\|auto] |   | true | DHW | 0 | 1 | 1 |
| dhw.modetype | mode type | enum [off\|eco\|comfort\|eco+] |   | false | DHW | 1 | 1 | 1 |
| dhw.settemp | set temperature | uint8 (>=0<=254) | C | true | DHW | 2 | 1 | 1 |
| dhw.settemplow | set low temperature | uint8 (>=0<=254) | C | true | DHW | 3 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 4 | 1 | 1 |
| dhw.chargeduration | charge duration | uint8 (>=0<=3810) | minutes | true | DHW | 5 | 1 | 15 |
| dhw.charge | charge | boolean |   | true | DHW | 6 | 1 | 1 |
| dhw.extra | extra | boolean |   | false | DHW | 7 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 8 | 1 | 1 |
| dhw.disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 9 | 1 | 1 |
| dhw.disinfecttime | disinfection time | uint8 (>=0<=1431) | minutes | true | DHW | 10 | 1 | 15 |
| dhw.dailyheating | daily heating | boolean |   | true | DHW | 11 | 1 | 1 |
| dhw.dailyheattime | daily heating time | uint8 (>=0<=1431) | minutes | true | DHW | 12 | 1 | 15 |

### ES72, RC20
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 |
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 |
| hc1.daytemp | day temperature | uint8 (>=0<=127) | C | true | HC | 60 | 1 | 1/2 |
| hc1.nighttemp | night temperature | uint8 (>=0<=127) | C | true | HC | 61 | 1 | 1/2 |
| hc1.program | program | enum [family\|morning\|evening\|am\|pm\|midday\|singles\|seniors] |   | true | HC | 26 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 |
| hc1.tempautotemp | temporary set temperature automode | uint8 (>=0<=127) | C | true | HC | 27 | 1 | 1/2 |
| hc1.heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 |
| hc1.summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 |
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 |
| minexttemp | minimal external temperature | int8 (>=-126<=126) | C | true | DEVICE_DATA | 51 | 1 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 |
| hc1.daytemp | day temperature | uint8 (>=0<=127) | C | true | HC | 60 | 1 | 1/2 |
| hc1.nighttemp | night temperature | uint8 (>=0<=127) | C | true | HC | 61 | 1 | 1/2 |
| hc1.program | program | enum [family\|morning\|evening\|am\|pm\|midday\|singles\|seniors] |   | true | HC | 26 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=0<=254) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=0<=254) | C | true | HC | 12 | 1 | 1 |
| hc1.tempautotemp | temporary set temperature automode | uint8 (>=0<=127) | C | true | HC | 27 | 1 | 1/2 |
| hc1.heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=10<=30) | C | true | HC | 8 | 1 | 1 |
| hc1.summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 |

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
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 |
| hc1.daytemp | day temperature | uint8 (>=10<=30) | C | true | HC | 60 | 1 | 1/2 |
| hc1.nighttemp | night temperature | uint8 (>=10<=30) | C | true | HC | 61 | 1 | 1/2 |
| hc1.designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 |
| hc1.offsettemp | offset temperature | int8 (>=-5<=5) | C | true | HC | 10 | 1 | 1/2 |
| hc1.holidaytemp | holiday temperature | uint8 (>=5<=30) | C | true | HC | 62 | 1 | 1/2 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1 |
| hc1.summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 |
| hc1.holidaymode | holiday mode | boolean |   | false | HC | 63 | 1 | 1 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-20<=10) | C | true | HC | 17 | 1 | 1 |
| hc1.nofrostmode | nofrost mode | enum [off\|outdoor\|room] |   | true | HC | 16 | 1 | 1 |
| hc1.roominfluence | room influence | uint8 (>=0<=10) | C | true | HC | 13 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 |
| hc1.flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | C | true | HC | 64 | 1 | 1 |
| hc1.heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.reducemode | reduce mode | enum [nofrost\|reduce\|room\|outdoor] |   | true | HC | 31 | 1 | 1 |
| hc1.controlmode | control mode | enum [outdoor\|room] |   | true | HC | 25 | 1 | 1 |
| hc1.control | control device | enum [off\|RC20\|RC3x] |   | true | HC | 41 | 1 | 1 |
| hc1.holidays | holiday dates | string |   | true | HC | 65 | 13 | 1 |
| hc1.vacations | vacation dates | string |   | true | HC | 78 | 13 | 1 |
| hc1.program | program | enum [own 1\|family\|morning\|evening\|am\|pm\|midday\|singles\|seniors\|new\|own 2] |   | true | HC | 26 | 1 | 1 |
| hc1.pause | pause time | uint8 (>=0<=99) | hours | true | HC | 91 | 1 | 1 |
| hc1.party | party time | uint8 (>=0<=99) | hours | true | HC | 92 | 1 | 1 |
| hc1.tempautotemp | temporary set temperature automode | uint8 (>=0<=30) | C | true | HC | 27 | 1 | 1/2 |
| hc1.noreducetemp | no reduce below temperature | int8 (>=-31<=10) | C | true | HC | 32 | 1 | 1 |
| hc1.reducetemp | off/reduce switch temperature | int8 (>=-20<=10) | C | true | HC | 33 | 1 | 1 |
| hc1.vacreducetemp | vacations off/reduce switch temperature | int8 (>=-20<=10) | C | true | HC | 93 | 1 | 1 |
| hc1.vacreducemode | vacations reduce mode | enum [nofrost\|reduce\|room\|outdoor] |   | true | HC | 94 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.dhwprio | dhw priority | boolean |   | true | HC | 34 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.switchtime1 | own1 program switchtime | string |   | true | HC | 95 | 8 | 1 |
| hc1.switchtime2 | own2 program switchtime | string |   | true | HC | 103 | 8 | 1 |
| dhw.mode | operating mode | enum [off\|on\|auto] |   | true | DHW | 0 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|on\|auto] |   | true | DHW | 4 | 1 | 1 |
| dhw.progmode | program | enum [std prog\|own prog] |   | true | DHW | 13 | 1 | 1 |
| dhw.circprog | circulation program | enum [std prog\|own prog] |   | true | DHW | 14 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 8 | 1 | 1 |
| dhw.disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 9 | 1 | 1 |
| dhw.disinfecthour | disinfection hour | uint8 (>=0<=23) |   | true | DHW | 15 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=254) | C | true | DHW | 16 | 1 | 1 |
| dhw.onetimekey | one time key function | boolean |   | true | DHW | 17 | 1 | 1 |
| dhw.switchtime | program switchtime | string |   | true | DHW | 18 | 8 | 1 |
| dhw.circswitchtime | circulation program switchtime | string |   | true | DHW | 26 | 8 | 1 |
| dhw.holidays | holiday dates | string |   | true | DHW | 34 | 13 | 1 |
| dhw.vacations | vacation dates | string |   | true | DHW | 47 | 13 | 1 |

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
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [night\|day\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [night\|day] |   | false | HC | 4 | 1 | 1 |
| hc1.daytemp | day temperature | uint8 (>=10<=30) | C | true | HC | 60 | 1 | 1/2 |
| hc1.nighttemp | night temperature | uint8 (>=10<=30) | C | true | HC | 61 | 1 | 1/2 |
| hc1.designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 |
| hc1.offsettemp | offset temperature | int8 (>=-5<=5) | C | true | HC | 10 | 1 | 1/2 |
| hc1.holidaytemp | holiday temperature | uint8 (>=5<=30) | C | true | HC | 62 | 1 | 1/2 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1 |
| hc1.summermode | summer mode | enum [winter\|summer] |   | false | HC | 22 | 1 | 1 |
| hc1.holidaymode | holiday mode | boolean |   | false | HC | 63 | 1 | 1 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-20<=10) | C | true | HC | 17 | 1 | 1 |
| hc1.nofrostmode | nofrost mode | enum [off\|outdoor\|room] |   | true | HC | 16 | 1 | 1 |
| hc1.roominfluence | room influence | uint8 (>=0<=10) | C | true | HC | 13 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 |
| hc1.flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | C | true | HC | 64 | 1 | 1 |
| hc1.heatingtype | heating type | enum [off\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.reducemode | reduce mode | enum [nofrost\|reduce\|room\|outdoor] |   | true | HC | 31 | 1 | 1 |
| hc1.controlmode | control mode | enum [outdoor\|room] |   | true | HC | 25 | 1 | 1 |
| hc1.control | control device | enum [off\|RC20\|RC3x] |   | true | HC | 41 | 1 | 1 |
| hc1.holidays | holiday dates | string |   | true | HC | 65 | 13 | 1 |
| hc1.vacations | vacation dates | string |   | true | HC | 78 | 13 | 1 |
| hc1.program | program | enum [own 1\|family\|morning\|evening\|am\|pm\|midday\|singles\|seniors\|new\|own 2] |   | true | HC | 26 | 1 | 1 |
| hc1.pause | pause time | uint8 (>=0<=99) | hours | true | HC | 91 | 1 | 1 |
| hc1.party | party time | uint8 (>=0<=99) | hours | true | HC | 92 | 1 | 1 |
| hc1.tempautotemp | temporary set temperature automode | uint8 (>=0<=30) | C | true | HC | 27 | 1 | 1/2 |
| hc1.noreducetemp | no reduce below temperature | int8 (>=-31<=10) | C | true | HC | 32 | 1 | 1 |
| hc1.reducetemp | off/reduce switch temperature | int8 (>=-20<=10) | C | true | HC | 33 | 1 | 1 |
| hc1.vacreducetemp | vacations off/reduce switch temperature | int8 (>=-20<=10) | C | true | HC | 93 | 1 | 1 |
| hc1.vacreducemode | vacations reduce mode | enum [nofrost\|reduce\|room\|outdoor] |   | true | HC | 94 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.dhwprio | dhw priority | boolean |   | true | HC | 34 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.switchtime1 | own1 program switchtime | string |   | true | HC | 95 | 8 | 1 |
| hc1.switchtime2 | own2 program switchtime | string |   | true | HC | 103 | 8 | 1 |
| dhw.mode | operating mode | enum [off\|on\|auto] |   | true | DHW | 0 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|on\|auto] |   | true | DHW | 4 | 1 | 1 |
| dhw.progmode | program | enum [std prog\|own prog] |   | true | DHW | 13 | 1 | 1 |
| dhw.circprog | circulation program | enum [std prog\|own prog] |   | true | DHW | 14 | 1 | 1 |
| dhw.disinfecting | disinfecting | boolean |   | true | DHW | 8 | 1 | 1 |
| dhw.disinfectday | disinfection day | enum [mo\|tu\|we\|th\|fr\|sa\|su\|all] |   | true | DHW | 9 | 1 | 1 |
| dhw.disinfecthour | disinfection hour | uint8 (>=0<=23) |   | true | DHW | 15 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=60<=80) | C | true | DHW | 16 | 1 | 1 |
| dhw.onetimekey | one time key function | boolean |   | true | DHW | 17 | 1 | 1 |
| dhw.switchtime | program switchtime | string |   | true | DHW | 18 | 8 | 1 |
| dhw.circswitchtime | circulation program switchtime | string |   | true | DHW | 26 | 8 | 1 |
| dhw.holidays | holiday dates | string |   | true | DHW | 34 | 13 | 1 |
| dhw.vacations | vacation dates | string |   | true | DHW | 47 | 13 | 1 |

### FW100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 |
| display | display | enum [dhw temperature\|date\|external temperature] |   | true | DEVICE_DATA | 65 | 1 | 1 |
| language | language | enum [german\|italian\|french\|dutch] |   | true | DEVICE_DATA | 66 | 1 | 1 |
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 |
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 |
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 |
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 |
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 |
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 |
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 |
| hc1.heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 202 | 1 | 1/2 |
| hc1.ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 |
| hc1.control | control device | enum [off\|FB10\|FB100] |   | true | HC | 41 | 1 | 1 |
| hc1.program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 26 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 |
| hc1.roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 203 | 1 | 1 |
| hc1.holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 63 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 204 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 |
| hc1.designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 |
| hc1.roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 |
| hc1.roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 |
| hc1.heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 25 | 1 | 1 |
| dhw.charge | charge | boolean |   | true | DHW | 6 | 1 | 1 |

### FW200
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 |
| display | display | enum [dhw temperature\|date\|external temperature] |   | true | DEVICE_DATA | 65 | 1 | 1 |
| language | language | enum [german\|italian\|french\|dutch] |   | true | DEVICE_DATA | 66 | 1 | 1 |
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 |
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 |
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 |
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 |
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 |
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 |
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 |
| hc1.heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 202 | 1 | 1/2 |
| hc1.ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 |
| hc1.control | control device | enum [off\|FB10\|FB100] |   | true | HC | 41 | 1 | 1 |
| hc1.program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 26 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 |
| hc1.roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 203 | 1 | 1 |
| hc1.holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 63 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 204 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 |
| hc1.designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 |
| hc1.roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 |
| hc1.roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 |
| hc1.heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 25 | 1 | 1 |
| dhw.charge | charge | boolean |   | true | DHW | 6 | 1 | 1 |

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
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 |
| hc1.heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 202 | 1 | 1/2 |
| hc1.ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 |
| hc1.control | control device | enum [off\|FB10\|FB100] |   | true | HC | 41 | 1 | 1 |
| hc1.program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 26 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 |
| hc1.roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 203 | 1 | 1 |
| hc1.holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 63 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 204 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 |
| hc1.designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 |
| hc1.roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 |
| hc1.roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 |
| hc1.heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 25 | 1 | 1 |
| dhw.charge | charge | boolean |   | true | DHW | 6 | 1 | 1 |

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
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 |
| hc1.heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 202 | 1 | 1/2 |
| hc1.ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 |
| hc1.control | control device | enum [off\|FB10\|FB100] |   | true | HC | 41 | 1 | 1 |
| hc1.program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 26 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 |
| hc1.roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 203 | 1 | 1 |
| hc1.holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 63 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 204 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 |
| hc1.designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 |
| hc1.roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 |
| hc1.roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 |
| hc1.heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 25 | 1 | 1 |
| dhw.charge | charge | boolean |   | true | DHW | 6 | 1 | 1 |

### FB10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 |
| display | display | enum [dhw temperature\|date\|external temperature] |   | true | DEVICE_DATA | 65 | 1 | 1 |
| language | language | enum [german\|italian\|french\|dutch] |   | true | DEVICE_DATA | 66 | 1 | 1 |
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 |
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 |
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 |
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 |
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 |
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 |
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 |
| hc1.heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 202 | 1 | 1/2 |
| hc1.ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 |
| hc1.control | control device | enum [off\|FB10\|FB100] |   | true | HC | 41 | 1 | 1 |
| hc1.program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 26 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 |
| hc1.roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 203 | 1 | 1 |
| hc1.holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 63 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 204 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 |
| hc1.designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 |
| hc1.roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 |
| hc1.roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 |
| hc1.heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 25 | 1 | 1 |
| dhw.charge | charge | boolean |   | true | DHW | 6 | 1 | 1 |

### FB100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 |
| display | display | enum [dhw temperature\|date\|external temperature] |   | true | DEVICE_DATA | 65 | 1 | 1 |
| language | language | enum [german\|italian\|french\|dutch] |   | true | DEVICE_DATA | 66 | 1 | 1 |
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 |
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 |
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 |
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 |
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 |
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 |
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 |
| hc1.heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 202 | 1 | 1/2 |
| hc1.ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 |
| hc1.control | control device | enum [off\|FB10\|FB100] |   | true | HC | 41 | 1 | 1 |
| hc1.program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 26 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 |
| hc1.roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 203 | 1 | 1 |
| hc1.holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 63 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 204 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 |
| hc1.designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 |
| hc1.roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 |
| hc1.roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 |
| hc1.heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 25 | 1 | 1 |
| dhw.charge | charge | boolean |   | true | DHW | 6 | 1 | 1 |

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
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 |
| hc1.heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 202 | 1 | 1/2 |
| hc1.ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 |
| hc1.control | control device | enum [off\|FB10\|FB100] |   | true | HC | 41 | 1 | 1 |
| hc1.program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 26 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 |
| hc1.roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 203 | 1 | 1 |
| hc1.holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 63 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 204 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 |
| hc1.designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 |
| hc1.roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 |
| hc1.roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 |
| hc1.heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 25 | 1 | 1 |
| dhw.charge | charge | boolean |   | true | DHW | 6 | 1 | 1 |

### FW500
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 |
| display | display | enum [dhw temperature\|date\|external temperature] |   | true | DEVICE_DATA | 65 | 1 | 1 |
| language | language | enum [german\|italian\|french\|dutch] |   | true | DEVICE_DATA | 66 | 1 | 1 |
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 |
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 |
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 |
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 |
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 |
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 |
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 |
| hc1.heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 202 | 1 | 1/2 |
| hc1.ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 |
| hc1.control | control device | enum [off\|FB10\|FB100] |   | true | HC | 41 | 1 | 1 |
| hc1.program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 26 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 |
| hc1.roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 203 | 1 | 1 |
| hc1.holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 63 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 204 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 |
| hc1.designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 |
| hc1.roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 |
| hc1.roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 |
| hc1.heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 25 | 1 | 1 |
| dhw.charge | charge | boolean |   | true | DHW | 6 | 1 | 1 |

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
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 |
| hc1.heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 202 | 1 | 1/2 |
| hc1.ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 |
| hc1.control | control device | enum [off\|FB10\|FB100] |   | true | HC | 41 | 1 | 1 |
| hc1.program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 26 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 |
| hc1.roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 203 | 1 | 1 |
| hc1.holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 63 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 204 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 |
| hc1.designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 |
| hc1.roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 |
| hc1.roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 |
| hc1.heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 25 | 1 | 1 |
| dhw.charge | charge | boolean |   | true | DHW | 6 | 1 | 1 |

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
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 |
| hc1.heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 202 | 1 | 1/2 |
| hc1.ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 |
| hc1.control | control device | enum [off\|FB10\|FB100] |   | true | HC | 41 | 1 | 1 |
| hc1.program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 26 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 |
| hc1.roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 203 | 1 | 1 |
| hc1.holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 63 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 204 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 |
| hc1.designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 |
| hc1.roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 |
| hc1.roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 |
| hc1.heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 25 | 1 | 1 |
| dhw.charge | charge | boolean |   | true | DHW | 6 | 1 | 1 |

### FW120
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | true | DEVICE_DATA | 33 | 13 | 1 |
| display | display | enum [dhw temperature\|date\|external temperature] |   | true | DEVICE_DATA | 65 | 1 | 1 |
| language | language | enum [german\|italian\|french\|dutch] |   | true | DEVICE_DATA | 66 | 1 | 1 |
| hybridstrategy | hybrid control strategy | enum [co2 optimized\|cost optimized\|outside temp switched\|co2 cost mix] |   | true | DEVICE_DATA | 54 | 1 | 1 |
| switchovertemp | outside switchover temperature | int8 (>=-20<=20) | C | true | DEVICE_DATA | 55 | 1 | 1 |
| energycostratio | energy cost ratio | uint8 (>=0<=20) |   | true | DEVICE_DATA | 56 | 1 | 1/10 |
| fossilefactor | fossile energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 57 | 1 | 1/10 |
| electricfactor | electric energy factor | uint8 (>=0<=5) |   | true | DEVICE_DATA | 58 | 1 | 1/10 |
| delayboiler | delay boiler support | uint8 (>=5<=120) | minutes | true | DEVICE_DATA | 59 | 1 | 1 |
| tempdiffboiler | temp diff boiler support | uint8 (>=1<=99) | C | true | DEVICE_DATA | 60 | 1 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/10 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |
| hc1.mode | operating mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 3 | 1 | 1 |
| hc1.modetype | mode type | enum [nofrost\|eco\|heat] |   | false | HC | 4 | 1 | 1 |
| hc1.heattemp | heat temperature | uint8 (>=0<=127) | C | true | HC | 202 | 1 | 1/2 |
| hc1.ecotemp | eco temperature | uint8 (>=0<=127) | C | true | HC | 5 | 1 | 1/2 |
| hc1.nofrosttemp | nofrost temperature | int8 (>=-63<=63) | C | true | HC | 17 | 1 | 1/2 |
| hc1.control | control device | enum [off\|FB10\|FB100] |   | true | HC | 41 | 1 | 1 |
| hc1.program | program | enum [prog a\|prog b\|prog c\|prog d\|prog e\|prog f] |   | true | HC | 26 | 1 | 1 |
| hc1.remotetemp | room temperature from remote | cmd [] (>=-1<=101) | C | true | HC | 42 | 1 | 1/10 |
| hc1.targetflowtemp | target flow temperature | uint8 (>=0<=254) | C | false | HC | 18 | 1 | 1 |
| hc1.summertemp | summer temperature | uint8 (>=9<=25) | C | true | HC | 8 | 1 | 1/2 |
| hc1.roomsensor | room sensor | enum [extern\|intern\|auto] |   | true | HC | 203 | 1 | 1 |
| hc1.holidaymode | holiday mode | enum [nofrost\|eco\|heat\|auto] |   | true | HC | 63 | 1 | 1 |
| hc1.switchonoptimization | switch-on optimization | boolean |   | true | HC | 30 | 1 | 1 |
| hc1.heatup | heatup | enum [slow\|medium\|fast] |   | true | HC | 204 | 1 | 1 |
| hc1.minflowtemp | min flow temperature | uint8 (>=5<=70) | C | true | HC | 11 | 1 | 1 |
| hc1.maxflowtemp | max flow temperature | uint8 (>=30<=90) | C | true | HC | 12 | 1 | 1 |
| hc1.designtemp | design temperature | uint8 (>=30<=90) | C | true | HC | 9 | 1 | 1 |
| hc1.roominfluence | room influence | enum [off\|intern\|extern\|auto] |   | true | HC | 13 | 1 | 1 |
| hc1.roominflfactor | room influence factor | uint8 (>=0<=100) | % | true | HC | 14 | 1 | 10 |
| hc1.heatingtype | heating type | enum [off\|heatingcurve\|radiator\|convector\|floor] |   | true | HC | 19 | 1 | 1 |
| hc1.controlmode | control mode | enum [off\|unmixed\|unmixed IPM\|mixed IPM] |   | true | HC | 25 | 1 | 1 |
| dhw.charge | charge | boolean |   | true | DHW | 6 | 1 | 1 |

### RT800, RC220
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |

### RC100H, CR10H
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |

### TR120RF, CR20RF
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| errorcode | error code | string |   | false | DEVICE_DATA | 0 | 8 | 1 |
| lastcode | last error code | string |   | false | DEVICE_DATA | 8 | 25 | 1 |
| datetime | date/time | string |   | false | DEVICE_DATA | 33 | 13 | 1 |
| hc1.seltemp | selected room temperature | int16 (>=0<=30) | C | true | HC | 0 | 1 | 1/2 |
| hc1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | HC | 1 | 1 | 1/10 |
| hc1.haclimate | mqtt discovery current room temperature | enum [selTemp\|roomTemp] (>=5<=30) |   | false | HC | 2 | 1 | 1 |

## Devices of type *mixer*
### XCUMixer
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| hc1.flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | HC | 0 | 1 | 1/10 |
| hc1.valvestatus | mixing valve actuator (VC1) | uint8 (>=0<=100) | % | false | HC | 1 | 1 | 1 |
| hc1.flowsettemp | setpoint flow temperature | uint8 (>=0<=254) | C | true | HC | 2 | 1 | 1 |
| hc1.pumpstatus | pump status (PC1) | boolean |   | true | HC | 3 | 1 | 1 |
| hc1.activated | activated | boolean |   | true | HC | 4 | 1 | 1 |
| hc1.flow | Durchfluss | uint16 (>=0<=31999) | l/h | false | HC | 5 | 1 | 1 |
| hc1.setdiffpress | set differential pressure | uint8 (>=0<=12700) | mbar | true | HC | 6 | 1 | 50 |
| hc1.valvesettime | time to set valve | uint8 (>=10<=600) | seconds | true | HC | 7 | 1 | 10 |
| hc1.flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | K | true | HC | 8 | 1 | 1 |
| hc1.dhwprio | dhw priority | boolean |   | true | HC | 9 | 1 | 1 |

### MM10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| hc1.flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | HC | 0 | 1 | 1/10 |
| hc1.valvestatus | mixing valve actuator (VC1) | int8 (>=-100<=100) | % | false | HC | 1 | 1 | 1 |
| hc1.flowsettemp | setpoint flow temperature | uint8 (>=0<=254) | C | true | HC | 2 | 1 | 1 |
| hc1.pumpstatus | pump status (PC1) | boolean |   | true | HC | 3 | 1 | 1 |
| hc1.activated | activated | boolean |   | true | HC | 4 | 1 | 1 |
| hc1.valvesettime | time to set valve | uint8 (>=10<=600) | seconds | true | HC | 7 | 1 | 10 |

### IPM
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| hc1.flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | HC | 0 | 1 | 1/10 |
| hc1.valvestatus | mixing valve actuator (VC1) | uint8 (>=0<=100) | % | false | HC | 1 | 1 | 1 |
| hc1.flowsettemp | setpoint flow temperature | uint8 (>=0<=254) | C | true | HC | 2 | 1 | 1 |
| hc1.pumpstatus | pump status (PC1) | boolean |   | true | HC | 3 | 1 | 1 |
| hc1.flowtempvf | flow temperature in header (T0/Vf) | uint16 (>=0<=3199) | C | false | HC | 10 | 1 | 1/10 |

### IPM2
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| hc1.flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | HC | 0 | 1 | 1/10 |
| hc1.valvestatus | mixing valve actuator (VC1) | uint8 (>=0<=100) | % | false | HC | 1 | 1 | 1 |
| hc1.flowsettemp | setpoint flow temperature | uint8 (>=0<=254) | C | true | HC | 2 | 1 | 1 |
| hc1.pumpstatus | pump status (PC1) | boolean |   | true | HC | 3 | 1 | 1 |
| hc1.flowtempvf | flow temperature in header (T0/Vf) | uint16 (>=0<=3199) | C | false | HC | 10 | 1 | 1/10 |

### MM50
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| hc1.flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | HC | 0 | 1 | 1/10 |
| hc1.valvestatus | mixing valve actuator (VC1) | uint8 (>=0<=100) | % | false | HC | 1 | 1 | 1 |
| hc1.flowsettemp | setpoint flow temperature | uint8 (>=0<=254) | C | true | HC | 2 | 1 | 1 |
| hc1.pumpstatus | pump status (PC1) | boolean |   | true | HC | 3 | 1 | 1 |
| hc1.activated | activated | boolean |   | true | HC | 4 | 1 | 1 |
| hc1.flow | Durchfluss | uint16 (>=0<=31999) | l/h | false | HC | 5 | 1 | 1 |
| hc1.setdiffpress | set differential pressure | uint8 (>=0<=12700) | mbar | true | HC | 6 | 1 | 50 |
| hc1.valvesettime | time to set valve | uint8 (>=10<=600) | seconds | true | HC | 7 | 1 | 10 |
| hc1.flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | K | true | HC | 8 | 1 | 1 |
| hc1.dhwprio | dhw priority | boolean |   | true | HC | 9 | 1 | 1 |

### MM100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| hc1.flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | HC | 0 | 1 | 1/10 |
| hc1.valvestatus | mixing valve actuator (VC1) | uint8 (>=0<=100) | % | false | HC | 1 | 1 | 1 |
| hc1.flowsettemp | setpoint flow temperature | uint8 (>=0<=254) | C | true | HC | 2 | 1 | 1 |
| hc1.pumpstatus | pump status (PC1) | boolean |   | true | HC | 3 | 1 | 1 |
| hc1.activated | activated | boolean |   | true | HC | 4 | 1 | 1 |
| hc1.flow | Durchfluss | uint16 (>=0<=31999) | l/h | false | HC | 5 | 1 | 1 |
| hc1.setdiffpress | set differential pressure | uint8 (>=0<=12700) | mbar | true | HC | 6 | 1 | 50 |
| hc1.valvesettime | time to set valve | uint8 (>=10<=600) | seconds | true | HC | 7 | 1 | 10 |
| hc1.flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | K | true | HC | 8 | 1 | 1 |
| hc1.dhwprio | dhw priority | boolean |   | true | HC | 9 | 1 | 1 |

### MM200
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| hc1.flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | HC | 0 | 1 | 1/10 |
| hc1.valvestatus | mixing valve actuator (VC1) | uint8 (>=0<=100) | % | false | HC | 1 | 1 | 1 |
| hc1.flowsettemp | setpoint flow temperature | uint8 (>=0<=254) | C | true | HC | 2 | 1 | 1 |
| hc1.pumpstatus | pump status (PC1) | boolean |   | true | HC | 3 | 1 | 1 |
| hc1.activated | activated | boolean |   | true | HC | 4 | 1 | 1 |
| hc1.flow | Durchfluss | uint16 (>=0<=31999) | l/h | false | HC | 5 | 1 | 1 |
| hc1.setdiffpress | set differential pressure | uint8 (>=0<=12700) | mbar | true | HC | 6 | 1 | 50 |
| hc1.valvesettime | time to set valve | uint8 (>=10<=600) | seconds | true | HC | 7 | 1 | 10 |
| hc1.flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | K | true | HC | 8 | 1 | 1 |
| hc1.dhwprio | dhw priority | boolean |   | true | HC | 9 | 1 | 1 |

### MZ100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| hc1.flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | HC | 0 | 1 | 1/10 |
| hc1.valvestatus | mixing valve actuator (VC1) | uint8 (>=0<=100) | % | false | HC | 1 | 1 | 1 |
| hc1.flowsettemp | setpoint flow temperature | uint8 (>=0<=254) | C | true | HC | 2 | 1 | 1 |
| hc1.pumpstatus | pump status (PC1) | boolean |   | true | HC | 3 | 1 | 1 |
| hc1.activated | activated | boolean |   | true | HC | 4 | 1 | 1 |
| hc1.flow | Durchfluss | uint16 (>=0<=31999) | l/h | false | HC | 5 | 1 | 1 |
| hc1.setdiffpress | set differential pressure | uint8 (>=0<=12700) | mbar | true | HC | 6 | 1 | 50 |
| hc1.valvesettime | time to set valve | uint8 (>=10<=600) | seconds | true | HC | 7 | 1 | 10 |
| hc1.flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | K | true | HC | 8 | 1 | 1 |
| hc1.dhwprio | dhw priority | boolean |   | true | HC | 9 | 1 | 1 |

### HM210
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| hc1.flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | HC | 0 | 1 | 1/10 |
| hc1.valvestatus | mixing valve actuator (VC1) | uint8 (>=0<=100) | % | false | HC | 1 | 1 | 1 |
| hc1.flowsettemp | setpoint flow temperature | uint8 (>=0<=254) | C | true | HC | 2 | 1 | 1 |
| hc1.pumpstatus | pump status (PC1) | boolean |   | true | HC | 3 | 1 | 1 |
| hc1.activated | activated | boolean |   | true | HC | 4 | 1 | 1 |
| hc1.flow | Durchfluss | uint16 (>=0<=31999) | l/h | false | HC | 5 | 1 | 1 |
| hc1.setdiffpress | set differential pressure | uint8 (>=0<=12700) | mbar | true | HC | 6 | 1 | 50 |
| hc1.valvesettime | time to set valve | uint8 (>=10<=600) | seconds | true | HC | 7 | 1 | 10 |
| hc1.flowtempoffset | flow temperature offset for mixer | uint8 (>=0<=20) | K | true | HC | 8 | 1 | 1 |
| hc1.dhwprio | dhw priority | boolean |   | true | HC | 9 | 1 | 1 |

## Devices of type *solar*
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
| dhw.mintemp | minimum temperature | uint8 (>=0<=254) | C | true | DHW | 0 | 1 | 1 |
| solarenabled | solarmodule enabled | boolean |   | true | DEVICE_DATA | 16 | 1 | 1 |

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
| ts8 | (TS8) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 21 | 1 | 1/10 |
| ts16 | (TS16) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 22 | 1 | 1/10 |
| heatassistvalve | heat assistance valve (M1) | boolean |   | false | DEVICE_DATA | 19 | 1 | 1 |
| heatassistpower | heat assistance valve power (M1) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 |
| solarpump2 | pump 2 (PS4) | boolean |   | false | DEVICE_DATA | 24 | 1 | 1 |
| solarpump2mod | pump 2 modulation (PS4) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 25 | 1 | 1 |
| cyl2bottomtemp | second cylinder bottom temperature (TS5) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 26 | 1 | 1/10 |
| cyl3bottomtemp | third cylinder bottom temperature (TS11) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 27 | 1 | 1/10 |
| cyltoptemp | cylinder top temperature (TS10) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 28 | 1 | 1/10 |
| heatexchangertemp | heat exchanger temperature (TS6) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 29 | 1 | 1/10 |
| cylpumpmod | cylinder pump modulation (PS5) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 30 | 1 | 1 |
| valvestatus | valve status | boolean |   | false | DEVICE_DATA | 31 | 1 | 1 |
| vs1status | valve status VS1 | boolean |   | false | DEVICE_DATA | 32 | 1 | 1 |
| vs3status | valve status VS3 | boolean |   | false | DEVICE_DATA | 33 | 1 | 1 |
| transferpump | transfer pump | boolean |   | false | DEVICE_DATA | 34 | 1 | 1 |
| transferpumpmod | transfer pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 35 | 1 | 1 |
| collectormaxtemp | maximum collector temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 36 | 1 | 1 |
| collectormintemp | minimum collector temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 37 | 1 | 1 |
| energylasthour | energy last hour | uint24 (>=0<=1677721) | Wh | false | DEVICE_DATA | 13 | 2 | 1/10 |
| energytoday | total energy today | uint24 (>=0<=16777214) | Wh | false | DEVICE_DATA | 38 | 2 | 1 |
| energytotal | total energy | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 40 | 2 | 1/10 |
| pump2worktime | pump 2 working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 42 | 2 | 1 |
| m1worktime | differential control working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 44 | 2 | 1 |
| heattransfersystem | heattransfer system | boolean |   | true | DEVICE_DATA | 46 | 1 | 1 |
| externalcyl | external cylinder | boolean |   | true | DEVICE_DATA | 47 | 1 | 1 |
| thermaldisinfect | thermal disinfection | boolean |   | true | DEVICE_DATA | 48 | 1 | 1 |
| heatmetering | heatmetering | boolean |   | true | DEVICE_DATA | 49 | 1 | 1 |
| activated | activated | boolean |   | true | DEVICE_DATA | 50 | 1 | 1 |
| solarpumpmode | solar pump mode | enum [constant\|pwm\|analog] |   | true | DEVICE_DATA | 51 | 1 | 1 |
| solarpumpkick | solar pump kick | boolean |   | true | DEVICE_DATA | 52 | 1 | 1 |
| plainwatermode | plain water mode | boolean |   | true | DEVICE_DATA | 53 | 1 | 1 |
| doublematchflow | doublematchflow | boolean |   | true | DEVICE_DATA | 54 | 1 | 1 |
| pump2minmod | minimum pump 2 modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 55 | 1 | 1 |
| turnondiff2 | pump 2 turn on difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 56 | 1 | 1/10 |
| turnoffdiff2 | pump 2 turn off difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 57 | 1 | 1/10 |
| pump2kick | pump kick 2 | boolean |   | true | DEVICE_DATA | 58 | 1 | 1 |
| climatezone | climate zone | uint8 (>=0<=254) |   | true | DEVICE_DATA | 59 | 1 | 1 |
| collector1area | collector 1 area | uint16 (>=0<=3199) | m² | true | DEVICE_DATA | 60 | 1 | 1/10 |
| collector1type | collector 1 type | enum [flat\|vacuum] |   | true | DEVICE_DATA | 61 | 1 | 1 |
| collector2area | collector 2 area | uint16 (>=0<=3199) | m² | true | DEVICE_DATA | 62 | 1 | 1/10 |
| collector2type | collector 2 type | enum [flat\|vacuum] |   | true | DEVICE_DATA | 63 | 1 | 1 |
| cylpriority | cylinder priority | enum [cyl 1\|cyl 2] |   | true | DEVICE_DATA | 64 | 1 | 1 |
| heatcntflowtemp | heat counter flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 65 | 1 | 1/10 |
| heatcntrettemp | heat counter return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 66 | 1 | 1/10 |
| heatcnt | heat counter impulses | uint8 (>=0<=254) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| swapflowtemp | swap flow temperature (TS14) | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 68 | 1 | 1/10 |
| swaprettemp | swap return temperature (TS15) | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 69 | 1 | 1/10 |
| heatassiston | heat assistance on | int8 (>=-12<=12) | K | true | DEVICE_DATA | 70 | 1 | 1/10 |
| heatassistoff | heat assistance off | int8 (>=-12<=12) | K | true | DEVICE_DATA | 71 | 1 | 1/10 |

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
| ts8 | (TS8) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 21 | 1 | 1/10 |
| ts16 | (TS16) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 22 | 1 | 1/10 |
| heatassistvalve | heat assistance valve (M1) | boolean |   | false | DEVICE_DATA | 19 | 1 | 1 |
| heatassistpower | heat assistance valve power (M1) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 |
| solarpump2 | pump 2 (PS4) | boolean |   | false | DEVICE_DATA | 24 | 1 | 1 |
| solarpump2mod | pump 2 modulation (PS4) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 25 | 1 | 1 |
| cyl2bottomtemp | second cylinder bottom temperature (TS5) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 26 | 1 | 1/10 |
| cyl3bottomtemp | third cylinder bottom temperature (TS11) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 27 | 1 | 1/10 |
| cyltoptemp | cylinder top temperature (TS10) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 28 | 1 | 1/10 |
| heatexchangertemp | heat exchanger temperature (TS6) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 29 | 1 | 1/10 |
| cylpumpmod | cylinder pump modulation (PS5) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 30 | 1 | 1 |
| valvestatus | valve status | boolean |   | false | DEVICE_DATA | 31 | 1 | 1 |
| vs1status | valve status VS1 | boolean |   | false | DEVICE_DATA | 32 | 1 | 1 |
| vs3status | valve status VS3 | boolean |   | false | DEVICE_DATA | 33 | 1 | 1 |
| transferpump | transfer pump | boolean |   | false | DEVICE_DATA | 34 | 1 | 1 |
| transferpumpmod | transfer pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 35 | 1 | 1 |
| collectormaxtemp | maximum collector temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 36 | 1 | 1 |
| collectormintemp | minimum collector temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 37 | 1 | 1 |
| energylasthour | energy last hour | uint24 (>=0<=1677721) | Wh | false | DEVICE_DATA | 13 | 2 | 1/10 |
| energytoday | total energy today | uint24 (>=0<=16777214) | Wh | false | DEVICE_DATA | 38 | 2 | 1 |
| energytotal | total energy | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 40 | 2 | 1/10 |
| pump2worktime | pump 2 working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 42 | 2 | 1 |
| m1worktime | differential control working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 44 | 2 | 1 |
| heattransfersystem | heattransfer system | boolean |   | true | DEVICE_DATA | 46 | 1 | 1 |
| externalcyl | external cylinder | boolean |   | true | DEVICE_DATA | 47 | 1 | 1 |
| thermaldisinfect | thermal disinfection | boolean |   | true | DEVICE_DATA | 48 | 1 | 1 |
| heatmetering | heatmetering | boolean |   | true | DEVICE_DATA | 49 | 1 | 1 |
| activated | activated | boolean |   | true | DEVICE_DATA | 50 | 1 | 1 |
| solarpumpmode | solar pump mode | enum [constant\|pwm\|analog] |   | true | DEVICE_DATA | 51 | 1 | 1 |
| solarpumpkick | solar pump kick | boolean |   | true | DEVICE_DATA | 52 | 1 | 1 |
| plainwatermode | plain water mode | boolean |   | true | DEVICE_DATA | 53 | 1 | 1 |
| doublematchflow | doublematchflow | boolean |   | true | DEVICE_DATA | 54 | 1 | 1 |
| pump2minmod | minimum pump 2 modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 55 | 1 | 1 |
| turnondiff2 | pump 2 turn on difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 56 | 1 | 1/10 |
| turnoffdiff2 | pump 2 turn off difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 57 | 1 | 1/10 |
| pump2kick | pump kick 2 | boolean |   | true | DEVICE_DATA | 58 | 1 | 1 |
| climatezone | climate zone | uint8 (>=0<=254) |   | true | DEVICE_DATA | 59 | 1 | 1 |
| collector1area | collector 1 area | uint16 (>=0<=3199) | m² | true | DEVICE_DATA | 60 | 1 | 1/10 |
| collector1type | collector 1 type | enum [flat\|vacuum] |   | true | DEVICE_DATA | 61 | 1 | 1 |
| collector2area | collector 2 area | uint16 (>=0<=3199) | m² | true | DEVICE_DATA | 62 | 1 | 1/10 |
| collector2type | collector 2 type | enum [flat\|vacuum] |   | true | DEVICE_DATA | 63 | 1 | 1 |
| cylpriority | cylinder priority | enum [cyl 1\|cyl 2] |   | true | DEVICE_DATA | 64 | 1 | 1 |
| heatcntflowtemp | heat counter flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 65 | 1 | 1/10 |
| heatcntrettemp | heat counter return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 66 | 1 | 1/10 |
| heatcnt | heat counter impulses | uint8 (>=0<=254) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| swapflowtemp | swap flow temperature (TS14) | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 68 | 1 | 1/10 |
| swaprettemp | swap return temperature (TS15) | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 69 | 1 | 1/10 |
| heatassiston | heat assistance on | int8 (>=-12<=12) | K | true | DEVICE_DATA | 70 | 1 | 1/10 |
| heatassistoff | heat assistance off | int8 (>=-12<=12) | K | true | DEVICE_DATA | 71 | 1 | 1/10 |

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
| ts8 | (TS8) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 21 | 1 | 1/10 |
| ts16 | (TS16) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 22 | 1 | 1/10 |
| heatassistvalve | heat assistance valve (M1) | boolean |   | false | DEVICE_DATA | 19 | 1 | 1 |
| heatassistpower | heat assistance valve power (M1) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 23 | 1 | 1 |
| solarpump2 | pump 2 (PS4) | boolean |   | false | DEVICE_DATA | 24 | 1 | 1 |
| solarpump2mod | pump 2 modulation (PS4) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 25 | 1 | 1 |
| cyl2bottomtemp | second cylinder bottom temperature (TS5) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 26 | 1 | 1/10 |
| cyl3bottomtemp | third cylinder bottom temperature (TS11) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 27 | 1 | 1/10 |
| cyltoptemp | cylinder top temperature (TS10) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 28 | 1 | 1/10 |
| heatexchangertemp | heat exchanger temperature (TS6) | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 29 | 1 | 1/10 |
| cylpumpmod | cylinder pump modulation (PS5) | uint8 (>=0<=100) | % | false | DEVICE_DATA | 30 | 1 | 1 |
| valvestatus | valve status | boolean |   | false | DEVICE_DATA | 31 | 1 | 1 |
| vs1status | valve status VS1 | boolean |   | false | DEVICE_DATA | 32 | 1 | 1 |
| vs3status | valve status VS3 | boolean |   | false | DEVICE_DATA | 33 | 1 | 1 |
| transferpump | transfer pump | boolean |   | false | DEVICE_DATA | 34 | 1 | 1 |
| transferpumpmod | transfer pump modulation | uint8 (>=0<=100) | % | false | DEVICE_DATA | 35 | 1 | 1 |
| collectormaxtemp | maximum collector temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 36 | 1 | 1 |
| collectormintemp | minimum collector temperature | uint8 (>=0<=254) | C | true | DEVICE_DATA | 37 | 1 | 1 |
| energylasthour | energy last hour | uint24 (>=0<=1677721) | Wh | false | DEVICE_DATA | 13 | 2 | 1/10 |
| energytoday | total energy today | uint24 (>=0<=16777214) | Wh | false | DEVICE_DATA | 38 | 2 | 1 |
| energytotal | total energy | uint24 (>=0<=1677721) | kWh | false | DEVICE_DATA | 40 | 2 | 1/10 |
| pump2worktime | pump 2 working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 42 | 2 | 1 |
| m1worktime | differential control working time | time (>=0<=16777214) | minutes | false | DEVICE_DATA | 44 | 2 | 1 |
| heattransfersystem | heattransfer system | boolean |   | true | DEVICE_DATA | 46 | 1 | 1 |
| externalcyl | external cylinder | boolean |   | true | DEVICE_DATA | 47 | 1 | 1 |
| thermaldisinfect | thermal disinfection | boolean |   | true | DEVICE_DATA | 48 | 1 | 1 |
| heatmetering | heatmetering | boolean |   | true | DEVICE_DATA | 49 | 1 | 1 |
| activated | activated | boolean |   | true | DEVICE_DATA | 50 | 1 | 1 |
| solarpumpmode | solar pump mode | enum [constant\|pwm\|analog] |   | true | DEVICE_DATA | 51 | 1 | 1 |
| solarpumpkick | solar pump kick | boolean |   | true | DEVICE_DATA | 52 | 1 | 1 |
| plainwatermode | plain water mode | boolean |   | true | DEVICE_DATA | 53 | 1 | 1 |
| doublematchflow | doublematchflow | boolean |   | true | DEVICE_DATA | 54 | 1 | 1 |
| pump2minmod | minimum pump 2 modulation | uint8 (>=0<=100) | % | true | DEVICE_DATA | 55 | 1 | 1 |
| turnondiff2 | pump 2 turn on difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 56 | 1 | 1/10 |
| turnoffdiff2 | pump 2 turn off difference | uint8 (>=0<=25) | C | true | DEVICE_DATA | 57 | 1 | 1/10 |
| pump2kick | pump kick 2 | boolean |   | true | DEVICE_DATA | 58 | 1 | 1 |
| climatezone | climate zone | uint8 (>=0<=254) |   | true | DEVICE_DATA | 59 | 1 | 1 |
| collector1area | collector 1 area | uint16 (>=0<=3199) | m² | true | DEVICE_DATA | 60 | 1 | 1/10 |
| collector1type | collector 1 type | enum [flat\|vacuum] |   | true | DEVICE_DATA | 61 | 1 | 1 |
| collector2area | collector 2 area | uint16 (>=0<=3199) | m² | true | DEVICE_DATA | 62 | 1 | 1/10 |
| collector2type | collector 2 type | enum [flat\|vacuum] |   | true | DEVICE_DATA | 63 | 1 | 1 |
| cylpriority | cylinder priority | enum [cyl 1\|cyl 2] |   | true | DEVICE_DATA | 64 | 1 | 1 |
| heatcntflowtemp | heat counter flow temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 65 | 1 | 1/10 |
| heatcntrettemp | heat counter return temperature | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 66 | 1 | 1/10 |
| heatcnt | heat counter impulses | uint8 (>=0<=254) |   | false | DEVICE_DATA | 67 | 1 | 1 |
| swapflowtemp | swap flow temperature (TS14) | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 68 | 1 | 1/10 |
| swaprettemp | swap return temperature (TS15) | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 69 | 1 | 1/10 |
| heatassiston | heat assistance on | int8 (>=-12<=12) | K | true | DEVICE_DATA | 70 | 1 | 1/10 |
| heatassistoff | heat assistance off | int8 (>=-12<=12) | K | true | DEVICE_DATA | 71 | 1 | 1/10 |

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
| dhw.hybriddhw | hybrid DHW | enum [eco\|high comfort] |   | true | DHW | 0 | 1 | 1 |
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
| dhw.nrg | energy | uint24 (>=0<=167772) | kWh | false | DHW | 1 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 32 | 2 | 1/100 |
| metertotal | meter total | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 34 | 2 | 1/100 |
| metercomp | meter compressor | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 36 | 2 | 1/100 |
| metereheat | meter e-heater | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 38 | 2 | 1/100 |
| meterheat | meter heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 40 | 2 | 1/100 |
| dhw.meter | meter | uint24 (>=0<=167772) | kWh | false | DHW | 3 | 2 | 1/100 |
| heatingstarts | heating control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 42 | 2 | 1 |
| dhw.startshp | starts hp | uint24 (>=0<=16777214) |   | false | DHW | 5 | 2 | 1 |
| fuelheat | fuel consumption heating | uint32 (>=0<=1677721) | kWh | false | DEVICE_DATA | 44 | 2 | 1/10 |
| dhw.fueldhw | fuel consumption | uint32 (>=0<=1677721) | kWh | false | DHW | 7 | 2 | 1/10 |
| elheat | el. consumption heating | uint32 (>=0<=1677721) | kWh | false | DEVICE_DATA | 46 | 2 | 1/10 |
| dhw.eldhw | el. consumption | uint32 (>=0<=1677721) | kWh | false | DHW | 9 | 2 | 1/10 |
| elgenheat | el. generation heating | uint32 (>=0<=1677721) | kWh | false | DEVICE_DATA | 48 | 2 | 1/10 |
| dhw.elgendhw | el generation | uint32 (>=0<=1677721) | kWh | false | DHW | 11 | 2 | 1/10 |

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
| dhw.hybriddhw | hybrid DHW | enum [eco\|high comfort] |   | true | DHW | 0 | 1 | 1 |
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
| dhw.nrg | energy | uint24 (>=0<=167772) | kWh | false | DHW | 1 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 32 | 2 | 1/100 |
| metertotal | meter total | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 34 | 2 | 1/100 |
| metercomp | meter compressor | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 36 | 2 | 1/100 |
| metereheat | meter e-heater | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 38 | 2 | 1/100 |
| meterheat | meter heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 40 | 2 | 1/100 |
| dhw.meter | meter | uint24 (>=0<=167772) | kWh | false | DHW | 3 | 2 | 1/100 |
| heatingstarts | heating control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 42 | 2 | 1 |
| dhw.startshp | starts hp | uint24 (>=0<=16777214) |   | false | DHW | 5 | 2 | 1 |
| fuelheat | fuel consumption heating | uint32 (>=0<=1677721) | kWh | false | DEVICE_DATA | 44 | 2 | 1/10 |
| dhw.fueldhw | fuel consumption | uint32 (>=0<=1677721) | kWh | false | DHW | 7 | 2 | 1/10 |
| elheat | el. consumption heating | uint32 (>=0<=1677721) | kWh | false | DEVICE_DATA | 46 | 2 | 1/10 |
| dhw.eldhw | el. consumption | uint32 (>=0<=1677721) | kWh | false | DHW | 9 | 2 | 1/10 |
| elgenheat | el. generation heating | uint32 (>=0<=1677721) | kWh | false | DEVICE_DATA | 48 | 2 | 1/10 |
| dhw.elgendhw | el generation | uint32 (>=0<=1677721) | kWh | false | DHW | 11 | 2 | 1/10 |

### CSH5800iG
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
| dhw.hybriddhw | hybrid DHW | enum [eco\|high comfort] |   | true | DHW | 0 | 1 | 1 |
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
| dhw.nrg | energy | uint24 (>=0<=167772) | kWh | false | DHW | 1 | 2 | 1/100 |
| nrgheat | energy heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 32 | 2 | 1/100 |
| metertotal | meter total | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 34 | 2 | 1/100 |
| metercomp | meter compressor | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 36 | 2 | 1/100 |
| metereheat | meter e-heater | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 38 | 2 | 1/100 |
| meterheat | meter heating | uint24 (>=0<=167772) | kWh | false | DEVICE_DATA | 40 | 2 | 1/100 |
| dhw.meter | meter | uint24 (>=0<=167772) | kWh | false | DHW | 3 | 2 | 1/100 |
| heatingstarts | heating control starts | uint24 (>=0<=16777214) |   | false | DEVICE_DATA | 42 | 2 | 1 |
| dhw.startshp | starts hp | uint24 (>=0<=16777214) |   | false | DHW | 5 | 2 | 1 |
| fuelheat | fuel consumption heating | uint32 (>=0<=1677721) | kWh | false | DEVICE_DATA | 44 | 2 | 1/10 |
| dhw.fueldhw | fuel consumption | uint32 (>=0<=1677721) | kWh | false | DHW | 7 | 2 | 1/10 |
| elheat | el. consumption heating | uint32 (>=0<=1677721) | kWh | false | DEVICE_DATA | 46 | 2 | 1/10 |
| dhw.eldhw | el. consumption | uint32 (>=0<=1677721) | kWh | false | DHW | 9 | 2 | 1/10 |
| elgenheat | el. generation heating | uint32 (>=0<=1677721) | kWh | false | DEVICE_DATA | 48 | 2 | 1/10 |
| dhw.elgendhw | el generation | uint32 (>=0<=1677721) | kWh | false | DHW | 11 | 2 | 1/10 |

## Devices of type *switch*
### WM10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| activated | activated | boolean |   | false | DEVICE_DATA | 0 | 1 | 1 |
| flowtemphc | flow temperature (TC1) | uint16 (>=0<=3199) | C | false | DEVICE_DATA | 1 | 1 | 1/10 |
| status | status | int8 (>=-126<=126) |   | false | DEVICE_DATA | 2 | 1 | 1 |

## Devices of type *controller*
### Rego 3000
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| datetime | date/time | string |   | false | DEVICE_DATA | 0 | 13 | 1 |

## Devices of type *connect*
### MX400
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| datetime | date/time | string |   | false | DEVICE_DATA | 0 | 13 | 1 |
| outdoortemp | outside temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | 13 | 1 | 1/10 |
| src1.currtemp | current room temperature | int16 (>=-3199<=3199) | C | false | SRC | 0 | 1 | 1/10 |
| src1.airhumidity | relative air humidity | int8 (>=-100<=100) | % | false | SRC | 1 | 1 | 1 |
| src1.dewtemperature | dew point temperature | int16 (>=-3199<=3199) | C | false | SRC | 2 | 1 | 1/10 |
| src1.seltemp | selected room temperature | uint8 (>=5<=30) | C | true | SRC | 3 | 1 | 1/2 |
| src1.mode | operating mode | enum [off\|manual\|auto] |   | true | SRC | 4 | 1 | 1 |
| src1.name | name | string |   | true | SRC | 5 | 26 | 1 |
| src1.childlock | child lock | boolean |   | true | SRC | 31 | 1 | 1 |
| src1.icon | icon | enum [none\|mdi:chef-hat\|mdi:sofa-single-outline\|mdi:bowl-mix-outline\|mdi:bed-single-outline\|mdi:bed-double-outline\|mdi:teddy-bear\|mdi:shower\|mdi:laptop\|mdi:door\|mdi:palette-outline\|mdi:washing-machine\|mdi:bookshelf] |   | true | SRC | 32 | 1 | 1 |

## Devices of type *alert*
### EM10
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| setflowtemp | set flow temperature | uint8 (>=0<=254) | C | false | DEVICE_DATA | 0 | 1 | 1 |
| setburnpow | burner set power | uint8 (>=0<=100) | % | false | DEVICE_DATA | 1 | 1 | 1 |

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

## Devices of type *heatsource*
### AM200
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| ahs1.sysflowtemp | system flow temperature | int16 (>=-3199<=3199) | C | false | AHS | 0 | 1 | 1/10 |
| ahs1.sysrettemp | system return temperature | int16 (>=-3199<=3199) | C | false | AHS | 1 | 1 | 1/10 |
| ahs1.altflowtemp | alternative hs flow temperature | int16 (>=-3199<=3199) | C | false | AHS | 2 | 1 | 1/10 |
| ahs1.altrettemp | alternative hs return temperature | int16 (>=-3199<=3199) | C | false | AHS | 3 | 1 | 1/10 |
| ahs1.cyltoptemp | cylinder top temperature | int16 (>=-3199<=3199) | C | false | AHS | 4 | 1 | 1/10 |
| ahs1.cylcentertemp | cylinder center temperature | int16 (>=-3199<=3199) | C | false | AHS | 5 | 1 | 1/10 |
| ahs1.cylbottomtemp | cylinder bottom temperature | int16 (>=-3199<=3199) | C | false | AHS | 6 | 1 | 1/10 |
| ahs1.fluegastemp | flue gas temperature | int16 (>=-3199<=3199) | C | false | AHS | 7 | 1 | 1/10 |
| ahs1.valvebuffer | buffer valve | uint8 (>=0<=100) | % | false | AHS | 8 | 1 | 1 |
| ahs1.valvereturn | return valve | uint8 (>=0<=100) | % | false | AHS | 9 | 1 | 1 |
| ahs1.apumpmod | alternative hs pump modulation | uint8 (>=0<=100) | % | false | AHS | 10 | 1 | 1 |
| ahs1.vr2config | vr2 configuration | enum [off\|bypass] |   | true | AHS | 11 | 1 | 1 |
| ahs1.ahsactivated | alternate heat source activation | boolean |   | true | AHS | 12 | 1 | 1 |
| ahs1.apumpconfig | primary pump config | boolean |   | true | AHS | 13 | 1 | 1 |
| ahs1.apumpsignal | output for pr1 pump | enum [off\|pwm\|pwm inverse] |   | true | AHS | 14 | 1 | 1 |
| ahs1.apumpmin | min output pump pr1 | uint8 (>=12<=50) | % | true | AHS | 15 | 1 | 1 |
| ahs1.temprise | ahs return temp rise | boolean |   | true | AHS | 16 | 1 | 1 |
| ahs1.setreturntemp | set temp return | uint8 (>=40<=75) | C | true | AHS | 17 | 1 | 1 |
| ahs1.mixruntime | mixer run time | uint16 (>=0<=600) | seconds | true | AHS | 18 | 1 | 1 |
| ahs1.setflowtemp | set flow temperature | uint8 (>=40<=75) | C | true | AHS | 19 | 1 | 1 |
| ahs1.bufbypass | buffer bypass config | enum [no\|mixer\|valve] |   | true | AHS | 20 | 1 | 1 |
| ahs1.bufmixruntime | bypass mixer run time | uint16 (>=0<=600) | seconds | true | AHS | 21 | 1 | 1 |
| ahs1.bufconfig | dhw buffer config | enum [off\|monovalent\|bivalent] |   | true | AHS | 22 | 1 | 1 |
| ahs1.blockmode | config htg. blocking mode | enum [off\|auto\|blocking] |   | true | AHS | 23 | 1 | 1 |
| ahs1.blockterm | config of block terminal | enum [n_o\|n_c] |   | true | AHS | 24 | 1 | 1 |
| ahs1.blockhyst | hyst. for boiler block | int8 (>=0<=50) | C | true | AHS | 25 | 1 | 1 |
| ahs1.releasewait | boiler release wait time | uint8 (>=0<=240) | minutes | true | AHS | 26 | 1 | 1 |
| ahs1.burner | burner | boolean |   | false | AHS | 27 | 1 | 1 |
| ahs1.apump | alternative hs pump | boolean |   | false | AHS | 28 | 1 | 1 |
| ahs1.heatrequest | heat request | uint8 (>=0<=100) | % | false | AHS | 29 | 1 | 1 |
| ahs1.blockremain | remaining blocktime | uint8 (>=0<=254) | minutes | false | AHS | 30 | 1 | 1 |
| ahs1.blockremaindhw | remaining blocktime dhw | uint8 (>=0<=254) | minutes | false | AHS | 31 | 1 | 1 |

## Devices of type *ventilation*
### HRV176, HRV156, 5000c, MV200
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
| bypass | bypass | boolean |   | true | DEVICE_DATA | 9 | 1 | 1 |

## Devices of type *water*
### IPM
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 0 | 1 | 1 |
| dhw.temp | current temperature | uint16 (>=0<=3199) | C | false | DHW | 1 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 2 | 1 | 1/10 |
| dhw.hydrTemp | hydraulic header temperature | uint16 (>=0<=3199) | C | false | DHW | 3 | 1 | 1/10 |
| dhw.pump | pump | boolean |   | false | DHW | 4 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=254) | C | true | DHW | 5 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 6 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 7 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=0<=254) | C | true | DHW | 8 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 9 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 10 | 1 | 1 |

### IPM2
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 0 | 1 | 1 |
| dhw.temp | current temperature | uint16 (>=0<=3199) | C | false | DHW | 1 | 1 | 1/10 |
| dhw.curtemp2 | current extern temperature | uint16 (>=0<=3199) | C | false | DHW | 2 | 1 | 1/10 |
| dhw.hydrTemp | hydraulic header temperature | uint16 (>=0<=3199) | C | false | DHW | 3 | 1 | 1/10 |
| dhw.pump | pump | boolean |   | false | DHW | 4 | 1 | 1 |
| dhw.flowtempoffset | flow temperature offset | uint8 (>=0<=254) | C | true | DHW | 5 | 1 | 1 |
| dhw.hyston | hysteresis on temperature | int8 (>=-126<=126) | C | true | DHW | 6 | 1 | 1 |
| dhw.hystoff | hysteresis off temperature | int8 (>=-126<=126) | C | true | DHW | 7 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=0<=254) | C | true | DHW | 8 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 9 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 10 | 1 | 1 |

### MM100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| dhw.temp | current temperature | uint16 (>=0<=3199) | C | false | DHW | 1 | 1 | 1/10 |
| dhw.tempstatus | temperature switch in assigned dhw (MC1) | int8 (>=-126<=126) |   | false | DHW | 11 | 1 | 1 |
| dhw.pump | pump | boolean |   | false | DHW | 4 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=254) | C | true | DHW | 12 | 1 | 1 |
| dhw.difftemp | start differential temperature | int8 (>=-126<=126) | C | true | DHW | 13 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=0<=254) | C | true | DHW | 8 | 1 | 1 |
| dhw.redtemp | reduced temperature | uint8 (>=0<=254) | C | true | DHW | 14 | 1 | 1 |
| dhw.requiredtemp | required temperature | uint8 (>=0<=254) | C | true | DHW | 15 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 9 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 10 | 1 | 1 |

### MM200
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| dhw.temp | current temperature | uint16 (>=0<=3199) | C | false | DHW | 1 | 1 | 1/10 |
| dhw.tempstatus | temperature switch in assigned dhw (MC1) | int8 (>=-126<=126) |   | false | DHW | 11 | 1 | 1 |
| dhw.pump | pump | boolean |   | false | DHW | 4 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=254) | C | true | DHW | 12 | 1 | 1 |
| dhw.difftemp | start differential temperature | int8 (>=-126<=126) | C | true | DHW | 13 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=0<=254) | C | true | DHW | 8 | 1 | 1 |
| dhw.redtemp | reduced temperature | uint8 (>=0<=254) | C | true | DHW | 14 | 1 | 1 |
| dhw.requiredtemp | required temperature | uint8 (>=0<=254) | C | true | DHW | 15 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 9 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|on\|auto\|own prog] |   | true | DHW | 10 | 1 | 1 |

### SM100, MS100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| dhw.temp | current temperature | uint16 (>=0<=3199) | C | false | DHW | 1 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 16 | 1 | 1/10 |
| dhw.coldtemp | cold water | uint16 (>=0<=3199) | C | false | DHW | 17 | 1 | 1/10 |
| dhw.temp5 | temperature 5 | uint16 (>=0<=3199) | C | false | DHW | 18 | 1 | 1/10 |
| dhw.rettemp | return temperature | uint16 (>=0<=3199) | C | false | DHW | 19 | 1 | 1/10 |
| dhw.pump | pump | boolean |   | false | DHW | 4 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=254) | C | true | DHW | 12 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 0 | 1 | 1 |
| dhw.redtemp | reduced temperature | uint8 (>=0<=254) | C | true | DHW | 14 | 1 | 1 |
| dhw.hottemp | extra hot temperature | uint8 (>=0<=254) | C | true | DHW | 20 | 1 | 1 |
| dhw.dailytemp | daily temperature | uint8 (>=0<=254) | C | true | DHW | 21 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=0<=254) | C | true | DHW | 8 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 9 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 10 | 1 | 1 |
| dhw.circtc | circulation time controled | boolean |   | true | DHW | 22 | 1 | 1 |
| dhw.keepwarm | keep warm | boolean |   | true | DHW | 23 | 1 | 1 |
| dhw.status2 | status 2 | enum [\|\|\|no heat\|\|\|heat request\|\|disinfecting\|hold] |   | false | DHW | 24 | 1 | 1 |
| dhw.pumpmod | pump modulation | uint8 (>=0<=100) | % | false | DHW | 25 | 1 | 1 |
| dhw.flow | flow rate | uint16 (>=0<=3199) | l/min | false | DHW | 26 | 1 | 1/10 |
| dhw.valvereturn | return valve | uint8 (>=0<=100) | % | true | DHW | 27 | 1 | 1 |
| dhw.deltatret | temp. diff. return valve | uint8 (>=0<=254) | K | true | DHW | 28 | 1 | 1 |
| dhw.errordisp | error display | enum [off\|normal\|inverted] |   | true | DHW | 29 | 1 | 1 |

### SM200, MS200
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| dhw.temp | current temperature | uint16 (>=0<=3199) | C | false | DHW | 1 | 1 | 1/10 |
| dhw.storagetemp1 | storage intern temperature | uint16 (>=0<=3199) | C | false | DHW | 16 | 1 | 1/10 |
| dhw.coldtemp | cold water | uint16 (>=0<=3199) | C | false | DHW | 17 | 1 | 1/10 |
| dhw.temp5 | temperature 5 | uint16 (>=0<=3199) | C | false | DHW | 18 | 1 | 1/10 |
| dhw.rettemp | return temperature | uint16 (>=0<=3199) | C | false | DHW | 19 | 1 | 1/10 |
| dhw.pump | pump | boolean |   | false | DHW | 4 | 1 | 1 |
| dhw.maxtemp | maximum temperature | uint8 (>=0<=254) | C | true | DHW | 12 | 1 | 1 |
| dhw.seltemp | selected temperature | uint8 (>=0<=254) | C | true | DHW | 0 | 1 | 1 |
| dhw.redtemp | reduced temperature | uint8 (>=0<=254) | C | true | DHW | 14 | 1 | 1 |
| dhw.hottemp | extra hot temperature | uint8 (>=0<=254) | C | true | DHW | 20 | 1 | 1 |
| dhw.dailytemp | daily temperature | uint8 (>=0<=254) | C | true | DHW | 21 | 1 | 1 |
| dhw.disinfectiontemp | disinfection temperature | uint8 (>=0<=254) | C | true | DHW | 8 | 1 | 1 |
| dhw.circ | circulation active | boolean |   | true | DHW | 9 | 1 | 1 |
| dhw.circmode | circulation pump mode | enum [off\|1x3min\|2x3min\|3x3min\|4x3min\|5x3min\|6x3min\|continuous] |   | true | DHW | 10 | 1 | 1 |
| dhw.circtc | circulation time controled | boolean |   | true | DHW | 22 | 1 | 1 |
| dhw.keepwarm | keep warm | boolean |   | true | DHW | 23 | 1 | 1 |
| dhw.status2 | status 2 | enum [\|\|\|no heat\|\|\|heat request\|\|disinfecting\|hold] |   | false | DHW | 24 | 1 | 1 |
| dhw.pumpmod | pump modulation | uint8 (>=0<=100) | % | false | DHW | 25 | 1 | 1 |
| dhw.flow | flow rate | uint16 (>=0<=3199) | l/min | false | DHW | 26 | 1 | 1/10 |
| dhw.valvereturn | return valve | uint8 (>=0<=100) | % | true | DHW | 27 | 1 | 1 |
| dhw.deltatret | temp. diff. return valve | uint8 (>=0<=254) | K | true | DHW | 28 | 1 | 1 |
| dhw.errordisp | error display | enum [off\|normal\|inverted] |   | true | DHW | 29 | 1 | 1 |

## Devices of type *pool*
### MP100
| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |
|-|-|-|-|-|-|-|-|-|
| pooltemp | pool temperature | int16 (>=-3199<=3199) | C | false | DEVICE_DATA | -1 | 1 | 1/10 |
| poolshuntstatus | pool shunt status opening/closing | enum [stopped\|opening\|closing\|open\|close] |   | false | DEVICE_DATA | -1 | 1 | 1 |
| poolshunt | pool shunt open/close (0% = pool / 100% = heat) | uint8 (>=0<=100) | % | false | DEVICE_DATA | -1 | 1 | 1 |

