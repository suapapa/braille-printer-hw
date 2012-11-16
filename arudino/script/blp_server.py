#!/usr/bin/python
# -*- coding: utf-8 -*-
 
# blp_server.py - UDS server for Braille Lable Printer
#
# Copyright (C) 2012 Homin Lee <homin.lee@suapapa.net>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

import serial
import socket
import os
import sys
import time

try:
    serialPort = sys.argv[1]
except:
    serialPort = '/dev/ttyUSB0'
    print "will open default serial port, %s"%serialPort

ser = serial.Serial(serialPort, 9600)
sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)

UDSPath = "/tmp/blp_uds"
if os.path.exists(UDSPath):
    os.remove(UDSPath)
sock.bind(UDSPath)

print("BLP server start. UDS: %s"%UDSPath)
while(1):
    try:
        packet = sock.recv(1024)
        if packet == "quit":
            break;
        else:
            ser.write(packet)
        time.sleep(0.5)
    except:
        break

ser.close()    
os.unlink(UDSPath)

print("BLP server finished")
# vim: et sw=4 fenc=utf-8:

