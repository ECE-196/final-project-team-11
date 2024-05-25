#include <Adafruit_Protomatter.h>
#include <arduinoFFT.h>

#define SAMPLES 512 // Must be a power of 2
#define SAMPLING_FREQUENCY 44100 // Hz, must be less than 1MHz

#define MIC_PIN 1               // Analog input pin for the microphone

// Define the pin numbers for the matrix connections
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

// Initialize arrays for the RGB pins and address pins
uint8_t rgbPins[] = {R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN};
uint8_t addrPins[] = {A_PIN, B_PIN, C_PIN, D_PIN, E_PIN};
uint8_t clockPin = CLK_PIN;
uint8_t latchPin = LAT_PIN;
uint8_t oePin = OE_PIN;

// Create an instance of the Protomatter matrix for a 64x64 matrix
Adafruit_Protomatter matrix(
  64,          // Width of matrix (or matrix chain) in pixels
  6,           // Bit depth, 1-6
  1, rgbPins,  // Number of matrix chains, array of 6 RGB pins for each
  5, addrPins, // Number of address pins (height is inferred), array of pins
  clockPin, latchPin, oePin, // Other matrix control pins
  false        // No double-buffering here (see "doublebuffer" example)
);

// Instantiate the FFT object
ArduinoFFT<double> FFT = ArduinoFFT<double>();

unsigned int samplingPeriodUs;
unsigned long microseconds;

double vReal[SAMPLES];
double vImag[SAMPLES];

// Function to map FFT results to matrix
void drawSpectrogram() {
  matrix.fillScreen(0); // Clear the screen first
  
  for (int x = 0; x < SAMPLES / 2; x++) {
    int intensity = map(vReal[x], 0, 1000, 0, 63);
    intensity = constrain(intensity, 0, 63);
    for (int y = 0; y < intensity; y++) {
      matrix.drawPixel(x, 63 - y, matrix.color565(0, 0, 255)); // Blue color
    }
  }
  matrix.show(); // Update the display
}

void setup() {
  USB.begin(115200);
  analogReadResolution(12); // Set ADC resolution to 12 bits (0-4095)

  samplingPeriodUs = round(1000000 * (1.0 / SAMPLING_FREQUENCY));

  // Initialize matrix
  ProtomatterStatus status = matrix.begin();
  Serial.print("Protomatter begin() status: ");
  Serial.println((int)status);
  if (status != PROTOMATTER_OK) {
    // DO NOT CONTINUE if matrix setup encountered an error.
    for (;;);
  }

  Serial.println("Microphone FFT Spectrogram started...");
}

void loop() {
  // Sample the microphone signal
  for (int i = 0; i < SAMPLES; i++) {
    microseconds = micros();

    vReal[i] = analogRead(MIC_PIN);
    vImag[i] = 0;

    while (micros() < (microseconds + samplingPeriodUs)) {
      // Wait for the next sample time
    }
  }

  // Compute FFT
  FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HANN, FFT_FORWARD);
  FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.complexToMagnitude(vReal, vImag, SAMPLES);
  // 
  float maxFreq = 0;
  float maxVal = 0;
  for(int i = 1; i < 512; i++){
      float val = vReal[i];
      if(val > maxVal){
        maxVal = val;
        maxFreq = i * 44100/1024;
      }
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
        sampleCount++;
        intervalStartTime = currentTime;
      }
      sumFreq += maxFreq;
    } else{
      for(int i = 0; i < 5; i++){
        sumFrequency += avgFreq[i];
      }
      averageFreq = sumFrequency/5; 
      Serial.printf("Average Dominant Frequencies (Hz): %.2f, %.2f, %.2f, %.2f, %.2f | Average Frequency (Hz): %.2f \n", avgFreq[0], avgFreq[1], avgFreq[2], avgFreq[3], avgFreq[4], averageFreq);
      sumFreq = 0;
      sampleCount = 0;
      averageFreq = 0;
      sumFrequency = 0;

      intervalStartTime = currentTime; 
    }
    

  // Draw the spectrogram on the matrix
  drawSpectrogram();

  // Small delay before next FFT calculation
}
