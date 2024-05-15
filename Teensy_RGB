#include <Audio.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <SmartMatrix3.h>
#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    64
#define BRIGHTNESS  255
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

const int myInput = AUDIO_INPUT_MIC;
// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=616.9999847412109,509.59999084472656
AudioAnalyzeFFT1024      fft;            //xy=806.2000350952148,449.0000047683716
AudioAnalyzePeak         peak1;          //xy=819.9999847412109,558.5999908447266
AudioMixer4               mixer;
AudioConnection patchCord0(i2s1, 0, mixer, 0);
AudioConnection patchCord1(mixer, fft);
AudioConnection patchCord2(i2s1, 1, peak1, 0);

AudioControlSGTL5000 audioShield;

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  while (!Serial);      // Wait for serial connection
   AudioMemory(30);
   audioShield.enable();
   audioShield.inputSelect(myInput);
   audioShield.volume(1);
   fft.windowFunction(AudioWindowHanning1024);

   // Assuming you want to use SmartMatrix3
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  if (fft.available() && peak1.available()) {
    float maxFreq = 0;
    float maxVal = 0;
    for (int i = 1; i < 512; i++) {
      float val = fft.read(i);
      if (val > maxVal) {
        maxVal = val;
        maxFreq = i * 44100 / 1024;
      }
    }
    float amplitude = peak1.read();
    if (amplitude < 0.07) {
      amplitude = 0;
    }

    unsigned long currentTime = millis();
    static unsigned long intervalStartTime = currentTime;
    static float sumFreq = 0;
    static float avgFreq[5] = {0}; // Array to store 5 average frequencies
    static int   sampleCount = 0;
    static float sumAmplitude = 0;
    static float avgAmplitude[5] = {0};
    static float averageAmp = 0;
    static float sumAmp = 0;
    static float averageFreq = 0;
    static float sumFrequency = 0;

    if (currentTime - intervalStartTime <= 5000 && sampleCount < 5) {
      // Reset variables for next iteration
      sumFreq = 0;
      sampleCount = 0;
      sumAmplitude = 0;
      averageAmp = 0;
      averageFreq = 0;
      sumAmp = 0;
      sumFrequency = 0;
      if (currentTime - intervalStartTime >= 1000) {
        avgFreq[sampleCount] = sumFreq / 86;
        sumFreq = 0;
        avgAmplitude[sampleCount] = sumAmplitude / 86;
        sumAmplitude = 0;
        sampleCount++;
        intervalStartTime = currentTime;
      }
      sumFreq += maxFreq;
      sumAmplitude += amplitude;
    } else {
      for (int i = 0; i < 5; i++) {
        sumAmp += avgAmplitude[i];
        sumFrequency += avgFreq[i];
      }
      averageAmp = sumAmp / 5;
      averageFreq = sumFrequency / 5;

      // Map frequency to color
      uint8_t colorIndex = map(averageFreq, 0, 1000, 0, 255);
      CRGB color = ColorFromPalette(RainbowColors_p, colorIndex);

      // Map amplitude to LED brightness
      uint8_t brightness = map(averageAmp, 0, 1, 0, 255);

      // Update LED colors and brightness
      for (int j = 0; j < NUM_LEDS; j++) {
        leds[j] = color;
        leds[j].fadeToBlackBy(255 - brightness);
      }
      // Display the LEDs
      FastLED.show();
      intervalStartTime = currentTime;
    }
  }
}
