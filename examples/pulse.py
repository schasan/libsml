#!/usr/bin/python
import RPi.GPIO as GPIO
import time
import threading
import requests
import json
from auth import headers

# Zaehler-Variable, global
Counter = 0
Calm = 10
Tic = 0
url = 'https://192.168.5.201:8443/services/collector'
lasttime = time.time()

# Pinreferenz waehlen
GPIO.setmode(GPIO.BCM)

# GPIO 18 (Pin 12) als Input definieren und Pullup-Widerstand aktivieren
GPIO.setup(18, GPIO.IN, pull_up_down = GPIO.PUD_UP)

def logger(data):
  #print(json.dumps(data))
  res = requests.post(url, data=json.dumps({'event': data}), headers=headers(), verify=False)
  if res.status_code != requests.codes.ok:
    print(res.json())

# Callback-Funktion
def Interrupt(channel):
  global lasttime

  Counter += 1
  if (Counter % Calm == 0):
    now = time.time()
    delta = now - lasttime
    lasttime = now
    global Counter
    watt = 360.0/delta
    print('Counter: {:6} Now: {:10.2f} Delta: {:6.4f} Watt: {:10.4f}'.format(Counter, now, delta, watt*Calm))
    logentry = {
      'Counter': Counter,
      'Delta': delta,
      'Now': now,
      'Watt': watt*Calm
    }
    logging_thread = threading.Thread(target=logger, args=(logentry,))
    logging_thread.start()

# Interrupt-Event hinzufuegen, steigende Flanke
GPIO.add_event_detect(18, GPIO.RISING, callback = Interrupt, bouncetime = 250)

# Endlosschleife, bis Strg-C gedrueckt wird
try:
  while True:
    # nix Sinnvolles tun
    Tic = Tic + 1
    #print('Tic: {} Counter: {}'.format(Tic, Counter))
    time.sleep(3600)
except KeyboardInterrupt:
  GPIO.cleanup()
  print('\nBye')

