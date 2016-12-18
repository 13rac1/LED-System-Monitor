# Teensy LED system monitor

Percent of CPU and RAM usage sent to a Teensy over USB controlling a TLC5947
connected to two 10 LED bar graphs.

Tested on Ubuntu 16.04 and Windows 10.

Notes:
* The output is drawn to frame buffers to allow for multiple animations.
* A pulse value range is calculated per frame in BPM.

Parts:
* Teensy 2.0: https://www.pjrc.com/store/teensy.html
* Adafruit 24-Channel 12-bit PWM LED Driver TLC5947: https://www.adafruit.com/products/1429
* 10 Segment Light Bar Graph LED Displays: 1 Blue, 4 Green, 3 Orange, and 2 Red.
* Protoboards
* Wiring

Required Python libraries:
* pyserial
* psutil

Todo:
* Third bar graph for GPU or ???
