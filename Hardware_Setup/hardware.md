Hardware Setup:

Teensy - Audio shield

LED size: 1.5 mm
Scan rate - 

https://github.com/pixelmatix/SmartMatrix/tree/c76d3efb417e802f0508740c2779e44463059bec

Use this commit

https://github.com/GadgetReboot/LEDPanel/tree/master/SmartMatrix3_Hack/src_file_changes


Edited Smart Matrix 3 Library



Paul Stoffregen Audio example
https://github.com/PaulStoffregen/Audio/blob/master/examples/Analysis/SpectrumAnalyzerBasic/SpectrumAnalyzerBasic.ino



WITH ESP


#define R1_PIN 46
#define G1_PIN 45
#define B1_PIN 42
#define R2_PIN 41
#define G2_PIN 40
#define B2_PIN 39
#define A_PIN 38
#define B_PIN 37
#define C_PIN 36
#define D_PIN 35
#define E_PIN 48
#define LAT_PIN 34
#define OE_PIN 33
#define CLK_PIN 47

uint8_t rgbPins[] = {R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN};
uint8_t addrPins[] = {A_PIN, B_PIN, C_PIN, D_PIN, E_PIN};
uint8_t clockPin = CLK_PIN;
uint8_t latchPin = LAT_PIN;
uint8_t oePin = OE_PIN;
