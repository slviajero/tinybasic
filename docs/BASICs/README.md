# BASICs

This folder contains examples and documentation for historical BASICs. I used some of them to program my own BASIC interpreter. Some I just conserve here for the fun of it.

## Licenses, Copyright and Intellectual Property

Most of these documents were once copyrighted information. As I found all of this as free downloads on the internet, I did not research the licence status. This is a time machine back into the beginning of computing. Most of these documents are 40 years old and older. I feel that having this things freely available will not violate anyones rights or interests. If you find your material here and want it removed, please contact me. I will delete the files immediately. 

## The books

### General Books on BASIC

**David Ahl's 101 BASIC Computer Games, March 1975 edition**. I used these programs to test my interpreter. It can run all of them. [101_BASIC_Computer_Games_Mar75.pdf](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/101_BASIC_Computer_Games_Mar75.pdf)

**Basic_Handbook_2nd_Edition_1981_CompuSoft_Publishing**. Compares many BASIC dialects. A great list of features and language styles, written by David Lien. [Basic_Handbook_2nd_Edition_1981_CompuSoft_Publishing-2](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/Basic_Handbook_2nd_Edition_1981_CompuSoft_Publishing-2.pdf)

### Palo Alto Tinybasic

**Palo Alto Tiny BASIC**. This is how I started. From this document the first interpreter was implemented from scratch. I kept compatibility to this language although the code now is far away from the Tinybasic language set. [TinyBASIC-2.0.pdf](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/TinyBASIC-2.0.pdf)

### Apple 1 and related stuff

**Apple 1 BASIC User Manual**. Steve Wozniaks draft manual. I used this to implement the second layer of BASIC commands. Tokenization and substring logic was taken from this. [apple1_basic_manual.pdf](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/apple1_basic_manual.pdf)

**The Apple 1 BASIC assembler source**. I never used this. Eric Smith disassembled the original ROM. [a1basic.asm](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/a1basic.asm)

### Cromemco and North Star BASIC

A few commercial BASICs seem seems to have inherited their core DNA from Apple 1 BASIC and from Palo Alto BASIC. The similarities are amazing. They didn't survive after Microsoft started to dominate the market.

**Cromenco BASIC**: [An Introduction to Structured BASIC for the Cromemco C-10](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/An%20Introduction%20to%20Structured%20BASIC%20for%20the%20Cromemco%20C-10.pdf)

**North Star BASIC**:[North_Star_BASIC_Version_6.pdf](North_Star_BASIC_Version_6.pdf)

**The early Radio Shack TRS80 BASIC**. A BASIC that was derived from Palo Alto BASIC and extended with graphics, a few commands and some I/O functions added. Only one array A() and two strings A\$ and B\$ which just can do input and printing. 
[Basic_Computer_Language_Its_Easier_Than_You_Think_1978_Radio_Shack.pdf](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/Basic_Computer_Language_Its_Easier_Than_You_Think_1978_Radio_Shack.pdf)

### Microsofts early BASICs

**Altair BASIC**. This BASIC started the home computer revolution on the first Altair computers. Remarkable software engineering. This indroduced RIGHT, LEFT, MID string operations. [AltairBasic_1275.pdf](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/AltairBasic_1275.pdf)

**Aim 65 BASIC**. Another variant of the early Microsoft BASIC. Build for the legendary AIM65. [aim65_basic.pdf](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/aim65_basic.pdf)

**Applesoft BASIC**. A Microsoft based BASIC interpreter for the Apple II from 1977. This is essentially the grandfather of the well known C64 interpreters. [Apple2AppleSoftBASICRef1977.pdf](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/Apple2AppleSoftBASICRef1977.pdf)

### More Apple Stuff

**Apple 1 Operations Manual**. This is how a computer manual looked like in the 1970s. A lot of hardware and assembler stuff. Note the original Apple logo on the cover. [apple.applei.1976.102646518.pd](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/apple.applei.1976.102646518.pdf)

**Apple 2 ROM Image documentation**. This is a collection of infos on the Apple II internal software. [Apple II Original ROM Info.pdf](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/Apple%20II%20Original%20ROM%20Info.pdf)

**DOS 3.2 Guide for Apple DOS**. This is how the Apple did disk access. [DOS_3.2_Instruction_and_Reference_Manual.pdf](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/DOS_3.2_Instruction_and_Reference_Manual.pdf)

### Newer BASIC dialects 

**Laser Color Computer BASIC**. A slightly extended MS BASIC with graphics commands. [laser_basic_manual.pdf](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/laser_basic_manual.pdf)

**Talent MSX BASIC**. An example for an MSX BASIC. In Spanish. [Talent DPC-200 (Manual de uso y MSX Basic).pdf](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/Talent%20DPC-200%20(Manual%20de%20uso%20y%20MSX%20Basic).pdf)

**Mallard BASIC**. This is a very powerful BASIC that has inherited a bit from Microsoft but also went its own way. [PCWSBAS.pdf](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/PCWSBAS.pdf)

**Enterprise BASIC**. Probably the most advanced BASIC interpreter for home computers. Written when home computers were at its peak. Never took off big time. [Enterprise_Programming_Guide.pdf](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/Enterprise_Programming_Guide.pdf)

**A new 6502 BASIC**. People still create BASIC interpreters for 6502. This is one example. [EhBASIC-manual.pdf](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/EhBASIC-manual.pdf)

### How it all started

**Mark II Timesharing and Dartmouth BASIC**. GE partnered with Dartmouth on the BASIC for the Mark II timesharing computer series. No string variables but a lot of maths.[711224A_BASIC_Language_Reference_Manual_Dec1968.pdf](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/711224A_BASIC_Language_Reference_Manual_Dec1968.pdf)

**DEC Basis Interpreters**. DEC ported BASIC interpreters to their PDP-10 and PDP-11 mini computers in the 60s. These languages were very similar to the Mark II Dartmouth BASICs. Remarkable features were matrix operations and (random) file access. These BASICs were mostly used for maths as powerful interactive calculators. They have no or very limited string functions. [DEC-10-LBLMA-A-D_BASIC_Conversational_Language_Manual_Mar74.pdf](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/DEC-10-LBLMA-A-D_BASIC_Conversational_Language_Manual_Mar74.pdf) [DEC-11-ORBPA-A-D_BASIC-PLUS_LangMan_Oct72.pdf](https://github.com/slviajero/tinybasic/blob/main/docs/BASICs/DEC-11-ORBPA-A-D_BASIC-PLUS_LangMan_Oct72.pdf)

