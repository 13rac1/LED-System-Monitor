# Teensy LED system cpu/ram monitor

Percent of CPU and RAM usage sent to a Teensy over USB controlling a TLC5947
connected to two 10 LED bar graphs.

Tested on Ubuntu 18.04 and Windows 10.

[![Animated GIF](images/ledmonitor-anim.gif?raw=true)](images/ledmonitor-anim.gif?raw=true)

# Parts

* Teensy 2.0: https://www.pjrc.com/store/teensy.html
* Adafruit 24-Channel 12-bit PWM LED Driver TLC5947: https://www.adafruit.com/products/1429
* 10 Segment Light Bar Graph LED Displays: 1 Blue, 4 Green, 3 Orange, and 2 Red.
* Protoboards
* Wiring

## Dependencies

Required Python libraries:

* pyserial
* psutil

Install using the `requirements.txt` or with package manager. Ubuntu 18.08 packages:

```bash
sudo apt install python3-serial python3-psutil
```

## Images

[![Lit lights](images/ledmonitor-lit-sm.jpg?raw=true)](images/ledmonitor-lit.jpg?raw=true)[![Both boards](images/ledmonitor-both-sm.jpg?raw=true)](images/ledmonitor-both.jpg?raw=true)

[![Board](images/ledmonitor-board-sm.jpg?raw=true)](images/ledmonitor-board.jpg?raw=true)[![Back of display](images/ledmonitor-back-sm.jpg?raw=true)](images/ledmonitor-back.jpg?raw=true)

## Notes

* The output is drawn to frame buffers to allow for multiple animations.
* A pulse value range is calculated per frame in BPM.

## Todo

* Third bar graph for GPU or ???
