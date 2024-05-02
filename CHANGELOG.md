**5/1/24**

Status so far:
Uri finished PCB design for microphone quality amplification
Alexis working on Teensy audio capture function
James working on Machine learning algorithm and optimization for audio to RGB array conversion

What we got done today: Figured out UART configuration for Teensy to ESP32s3 communication

** Hardware Config**
Connect pin 0 on teensy to pin 17 on ESP
Connect pin 1 on teensy to pin 18 on ESP
^ for UART communication

Trying to figure out how to transmit data in packets every 5 seconds from teensy to ESP32.

