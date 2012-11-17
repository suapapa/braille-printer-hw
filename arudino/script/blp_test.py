#!/usr/bin/python

import sys
import serial
import time

if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyUSB0', 9600)

    data = sys.argv[1].strip()
    ser.write("####%s%s"%(chr(len(data)), data))
    ser.close()

