#include <SD.h>
#include "main.h"

#ifndef VIDEO2LED_H
#define VIDEO2LED_H

#include "ExponentMap.h"
// #define SERIAL_DEBUG
// #define WAIT_SERIAL
// #define VALUES_DEBUG
// #define EXPO_PWM
#define GAMMA_PWM
// #define MIN_LED_1 // min led level = 1

#ifdef SERIAL_DEBUG
#define DPRINT(x) Serial.print(x)
#define DPRINTLN(x) Serial.println(x)
#else
#define DPRINT(x)
#define DPRINTLN(x)

#endif

// teensy pins
#define DATA 5
#define WR 4
#define RD 3
#define CS 2 // D2 on firebeetle

#define FRAME_DELAY 20 // typ 18
// #define BRIGHTNESS 4 // max 15

// #define TOTAL_SOURCE_PIXELS 5184
#define W_SOURCE 28 // source width 28
#define H_SOURCE 15 // source height 15
#define W_OUTPUT 12 // panel width
#define H_OUTPUT 12 // panel height
// #define W_OFFSET 0
// #define H_OFFSET 0
#define X_OFFSET 10
#define Y_OFFSET 0
#define GAMMA 1
#define BUFFER_SIZE 192 // number of pixels (16 * 12 // 16 par ligne, car Lumissil pensé sur une base de 8 > 1 à 6, puis 9 à 14

// fts
// test //
void lineDot();

// pour test / mesures - a enlever
extern u_int16_t frameTimerMin;
extern u_int16_t frameTimerMax;
extern u_int16_t frameCount;
//

// file //
void processLine(String line);
void readAndProcessFile(const char *filename);
void readAndProcessFileBinary(const char *filename);
void readAndProcessFileBinaryFade(const char *filename);
void readAndProcessFileBinaryFadeRandom(const char *filename);
void displayFrame();
void displayFrameBinary(byte (&binArray)[W_SOURCE * H_SOURCE], byte (&binArrayNext)[W_SOURCE * H_SOURCE]);
void displayFrameBinaryRandom(byte (&binArray)[W_SOURCE * H_SOURCE], byte (&binArrayNext)[W_SOURCE * H_SOURCE]);
// void displayFrameBinary();
void binaryToDot(int x, int y, byte (&binArray)[W_SOURCE * H_SOURCE], byte (&binArrayNext)[W_SOURCE * H_SOURCE], int i);
void initDotRandom(void);
void dotRandomize(int i);
void randomDotTimer(int currentFrameDelay);
void fade(int currentFrameDelay);
void fadeRandom(int currentFrameDelay);


// test exponent
void initExp(void);

// test correct led curve
uint8_t gammaPixel(uint8_t maxValue, uint8_t initalValue, double gamma);
void printExpoScale(void);

#endif