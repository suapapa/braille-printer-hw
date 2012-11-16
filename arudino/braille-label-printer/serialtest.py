#!/usr/bin/python

import sys
import serial
import time

def sendString(ser, string):
    ser.write("####%s%s"%(chr(len(string)), string))
    ok = ser.read(2)
    if ok.strip() == 'OK':
        return True
    return False

if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyUSB0', 9600)

    data = sys.argv[1].strip()
    print "Printing %s..."%data
    if not sendString(ser, data):
        print "!! Somthing wrong !!"
    ser.close()

