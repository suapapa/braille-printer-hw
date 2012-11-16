#!/usr/bin/python
# -*- coding: utf-8 -*-
 
# blp_server_test.py - Simple test for BLP server
#
# Copyright (C) 2012 Homin Lee <homin.lee@suapapa.net>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

import socket
import sys
import os

sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
UDSPath = "/tmp/blp_uds"

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("usage: %s <message>"%sys.argv[0])
        exit(1)

    data = sys.argv[1]
    packet = "####%s%s"%(chr(len(data)), data)
    n = sock.sendto(packet, UDSPath)
    if (n != len(packet)):
       print("somthing wrong!")
       exit(1)

    exit(0)

# vim: et sw=4 fenc=utf-8:

