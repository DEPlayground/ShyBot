#!/bin/sh
# Lara Maia <dev@lara.click> 2016

arduino-ide --verbose --upload --board arduino:avr:uno --port /dev/ttyACM0 $PWD/shybot.ino
