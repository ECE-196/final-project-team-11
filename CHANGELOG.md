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

**5/25/24**
Got Teensy -> ESP32 communication working
Connect pin 0 on teensy to pin 18 on ESP
Connect pin 1 on teensy to pin 17 on ESP

use USB Serial to print to computer,
send through Serial1

See james_progress/week9/ for working communications

**5/29/24**

Got machine learning working on ESP and Teensy.
Utilized pytorch and exported parameters after training model on computer with collected music frequency data.

Model works as follows:

Inferences on 10 past domininat frequencies in last 10 seconds, as well as the current frequency with a boosted weight.
Uses a hidden layer of size 16 with two layers and ReLU activation. Trained on Break it Out by Rocket Summer

Remember to normalize inputs on arduino.