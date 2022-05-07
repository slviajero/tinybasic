10 REM "Blink from Arduino examples"
100 REM "setup() put your setup code here, to run once:"
110 PINM LED, 1
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 DWRITE LED, 1
230 DELAY 1000
240 DWRITE LED, 0
250 DELAY 1000
260 NEXT
