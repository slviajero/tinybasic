100 REM "Test Gosub stack error handling"
110 PRINT "Should produce a GOSUB error in 600"
200 GOSUB 300
210 END 
300 GOSUB 400
310 RETURN 
400 GOSUB 500
410 RETURN 
500 GOSUB 600
510 RETURN 
600 GOSUB 700
610 RETURN 
700 PRINT "This should not show"
800 END