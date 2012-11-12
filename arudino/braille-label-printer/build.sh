#!/bin/bash

scons ARDUINO_BOARD=atmega168 ARDUINO_PORT=/dev/ttyUSB0 $1
