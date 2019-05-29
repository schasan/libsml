#!/usr/bin/python
import RPi.GPIO as GPIO
import time
import threading
import requests
import json

# Zaehler-Variable, global
Counter = 0

# Pinreferenz waehlen
GPIO.setmode(GPIO.BCM)

# GPIO 18 (Pin 12) als Input definieren und Pullup-Widerstand aktivieren
GPIO.setup(18, GPIO.IN, pull_up_down = GPIO.PUD_UP)

# Callback-Funktion
def Interrupt(channel):
  global Counter
  Counter += 1
  print(str(Counter))

# Interrupt-Event hinzufuegen, steigende Flanke
GPIO.add_event_detect(18, GPIO.RISING, callback = Interrupt)

# Endlosschleife, bis Strg-C gedrueckt wird
try:
  while True:
    time.sleep(3600)
except KeyboardInterrupt:
  GPIO.cleanup()
  print('\nBye')

