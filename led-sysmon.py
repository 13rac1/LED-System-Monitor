#!/usr/bin/env python3
import sys, serial
import psutil
import time
from serial.tools import list_ports
from serial.serialutil import SerialException

# https://pyserial.readthedocs.io/en/latest/

# Stock Teensy USB serial microcontroller program id data:
VENDOR_ID = "16C0"
PRODUCT_ID = "0483"
SERIAL_NUMBER = "12345"

def getTeensyPort():
    for port in list(list_ports.comports()):
        if "USB VID:PID=%s:%s SER=%s"%(VENDOR_ID, PRODUCT_ID, SERIAL_NUMBER) in port[2]:
            return port[0]

def send_data():
    TeensyPort = getTeensyPort()
    if TeensyPort:
        print("Teensy found on port %s" % TeensyPort)
    else:
        print("No compatible Teensy found. Retry in one second.")
        time.sleep(1)
        return


    # Open serial port
    try:
        ser = serial.Serial(TeensyPort)
    except SerialException:
        print("Could not open port. Retry in one second.")
        time.sleep(1)
        return

    while True:
        cpu = int(psutil.cpu_percent())
        ram = int(psutil.virtual_memory().percent)
        out = bytes(bytearray([cpu, ram]))

        try:
            # Uncomment to test connect/nodata states.
            # time.sleep(2)
            ser.write(out)
            #print(ord(ser.read(1)))
        except SerialException:
            print("Lost Connection.")
            ser.close()
            return

        time.sleep(0.2)

def main():
    while True:
        send_data()

if __name__ == "__main__":
	main()
