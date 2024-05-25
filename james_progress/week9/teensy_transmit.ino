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
AudioAnalyzePeak         peak1;          //xy=819.9999847412109,558.5999908447266
AudioMixer4               mixer;
AudioConnection patchCord0(i2s1, 0, mixer, 0);
AudioConnection patchCord1(mixer, fft);
AudioConnection patchCord2(i2s1, 1, peak1, 0);

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
  if(fft.available() && peak1.available()){
    float maxFreq = 0;
    float maxVal = 0;
    for(int i = 1; i < 512; i++){
      float val = fft.read(i);
      if(val > maxVal){
        maxVal = val;
        maxFreq = i * 44100/1024;
      }
    }
    float amplitude = peak1.read();
    if(amplitude < 0.07){
      amplitude = 0;
    }

    unsigned long currentTime = millis();
    static unsigned long intervalStartTime = currentTime;
    static float sumFreq = 0;
    static float avgFreq[5] = {0}; // Array to store 5 average frequencies
    static int sampleCount = 0;
    static float sumAmplitude = 0;
    static float avgAmplitude[5] = {0};
    static float averageAmp = 0; 
    static float sumAmp = 0;
    static float averageFreq = 0; 
    static float sumFrequency = 0;

    if(currentTime - intervalStartTime <= 5000 && sampleCount < 5){
      if(currentTime - intervalStartTime >= 1000){
        avgFreq[sampleCount] = sumFreq/86;
        sumFreq = 0;
        avgAmplitude[sampleCount] = sumAmplitude/86;
        sumAmplitude = 0;
        sampleCount++;
        intervalStartTime = currentTime;
      }
      sumFreq += maxFreq;
      sumAmplitude += amplitude;
    }
    else{
      for(int i = 0; i < 5; i++){
        sumAmp += avgAmplitude[i];
        sumFrequency += avgFreq[i];
      }
      averageAmp = sumAmp/5;
      averageFreq = sumFrequency/5; 

      // Serial.printf("Average Dominant Frequencies (Hz): %.2f, %.2f, %.2f, %.2f, %.2f | Average Amplitude (Hz): %.2f, %.2f, %.2f, %.2f, %.2f | Average Amplitude (Hz): %.2f | Average Frequency (Hz): %.2f \n", avgFreq[0], avgFreq[1], avgFreq[2], avgFreq[3], avgFreq[4], avgAmplitude[0], avgAmplitude[1], avgAmplitude[2], avgAmplitude[3], avgAmplitude[4], averageAmp, averageFreq);
      for(int i = 0; i < 5; i++) {
        Serial1.printf("%.2f,", avgFreq[i]);
      }
      Serial1.print("\n");

      sumFreq = 0;
      sampleCount = 0;
      sumAmplitude = 0;
      averageAmp = 0; 
      averageFreq = 0;
      sumAmp = 0;
      sumFrequency = 0;

      intervalStartTime = currentTime; 
    }
  }
}

