#include <SD.h>

#ifndef VIDEO2LED_H
#define VIDEO2LED_H

#include "ExponentMap.h"
// #define SERIAL_DEBUG
// #define VALUES_DEBUG
// #define WAIT_SERIAL
// #define EXPO_PWM
#define GAMMA_PWM

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

#define FRAME_DELAY 18
// #define BRIGHTNESS 4 // max 15

// #define TOTAL_SOURCE_PIXELS 5184
#define W_SOURCE 28 // source width 28
#define H_SOURCE 15 // source height 15
// #define W_OUTPUT 24 // panel width
// #define H_OUTPUT 8  // panel height
// #define W_OFFSET 0
// #define H_OFFSET 0
#define X_OFFSET 10
#define Y_OFFSET 0
#define GAMMA 1

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
void displayFrame();
void displayFrameBinary();

// test exponent
void initExp(void);

// test correct led curve
uint8_t gammaPixel(uint8_t maxValue, uint8_t initalValue, double gamma);
void printExpoScale(void);


#endif