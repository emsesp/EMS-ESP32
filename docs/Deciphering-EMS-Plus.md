> [!NOTE]
> This page is a collection of findings to figure out how the EMS+ / EMS 2.0 protocol works.

***

```
sr = source ID
de = destination ID (read commands have the 8th bit set)
e+ = EMS flag (>0xF0)
of = offset
TYPE = Type ID
DATA = data block

if 3rd byte = FF

sr de FF of TYPE   DATA
-- -- -- -- ----   ----------------------------------------------------------------------------------   

48 10 FF 0A 01B9   28 (write 1 byte, offset 0A, type 01B9, value 28 = 20.0 deg)
48 10 FF 0A 01B9   29 (write 1 byte, offset 0A, type 01B9, value 29 = 20.5 deg)
48 10 FF 00 01B9   FF (write 1 byte, type 01B9, value FF)

48 90 FF 25 0101      (read byte 25 from 0101)

10 48 FF 25 01A8      (no data. an acknowledgment)

48 90 FF 1B 0201   A8 (why does a read have a value?)
10 48 FF 1B 01A8      (why no data. an acknowledgment?)

09 B0 FF 00 FF00   01 (type FF00?)
                                         
                                                                       
broadcast messages (where de = 00):

sr 00 FF of TYPE   DATA
-- -- -- -- ----   ----------------------------------------------------------------------------------
10 00 FF 00 01B9   FF
10 00 FF 12 01B9   00
10 00 FF 08 01A5   01 99
10 00 FF 0A 01A5   01
10 00 FF 19 01A5   06 04 00 00 00 00 FF 64 37 00 3C 01 FF 01
10 00 FF 00 01A5   80 00 01 30 28 00 30 28 01 54 03 03 01 01 54 02 A8 00 00 11 01 03 FF FF 00
10 00 FF 00 01A5   00 D7 21 00 00 00 00 30 01 84 01 01 03 01 84 01 F1 00 00 11 01 00 08 63 00
30 00 FF 00 0262   01 FB 01 9E 80 00 80 00 80 00 80 00 80 00 80 00 80 00 80 00 80 00 80 00 2B (SM100)
30 00 FF 00 0264   00 00 00 04 00 00 FF 00 00 1E 0C 20 64 00 00 00 00 E9 (SM100)
90 00 FF 00 006F   01 01 00 46 00 B9 (FR10)
10 00 FF 00 01A5   80 00 01 30 28 00 30 28 01 54 03 03 01 01 54 02 A8 00 00 11 01 03 FF FF 00 (RC310)
10 00 FF 00 01A5   00 D7 21 00 00 00 00 30 01 84 01 01 03 01 84 01 F1 00 00 11 01 00 08 63 00 (RC1010)
                                                                                                                                                                                        
30 09 FF 00 0001  (this doesn't look correct)

if 3rd byte = F9 or F7

sr de F9 00 no FF TYPE ??   DATA
-- -- -- -- -- -- ---- --   ----------------------------------------------------------------------------------   

10 48 F9 00    FF 016C 08   4F 00 00 00 02 00 00 00 02 00 03 00 03 00 03 00 02 (data, type 016C)
48 90 F9 00 11 FF 016D 08   (read 17 bytes, type 016D)
10 48 F9 00    FF 016D 08   4F 00 00 00 02 00 00 00 02 00 03 00 03 00 03 00 02

48 90 F9 00 11 FF 019E 00   (read 17 bytes, type 019E)
10 48 F9 00    FF 019E 00   4B 00 00 00 01 00 00 00 01 00 1B 00 06 00 1B 00 01
10 00 F7 00    FF 01B9 35   19 (broadcast, type 01B9, value 19)

                                                                                                
```