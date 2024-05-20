#include <Adafruit_Protomatter.h>
#include <Arduino.h>

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

// Function to generate a random color
uint16_t getRandomColor() {
  uint8_t red = random(256);
  uint8_t green = random(256);
  uint8_t blue = random(256);
  return matrix.color565(red, green, blue);
}

// SETUP - RUNS ONCE AT PROGRAM START
void setup(void) {
  Serial.begin(9600);

  // Initialize random seed
  randomSeed(analogRead(0));

  // Initialize matrix
  ProtomatterStatus status = matrix.begin();
  Serial.print("Protomatter begin() status: ");
  Serial.println((int)status);
  if (status != PROTOMATTER_OK) {
    // DO NOT CONTINUE if matrix setup encountered an error.
    for (;;);
  }

  // Initial background color
  matrix.fillScreen(getRandomColor());

  // Draw a blue vertical line in the middle
  for (int y = 0; y < matrix.height(); y++) {
    matrix.drawPixel(matrix.width() / 2, y, matrix.color565(0, 0, 255));
  }

  // Show initial state
  matrix.show();
}

// LOOP - RUNS REPEATEDLY AFTER SETUP
void loop(void) {
  // Change the background color every second
  delay(1000);

  // Fill the screen with a random color
  matrix.fillScreen(getRandomColor());

  // Draw a blue vertical line in the middle
  for (int x = 0; x < matrix.width(); x++) {
    matrix.drawPixel(x, matrix.height() / 2, matrix.color565(0, 0, 255));
  }

  // Update the matrix display
  matrix.show();
}
