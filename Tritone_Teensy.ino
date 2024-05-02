#include <Audio.h>
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
// GUItool: end automatically generated code

AudioControlSGTL5000 audioShield;
unsigned long lastPeakCountTime = 0;  // Time of the last peak count
unsigned int peakCount = 0;  // Number of peaks detected in the last second
unsigned int bpm = 0;  // Beats per minute

void setup() {
  Serial.begin(9600);  // Initialize serial communication
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
    for (int i = 1; i < 512; i++) {
      float val = fft.read(i);
      if (val > maxVal) {
        maxVal = val;
        maxFreq = i * 44100 / 1024; //sampling rate (44100) and size of fft (1024)
      //By dividing i by 1024, you get a value between 0 and 1
      //representing the position of the bin in the frequency spectrum. 
      //Multiplying this value by the sampling rate 44100 gives you the actual frequency in Hz that the bin represents. 
      //This calculation allows you to determine the frequency of the audio signal corresponding to each bin in the FFT output.
      }

    }
      float amplitude = peak1.read();
      if(amplitude < 0.5){
        amplitude = 0;
      }

      bpm = calculateBPM(amplitude);

      String tempo;
      if (bpm < 60) {
        tempo = "Largo";
      } else if (bpm < 76) {
        tempo = "Adagio";
      } else if (bpm < 108) {
        tempo = "Moderato";
      } else if (bpm < 120) {
        tempo = "Allegro";
      } else {
        tempo = "Presto";
      }
    
    Serial.println(amplitude);
    Serial.printf("Dominant Frequency: %.2f Hz | Amplitude: %.2f | Tempo: %s | BPM: %d\n", maxFreq, amplitude, tempo.c_str(), bpm);
  }
}

int calculateBPM(float amplitude) {
  // Count peaks in the last second
  unsigned long currentTime = millis();
  if (currentTime - lastPeakCountTime >= 1000) {
    // One second has elapsed, calculate BPM
    bpm = peakCount*60;
    // Reset peak count and update lastPeakCountTime
    peakCount = 0;
    lastPeakCountTime = currentTime;
  }

  // Detect peak
  if(amplitude >= 0.5){ //fine tune 
    peakCount++;
  }
  return bpm;
}
