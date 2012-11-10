#!/usr/bin/python

import sys
import serial

def sendString(ser, string):
    ser.write("####%s%s"%(chr(len(string)), string))
    ok = ser.readline()
    if ok.strip() == 'OK':
        return True
    return False

if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyUSB0', 9600)
    print ser.readline() # wait "HELLO"

    data = sys.argv[1].strip()
    print "Printing %s..."%data
    if not sendString(ser, data):
        print "!! Somthing wrong !!"
    ser.close()

