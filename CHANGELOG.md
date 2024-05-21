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


**5/13/24**

https://www.youtube.com/watch?v=bu308tAqyIw

Received Parts, trying to set up Teensy -> RGB shield direct. Very useful video above.

**5/15/24**
Got Teensy 3.6 connected to RGB board;
Edited Teensy library 3.

**5/20/24**
Completely switched to ESP32 Devboard because of increased compatibility with Adafruit 64x64 and simpler audio processing capabilities.

Worked on poster

Added website: https://hamesjan.github.io/tritone_website