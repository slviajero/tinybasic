# Various test programs and benchmarks for the BASIC interpreter

## Programs 

- creative.bas: Creative computing benchmark
- rfbench[1-8].bas: Rugg/Feldmann benchmarks 
- rfbench.txt: Results for Arduino UNO (integer), Arduino Mega, ESP8266, and ESP32 (float)
- timetest.bas: calibration benchmark to show peformance of low level features
- string.bas: elementary string test
- testfor.bas: nesting of for loops
- array2d.bas: addressing of arrays

## BASIC benchmarks 

The benchmarks show that BASIC is about as fast as the Apple Integer BASIC, scaled by the clock frequency of the CPU. This means that an 16 MHz Arduino UNO with integer arithmentic is 16 times faster than an 6502 machine with 1 MHz running Apple Integer BASIC. This is not surpising. As the BASIC interpreter is a reimplementation and extension of Apple Integer BASIC concepts it is about as fast as its legendary predecessor.

Floating point results need to be compared with Microsoft BASIC variants like Applesoft BASIC. Scaled with the clock frequency, this BASIC interpreter is 10-30% faster than MS BASIC depending on the bechmark. Tokenisation and the absence of dynamic memory mechanims explain this.

The benchmarks also show how fast microcontrollers are as compared to the old times 8bit computers. An Arduino Mega runs rfbench7.bas in 2.5 seconds. An Apple 2 with Applesoft BASIC needed 44.8 seconds. The same benchmarks needs 0.5s on an ESP32. 

Networking functions in BASIC make it 30-40% slower as shown in the ESP32 results. This due to the byield() function and the client loop functions in it. 

## Function tests

The indexing of strings, arrays and the handling of FOR loops are the more complex parts of the code. The test programs array2d.bas, string.bas, and testfor.bas are quick and dirty function tests. 


## Original files

See 

- https://en.wikipedia.org/wiki/Creative_Computing_Benchmark

- https://en.wikipedia.org/wiki/Rugg/Feldman_benchmarks

- https://archive.org/details/kilobaudmagazine-1977-06/page/n67/mode/2up


for further information
