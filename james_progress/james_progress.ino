/*
 * Modified example copied from FastLED 3.0 Branch - originally written by Daniel Garcia
 * This example shows how to use some of FastLED's functions with the SmartMatrix Library
 * using the SmartMatrix buffers directly instead of FastLED's buffers.
 * FastLED's dithering and color balance features can't be used this way, but SmartMatrix can draw in
 * 36-bit color and so dithering may not provide much advantage.
 *
 * This example requires FastLED 3.0 or higher.  If you are having trouble compiling, see
 * the troubleshooting instructions here:
 * https://github.com/pixelmatix/SmartMatrix/#external-libraries
 */

#include <SmartMatrix3.h>
#include <FastLED.h>

#include <Audio.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>


#define COLOR_DEPTH 24                  // This sketch and FastLED uses type `rgb24` directly, COLOR_DEPTH must be 24
const uint8_t kMatrixWidth = 64;        // known working: 32, 64, 96, 128
const uint8_t kMatrixHeight = 64;       // known working: 16, 32, 48, 64
const uint8_t kRefreshDepth = 36;       // known working: 24, 36, 48
const uint8_t kDmaBufferRows = 4;       // known working: 2-4, use 2 to save memory, more to keep from dropping frames and automatically lowering refresh rate
const uint8_t kPanelType = SMARTMATRIX_HUB75_64ROW_MOD32SCAN; // use SMARTMATRIX_HUB75_16ROW_MOD8SCAN for common 16x32 panels, or use SMARTMATRIX_HUB75_64ROW_MOD32SCAN for common 64x64 panels
const uint8_t kMatrixOptions = (SMARTMATRIX_OPTIONS_NONE);      // see http://docs.pixelmatix.com/SmartMatrix for options
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);
const uint8_t kScrollingLayerOptions = (SM_SCROLLING_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);
SMARTMATRIX_ALLOCATE_SCROLLING_LAYER(scrollingLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kScrollingLayerOptions);

// The 32bit version of our coordinates
static uint16_t x;
static uint16_t y;
static uint16_t z;

// We're using the x/y dimensions to map to the x/y pixels on the matrix.  We'll
// use the z-axis for "time".  speed determines how fast time moves forward.  Try
// 1 for a very slow moving effect, or 60 for something that ends up looking like
// water.
// uint16_t speed = 1; // almost looks like a painting, moves very slowly
uint16_t speed = 20; // a nice starting speed, mixes well with a scale of 100
// uint16_t speed = 33;
// uint16_t speed = 100; // wicked fast!

// Scale determines how far apart the pixels in our noise matrix are.  Try
// changing these values around to see how it affects the motion of the display.  The
// higher the value of scale, the more "zoomed out" the noise iwll be.  A value
// of 1 will be so zoomed in, you'll mostly see solid colors.

// uint16_t scale = 1; // mostly just solid colors
// uint16_t scale = 4011; // very zoomed out and shimmery
uint16_t scale = 31;

// This is the array that we keep our computed noise values in
uint8_t noise[kMatrixWidth][kMatrixHeight];

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
  // uncomment the following lines if you want to see FPS count information
  // Serial.begin(38400);
  // Serial.println("resetting!");
  delay(3000);

  matrix.addLayer(&backgroundLayer); 
  matrix.begin();

  backgroundLayer.setBrightness(128);

  // Initialize our coordinates to some random values
  x = random16();
  y = random16();
  z = random16();

  Serial.begin(9600);  // Initialize serial communication
  while (!Serial);      // Wait for serial connection
   AudioMemory(30);
   audioShield.enable();
   audioShield.inputSelect(myInput);
   audioShield.volume(1);
   fft.windowFunction(AudioWindowHanning1024);

  // Show off smart matrix scrolling text

}




// Fill the x/y array of 8-bit noise values using the inoise8 function.
void fillnoise8() {
  for(int i = 0; i < kMatrixWidth; i++) {
    int ioffset = scale * i;
    for(int j = 0; j < kMatrixHeight; j++) {
      int joffset = scale * j;
      noise[i][j] = inoise8(x + ioffset,y + joffset,z);
    }
  }
  z += speed;
}

void loop() {
  float maxFreq = 0;
  static float avgFreq[2] = {0}; // Array to store 5 average frequencies

  if(fft.available()){
    float maxVal = 0;
    for(int i = 1; i < 512; i++){
      float val = fft.read(i);
      if(val > maxVal){
        maxVal = val;
        maxFreq = i * 44100/1024;
      }
    }
    float amplitude = peak1.read();

    unsigned long currentTime = millis();
    static unsigned long intervalStartTime = currentTime;
    static float sumFreq = 0;
    static int sampleCount = 0;
    static float sumAmplitude = 0;
    static float avgAmplitude[2] = {0};




    if(currentTime - intervalStartTime <= 2000 && sampleCount < 2){
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
      Serial.printf("Average Dominant Frequencies (Hz): %.2f, %.2f | Average Amplitude (Hz): %.2f, %.2f,  \n", avgFreq[0], avgFreq[1] , avgAmplitude[0], avgAmplitude[1]);
      sumFreq = 0;
      sampleCount = 0;
      sumAmplitude = 0;
      intervalStartTime = currentTime; 
     }
  }

  // ADAFRUIT MATRIX


  static uint8_t circlex = 0;
  static uint8_t circley = 0;



  // if sketch uses swapBuffers(false), wait to get a new backBuffer() pointer after the swap is done:
  // while(backgroundLayer.isSwapPending());

  // rgb24 *buffer = backgroundLayer.backBuffer();

  

  // static uint8_t ihue=0;
  // fillnoise8();
  // for(int i = 0; i < kMatrixWidth; i++) {
  //   for(int j = 0; j < kMatrixHeight; j++) {
  //     // We use the value at the (i,j) coordinate in the noise
  //     // array for our brightness, and the flipped value from (j,i)
  //     // for our pixel's hue.
  //     buffer[kMatrixWidth*j + i] = CRGB(CHSV(noise[j][i],255,noise[i][j]));

  //     // You can also explore other ways to constrain the hue used, like below
  //     // buffer[kMatrixHeight*j + i] = CRGB(CHSV(ihue + (noise[j][i]>>2),255,noise[i][j]));
  //   }
  // }
  // ihue+=1;

  // backgroundLayer.fillCircle(circlex % kMatrixWidth,circley % kMatrixHeight,6,CRGB(CHSV(ihue+128,255,255)));
  // circlex += random16(2);
  // circley += random16(2);

  // // buffer is filled completely each time, use swapBuffers without buffer copy to save CPU cycles
  // backgroundLayer.swapBuffers(false);
  while (backgroundLayer.isSwapPending());

    // Get the buffer
    rgb24 *buffer = backgroundLayer.backBuffer();

    // Determine color based on avgFreq[0]
    uint8_t hue = map(avgFreq[0], 0, 2000, 0, 255); // Map frequency to hue (0 to 2000 Hz)
    CRGB color = CHSV(hue, 255, 255);

    // Fill the buffer with the determined color
    for (int i = 0; i < kMatrixWidth; i++) {
        for (int j = 0; j < kMatrixHeight; j++) {
            buffer[kMatrixWidth * j + i] = color;
        }
    }

    // Update the matrix
    backgroundLayer.swapBuffers(false);
  //matrix.countFPS();      // print the loop() frames per second to Serial

  
}
