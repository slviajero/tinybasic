10 REM "Microsoft Mode"
20 SET 22, "Microsoft"
1000 REM "Energierechner - Berechnet die Rendite eine Solaranlage"
1010 REM "Einspeisevergütung 10kWp und 40 kWp"
1020 DATA 7.94, 6.88
1030 REM "Laufzeit der Förderung in Jahren""
1040 DATA 20
1050 REM "Wartungskosten über 20 Jahre in Prozent"
1060 DATA 15
1070 REM "Inflation und Preissteigerung Stromkosten"
1080 DATA 4
1090 REM "Restwert der Anlage nach Laufzeit"
1100 DATA 80
2000 INPUT "kWp Leitung der Anlage: ",KW
2010 INPUT "Erwartete Jahresleistung (kWh): ",EA
2020 INPUT "Eigenverbrauch pro Jahr (kWh): ",VE
2030 INPUT "Strompreis (in Cent): ", PS
2040 INPUT "Preis der Anlage: ", PA
3000 PRINT "*** Grunddaten ***"
3010 READ V1, V2, LZ, WK
3015 DIM PI(LZ), VI(LZ)
3020 PRINT "<10kWp",V1,">10 kWp",V2
3030 IF KW>40 THEN PRINT "Berechnung nur für <40kWp": END
3040 IF KW>10 THEN V=(V1*10+V2*(KW-10))/KW ELSE V=V1
3050 PRINT "Verguetung für Gesamtleistung:",V
3060 PRINT
3100 PRINT "*** Kosten der Anlage und Strompreis ***"
3110 PRINT "****** Simulation ohne Inflation *******"
3120 GK=PA*(1+WK/100)
3125 PRINT "Wartungskosten:";TAB(34);#8;GK-PA
3130 PRINT "Gesamtkosten inkl Wartung:";TAB(34);#8;GK
3160 PRINT "Kosten Stromversorger pro Jahr:";TAB(34);#8;PS/100*VE
3170 PRINT "Kosten Stromversorger Laufzeit:";TAB(34);#8;PS/100*VE*LZ
3200 PRINT "*** Einspeisevergütung ***"
3210 PRINT "Einspeisung pro Jahr:";TAB(32);#8;EA-VE 
3220 VJ=INT((EA-VE)*V/100)
3230 PRINT "Verguetung pro Jahr:";TAB(32);#8;VJ
3240 VG=VJ*LZ
3250 PRINT "Verguetung ueber Laufzeit: ";TAB(32);#8;VG
3260 PRINT "Eingesparte Stromkosten: ";TAB(32);#8;PS/100*VE*LZ
3270 PRINT "Ertraege und Einsparungen:";TAB(32);#8;VG+PS/100*VE*LZ
3380 PRINT "Gesamkosten:";TAB(32);#8;GK
3390 PRINT "Einsparung:";TAB(32);#8;VG+PS/100*VE*LZ-GK
3393 R0=POW((VG+PS/100*VE*LZ)/GK,1/LZ)-1
3397 PRINT "Rendite ohne Inflation:", R0*100
3400 PRINT "****** Simulation mit Inflation *******"
3410 READ SI
3420 PRINT "Preissteigerung Strom", SI, "Prozent"
3430 PI(1)=PS
3440 FOR J=2 TO LZ: PI(J)=PI(J-1)*(1+SI/100): NEXT
3450 VI(1)=PS/100*VE
3460 FOR J=2 TO LZ: VI(J)=VI(J-1)*(1+SI/100): NEXT
3470 PRINT "Jahr Preis Kosten"
3480 FOR J=1 TO LZ
3490 PRINT #6, J, INT(PI(J)), INT(VI(J))
3500 NEXT
3510 GI=0: FOR J=1 TO LZ: GI=VI(J)+GI: NEXT: GI=INT(GI)
3520 PRINT "Stromkosten mit Inflation:";TAB(32);#8; GI
3550 PRINT "Verguetung ueber Laufzeit: ";TAB(32);#8; VG
3560 PRINT "Eingesparte Stromkosten: ";TAB(32);#8; GI 
3570 PRINT "Ertraege und Einsparungen:";TAB(32);#8; VG+GI
3580 PRINT "Gesamkosten:";TAB(32);#8;GK
3590 PRINT "Einsparung:";TAB(32);#8;VG+GI-GK
3600 RE=POW((VG+GI)/GK,1/LZ)-1
3610 PRINT "Rendite:",RE*100
3700 PRINT "****** Simulation mit Restwert *******"
3710 READ RW
3715 PRINT "Restwert der Anlage in Prozent:";TAB(32),#8;RW
3720 GR=PA*RW/100
3730 PRINT "Restwert der Anlage:";TAB(32),#8;GR
3740 RR=POW((VG+GI)/(GK-GR),1/LZ)-1
3750 PRINT "Rendite nach Restwert:",RR*100



