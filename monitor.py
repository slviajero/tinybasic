#!/usr/bin/python3
#
# $Id: monitor.py,v 1.6 2021/08/20 19:59:44 stefan Exp stefan $
#
# Arduino Monitor V0.1 for Stefan's tinybasic. 
#
# This is a very basic serial monitor with a few special features.
# it connects to the Arduino serial port and interacts with
# Stefan's Tinybasic. 
#
# Special commands
# <cntrl> L loads a program into the Arduino - literally typing it in
# <cntrl> S saves a program from the Arduino - collects the output of LIST 
#         to save type <cntrl S>, enter the file name and then type list
#         end save mode with another <cntrl S>, then hand edit the output 
#         file to remove the "list" and "Ready"
# <cntrl> D ends the monitor 
# 
# Set the port here 
#
#port = '/dev/cu.usbserial-1420'
port = '/dev/cu.usbmodem14201'
#port = '/dev/cu.usbserial-0001'
#port = '/dev/ttyUSB0'

import sys
import serial
import threading 
from time import sleep
import readchar

# timeout is needed to terminate properly

ser = serial.Serial(port, 9600, timeout = 0.3, xonxoff = True)

#
# The upload function
#

def sendline(l):
	sleep(0.1)
	for ch in l:
		sleep(0.01)
		chp = ch.encode('utf-8')
		ser.write(chp)

cont = True;
saveflag = False;

def loadfile():
	fn = input("Loading from file: ")
	f = open(fn, "r")
	for line in f:
		sendline(line)

#
# The reader thread 
#
def readfunction():
	while cont:
		ch = ser.read()
		chp = ch.decode('utf-8')
		if ( chp == "\n"):
			sys.stdout.write("\r");
		if ( chp == "\r"):
			sys.stdout.write("\r");
			sys.stdout.write("\n");
		sys.stdout.write(chp)
		sys.stdout.flush()
		if (saveflag):
			if (chp != "\r"):
				f2.write(chp)

reader = threading.Thread(target=readfunction)
reader.start()

# 
#  The write loop
#
while cont:
	ch = readchar.readchar()
	chp = ch.encode('utf-8')
	if (chp == b'\x04'):
		cont = False
	elif (chp == b'\x0c'):
		loadfile()	
	elif (chp == b'\x13'):
		if (not saveflag):
			saveflag = True
			print("Save on")
			fn = input("Save to file: ")
			f2 = open(fn, "w")
		else:
			saveflag = False
			print("Save off")
			f2.close()
	else:
		ser.write(chp)

reader.join()
