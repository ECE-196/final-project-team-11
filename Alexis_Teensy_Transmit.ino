#include <Audio.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

const int myInput = AUDIO_INPUT_MIC;
// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=616.9999847412109,509.59999084472656
AudioAnalyzeFFT1024      fft;            //xy=806.2000350952148,449.0000047683716
AudioMixer4               mixer;
AudioConnection patchCord0(i2s1, 0, mixer, 0);
AudioConnection patchCord1(mixer, fft);

AudioControlSGTL5000 audioShield;

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  Serial1.begin(115200); // Initialize Serial1 for communication with ESP32

  while (!Serial);      // Wait for serial connection
   AudioMemory(30);
   audioShield.enable();
   audioShield.inputSelect(myInput);
   audioShield.volume(1);
   fft.windowFunction(AudioWindowHanning1024);
}

void loop() {
  if(fft.available()){
    float maxFreq = 0;
    float maxVal = 0;
    for(int i = 1; i < 512; i++){
      float val = fft.read(i);
      if(val > maxVal){
        maxVal = val;
        maxFreq = i * 44100/1024;
      }
    }

    unsigned long currentTime = millis();
    static unsigned long intervalStartTime = currentTime;
    static float sumFreq = 0;
    static float avgFreq = 0; // Array to store 5 average frequencies
    static int sampleCount = 0;

    sumFreq += maxFreq;
    sampleCount++;

      if(currentTime - intervalStartTime >= 1000){
        avgFreq = sumFreq/86;
        Serial1.write((byte*)&avgFreq, sizeof(float));

         sumFreq = 0;
        sampleCount = 0;
        intervalStartTime = currentTime;
      }
  }
}
