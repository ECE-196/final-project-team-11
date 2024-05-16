
#include <Audio.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <SmartMatrix3.h>
#include <FastLED.h>

#define COLOR_DEPTH 24
const uint8_t kMatrixWidth = 64;
const uint8_t kMatrixHeight = 64;
const uint8_t kRefreshDepth = 36;
const uint8_t kDmaBufferRows = 4;
const uint8_t kPanelType = SMARTMATRIX_HUB75_64ROW_MOD32SCAN;
const uint8_t kMatrixOptions = (SMARTMATRIX_OPTIONS_NONE);
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);

const int myInput = AUDIO_INPUT_MIC;

AudioInputI2S            audioInput;
AudioAnalyzeFFT1024      fft;
AudioAnalyzePeak         peak1;
AudioMixer4              mixer;
AudioConnection          patchCord0(audioInput, 0, mixer, 0);
AudioConnection          patchCord1(mixer, fft);
AudioConnection          patchCord2(audioInput, 1, peak1, 0);

AudioControlSGTL5000 audioShield;

float scale = 256.0;
float level[23];
int shown[23] = {0};
const CRGB black = CRGB(0, 0, 0);

void setup() {
    Serial.begin(9600);
    while (!Serial);
    AudioMemory(12);
    audioShield.enable();
    audioShield.inputSelect(myInput);
    fft.windowFunction(AudioWindowHanning1024);
    audioShield.volume(1);

    matrix.addLayer(&backgroundLayer);
    matrix.begin();
    matrix.setBrightness(100);
}

void loop() {
    if (fft.available()) {
        level[0] = fft.read(2);
        level[1] = fft.read(3);
        level[2] = fft.read(4);
        level[3] = fft.read(5, 6);
        level[4] = fft.read(7, 8);
        level[5] = fft.read(9, 10);
        level[6] = fft.read(11, 14);
        level[7] = fft.read(15, 19);
        level[8] = fft.read(20, 25);
        level[9] = fft.read(26, 32);
        level[10] = fft.read(33, 41);
        level[11] = fft.read(42, 52);
        level[12] = fft.read(53, 65);
        level[13] = fft.read(66, 82);
        level[14] = fft.read(83, 103);
        level[15] = fft.read(104, 127);
        level[16] = fft.read(128, 159);
        level[17] = fft.read(160, 199);
        level[18] = fft.read(200, 249);
        level[19] = fft.read(250, 311);
        level[20] = fft.read(312, 387);
        level[21] = fft.read(388, 479);
        level[22] = fft.read(480, 511);
        for (int i = 0; i < 23; i++) {
            Serial.println(level[i]);
        }

        backgroundLayer.fillScreen(black);

        for (int i = 0; i < 23; i++) {
            int val = level[i] * scale;

            if (val >= kMatrixHeight) val = kMatrixHeight - 1;

            if (val >= shown[i]) {
                shown[i] = val;
            }
            else {
                if (shown[i] > 0) shown[i] = shown[i] - 1;
                val = shown[i];
            }

            CRGB color = CRGB(CHSV(i * 23, 255, 255));
            if (shown[i] >= 0) {
                for (int y = kMatrixHeight - 1; y > kMatrixHeight - 1 - val; y--) {
                    for (int x = i * (kMatrixWidth / 23); x < (i + 1) * (kMatrixWidth / 23); x++) {
                        backgroundLayer.drawPixel(x, y, color);
                    }
                }
            }
        }
        backgroundLayer.swapBuffers();
    }
}
