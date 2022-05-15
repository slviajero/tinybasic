10 REM "MultiSerial from Arduino examples"
20 REM "BASIC needs to be compiled with ARDUINOPRT"
30 REM "and HASIOT for this program to work"
100 REM "setup() put your setup code here, to run once:"
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 IF AVAIL(1) GET C : PUT &4, C
230 IF AVAIL(4) GET &4, C : PUT C
240 NEXT
