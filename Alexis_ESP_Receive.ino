#include <cppQueue.h>
#include <Adafruit_Protomatter.h>
#include <arduinoFFT.h>


#define SAMPLES 512              // Must be a power of 2
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

uint16_t color;
cppQueue freqQueue(sizeof(float), 10, FIFO); // FIFO: First In First Out

double vReal[SAMPLES];
double vImag[SAMPLES];

void setup() {
  USBSerial.begin(9600); // Initialize serial communication for debugging (USB Serial)
  Serial1.begin(115200, SERIAL_8N1, 17, 18);
  analogReadResolution(12); // Set ADC resolution to 12 bits (0-4095)

  samplingPeriodUs = round(1000000 * (1.0 / SAMPLING_FREQUENCY));

  // Initialize matrix
  matrix.begin();
}

// Function to map FFT results to matrix
void drawSpectrogram(uint16_t color) {
  matrix.fillScreen(0); // Clear the screen first

  randomSeed(analogRead(0));

  // Find the index of the maximum magnitude in the FFT results
  for (int x = 1; x < SAMPLES; x = x + 8) {

    // Map frequency to color and draw on matrix
    int intensity = map(vReal[x], 0, 1000, 0, 16); // Half of the original height (64/2 = 32)
    intensity = constrain(intensity, 0, 16);

    //uint16_t color;
    //int hue = map(x, 0, SAMPLES / 2, 0, 255); // Map frequency to hue (0-255)
    //color = matrix.colorHSV(hue * 256, 255, 255); // Convert hue to RGB color

    int startY = 32; // Middle of the matrix (64/2 = 32)

    for (int y = 0; y < intensity; y++) {
      if ( (x/8) % 2 == 0 ) {
        matrix.drawPixel(x / 8, startY - y, color); // Go up
      } else {
        matrix.drawPixel(x / 8, startY + y, color); // Go down
      }
    }
  }
  matrix.show(); // Update the display
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
  FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.complexToMagnitude(vReal, vImag, SAMPLES);

  if (Serial1.available() >= sizeof(float)) {
    float receivedFreq;
    Serial1.readBytes((byte*)&receivedFreq, sizeof(float));

    // Remove the oldest element if the queue is full
    if (freqQueue.isFull()) {
        float temp;
        freqQueue.pop(&temp);
    }

    // Add the received frequency to the queue
    freqQueue.push(&receivedFreq);
}

  USBSerial.print("All Frequencies in Queue: "); //FIFO
  int queueSize = freqQueue.getCount();
  for (int i = 0; i < queueSize; i++) {
    float freq;
    freqQueue.pop(&freq); // Pop the element from the queue
    USBSerial.print(freq, 2); //last one in, last out out, read it from right to left 
    USBSerial.print(", ");
    freqQueue.push(&freq); // Push the element back to the queue
  }
  USBSerial.println();

 if (!freqQueue.isEmpty()) {  
    float freq1;
    freqQueue.peek(&freq1);
    int hue = map(freq1, 0, SAMPLES / 2, 0, 255); // Map frequency to hue (0-255)
    color = matrix.colorHSV(hue * 256, 255, 255); // Convert hue to RGB color

    // Draw the spectrogram on the matrix with the calculated color
    drawSpectrogram(color);
  }
}
