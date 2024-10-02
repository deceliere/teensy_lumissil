// #include "DFRobot_HT1632C.h"
#include "video2led.h"
#include "main.h"
#include <SD.h>
#include <I2C.h>
// #include <Arduino.h>
#include "ExponentMap.h"

const int bufferSize = W_SOURCE * H_SOURCE; // pixel q
String buffer;
matrixDot dot[192];
int values[bufferSize];
u_int16_t valueMin = 0;
u_int16_t valueMax = 0;
u_int16_t frameCount;

const byte pwm_max_value = 255;
byte steps_count;

ExponentMap<int> expoPWM(255, pwm_max_value);

void initExp(void)
{
  steps_count = expoPWM.stepsCount();
}

void printExpoScale(void)
{
  for (int i = 0; i < 256; i++)
  {
    DPRINT(i);
    DPRINT("=");
    DPRINTLN(expoPWM(i));
  }
}

void readAndProcessFileBinary(const char *filename)
{
  byte binSourceCurrent[W_SOURCE * H_SOURCE]; // Buffer pour stocker une ligne de pixels
  elapsedMillis frameDelay = 0;
  File file = SD.open(filename);
  frameCount = 0;
#ifdef SERIAL_DEBUG
  printExpoScale();
#endif
  if (file)
  {
    // Boucle de lecture des lignes du fichier
    while (file.available())
    {
      if (frameDelay > FRAME_DELAY)
      {
        file.read(binSourceCurrent, W_SOURCE * H_SOURCE);
        displayFrameBinary(binSourceCurrent, binSourceCurrent);
        frameDelay = 0;
      }
    }
    file.close();
    DPRINTLN("file closed");
  }
  else
  {
    Serial.println("Erreur lors de l'ouverture du fichier");
  }
  frameCount++;
  DPRINT("min value: ");
  DPRINTLN(valueMin);
  DPRINT("max value: ");
  DPRINTLN(valueMax);
  DPRINT("frametimer (microS) min: ");
  DPRINTLN(frameTimerMin);
  DPRINT("frametimer (microS) max: ");
  DPRINTLN(frameTimerMax);
  DPRINT("total microS variation span: ");
  DPRINTLN(frameTimerMax - frameTimerMin);
  DPRINT("total frame count: ");
  DPRINTLN(frameCount);
  // DPRINT("loop duration (S): ");
  // DPRINTLN(loopDur / 1000);
}

void initDotRandom(void)
{
  for (int i = 0; i < 192; i++)
  {
    dot[i].currentRandomTimer = random(FRAME_DELAY);
    dot[i].nextRandomTimer = random(FRAME_DELAY);
  }
}

void dotRandomize(int i)
{
    dot[i].currentRandomTimer = dot[i].nextRandomTimer;
    dot[i].nextRandomTimer = random(FRAME_DELAY);
}

#ifdef SERIAL_DEBUG
#define SAMPLE 33 // variable aleatoire pour test
#endif

void randomDotTimer(int currentFrameDelay, int i)
{
  if (dot[i].pixelTimer < dot[i].nextRandomTimer + FRAME_DELAY - dot[i].currentRandomTimer)
  {
    if (dot[i].pwmTmp != dot[i].pwmNextTmp)
      dot[i].pwmFade = (((double)(dot[i].pwmNextTmp - dot[i].pwmTmp) / (dot[i].nextRandomTimer + FRAME_DELAY - dot[i].currentRandomTimer)) * dot[i].pixelTimer) + dot[i].pwmTmp;
    else
      dot[i].pwmFade = dot[i].pwm;
    #ifdef SERIAL_DEBUG
    if (i == SAMPLE)
    {
      DPRINT("dot[");DPRINT(SAMPLE);DPRINT("].pwmFade=");DPRINTLN(dot[SAMPLE].pwmFade);
      DPRINT("dot[");DPRINT(SAMPLE);DPRINT("].pwm=");DPRINTLN(dot[SAMPLE].pwm);
      DPRINT("dot[");DPRINT(SAMPLE);DPRINT("].pwmNext=");DPRINTLN(dot[SAMPLE].pwmNext);
      DPRINT("dot[");DPRINT(SAMPLE);DPRINT("].currentRandom=");DPRINTLN(dot[SAMPLE].currentRandomTimer);
      DPRINT("dot[");DPRINT(SAMPLE);DPRINT("].nextRandom=");DPRINTLN(dot[SAMPLE].nextRandomTimer);DPRINTLN();
      DPRINT("dot[");DPRINT(SAMPLE);DPRINT("].pixelTimer=");DPRINTLN(dot[SAMPLE].pixelTimer);
      DPRINT("max time=");DPRINTLN(dot[i].nextRandomTimer + FRAME_DELAY - dot[i].currentRandomTimer);
      DPRINT("time factor=");DPRINTLN(((dot[i].nextRandomTimer + FRAME_DELAY - dot[i].currentRandomTimer) * dot[i].pixelTimer));
      DPRINT("PWM / time factor=");DPRINTLN((double)((dot[i].pwmNext - dot[i].pwm) / (dot[i].nextRandomTimer + FRAME_DELAY - dot[i].currentRandomTimer)) * dot[i].pixelTimer);
      DPRINT("portion=");DPRINTLN(((double)(dot[i].pwmNext - dot[i].pwm) / (dot[i].nextRandomTimer + FRAME_DELAY - dot[i].currentRandomTimer)) * dot[i].pixelTimer);DPRINTLN();
      // DPRINT("portion test=");DPRINTLN((double) 2/1000 * 100);DPRINTLN();

    }
    #endif
  }
  else
  {
    // dot[i].pwmFade = dot[i].pwm;
    dot[i].pwmTmp = dot[i].pwm;
    dot[i].pwmNextTmp = dot[i].pwmNext;
    dotRandomize(i);
    dot[i].pixelTimer = 0;
    #ifdef SERIAL_DEBUG
    if (i == SAMPLE)
    {
      DPRINT("DOT[");DPRINT(i);DPRINTLN("] REDEFINED");DPRINTLN();DPRINTLN();
      DPRINTLN("//////////////////////////////////////////////////////////////////////////////////////////");
      DPRINTLN("//////////////////////////////////////////////////////////////////////////////////////////");
    }
    #endif
  }
}


void fade(int currentFrameDelay)
{
  uint8_t buffer[192];

  clearBuffer(buffer, 192);
  if (!currentFrameDelay)
    currentFrameDelay = 1;
  for (int i = 0; i < W_OUTPUT * H_OUTPUT; i++)
  {
    // dot[i].pwmFade = ((float)(dot[i].pwmNext - dot[i].pwm) / (FRAME_DELAY + FRAME_DELAY * 5 / 100) * currentFrameDelay) + dot[i].pwm; // ajouter "+ FRAME_DELAY * 5 / 100" permet d'atteindre la
    dot[i].pwmFade = ((double)(dot[i].pwmNext - dot[i].pwm) / FRAME_DELAY * currentFrameDelay) + dot[i].pwm; // test pour gestion random dots
    // randomDotTimer(currentFrameDelay, i);
    writeFadeToBuffer(buffer, dot[i]);
  }
  // DPRINT(dot[SAMPLE].pwm);
  // DPRINT(" ");
  // DPRINT(dot[SAMPLE].pwmFade);
  // DPRINT(" ");
  // DPRINTLN(dot[SAMPLE].pwmNext);
  // DPRINT("currentFrameDelay=");
  // DPRINTLN(currentFrameDelay);
  IS_I2C_BufferWrite(buffer, 192, 0, Addr_GND_GND);
}

void fadeRandom(int currentFrameDelay)
{
  uint8_t buffer[192];

  clearBuffer(buffer, 192);
  if (!currentFrameDelay)
    currentFrameDelay = 1;
  for (int i = 0; i < W_OUTPUT * H_OUTPUT; i++)
  {
    // dot[i].pwmFade = ((float)(dot[i].pwmNext - dot[i].pwm) / (FRAME_DELAY + FRAME_DELAY * 5 / 100) * currentFrameDelay) + dot[i].pwm; // ajouter "+ FRAME_DELAY * 5 / 100" permet d'atteindre la
    // dot[i].pwmFade = ((float)(dot[i].pwmNext - dot[i].pwm) / FRAME_DELAY * currentFrameDelay) + dot[i].pwm; // test pour gestion random dots
    randomDotTimer(currentFrameDelay, i);
    writeFadeToBuffer(buffer, dot[i]);
  }
  // DPRINT(dot[SAMPLE].pwm);
  // DPRINT(" ");
  // DPRINT(dot[SAMPLE].pwmFade);
  // DPRINT(" ");
  // DPRINTLN(dot[SAMPLE].pwmNext);
  // DPRINT("currentFrameDelay=");
  // DPRINTLN(currentFrameDelay);
  IS_I2C_BufferWrite(buffer, 192, 0, Addr_GND_GND);
}

void readAndProcessFileBinaryFade(const char *filename)
{
  byte binSourceCurrent[W_SOURCE * H_SOURCE]; // Buffer pour stocker une ligne de pixels
  byte binSourceNext[W_SOURCE * H_SOURCE];
  elapsedMillis currentFrameDelay = 0;
  File file = SD.open(filename);
  frameCount = 0;
  if (file)
  {
    // Boucle de lecture des lignes du fichier
    while (file.available())
    {
      if (!frameCount)
      {
        file.read(binSourceCurrent, W_SOURCE * H_SOURCE);
        file.read(binSourceNext, W_SOURCE * H_SOURCE);
        displayFrameBinary(binSourceCurrent, binSourceNext);
        currentFrameDelay = 0;
        frameCount++;
      }
      if (currentFrameDelay > FRAME_DELAY)
      {
        memcpy(binSourceCurrent, binSourceNext, W_SOURCE * H_SOURCE * sizeof(uint8_t));
        file.read(binSourceNext, W_SOURCE * H_SOURCE);
        displayFrameBinary(binSourceCurrent, binSourceNext);
        // DPRINT("frame=");
        // DPRINT(frameCount);
        // DPRINT(" millis=");
        // DPRINTLN(currentFrameDelay);
        currentFrameDelay = 0;
        frameCount++;
      }
      else if (currentFrameDelay < FRAME_DELAY)
      {
        fade(currentFrameDelay);
      }
    }
    file.close();
    DPRINTLN("file closed");
  }
  else
  {
    Serial.println("Erreur lors de l'ouverture du fichier");
  }
  DPRINT("min value: ");
  DPRINTLN(valueMin);
  DPRINT("max value: ");
  DPRINTLN(valueMax);
  DPRINT("frametimer (microS) min: ");
  DPRINTLN(frameTimerMin);
  DPRINT("frametimer (microS) max: ");
  DPRINTLN(frameTimerMax);
  DPRINT("total microS variation span: ");
  DPRINTLN(frameTimerMax - frameTimerMin);
  DPRINT("total frame count: ");
  DPRINTLN(frameCount);
}

void readAndProcessFileBinaryFadeRandom(const char *filename)
{
  byte binSourceCurrent[W_SOURCE * H_SOURCE]; // Buffer pour stocker une ligne de pixels
  byte binSourceNext[W_SOURCE * H_SOURCE];
  elapsedMillis currentFrameDelay = 0;
  File file = SD.open(filename);
  frameCount = 0;
  initDotRandom();
#ifdef DEBUG //  debug random ft
  for (int i = 0; i < 192; i++)
  {
    DPRINT("dot>");
    DPRINT(i);
    DPRINT(" randomCurrent>");
    DPRINT(dot[i].currentRandomTimer);
    DPRINT(" randomNext>");
    DPRINTLN(dot[i].nextRandomTimer);
  }
#endif
  if (file)
  {
    // Boucle de lecture des lignes du fichier
    while (file.available())
    {
      if (!frameCount)
      {
        file.read(binSourceCurrent, W_SOURCE * H_SOURCE);
        file.read(binSourceNext, W_SOURCE * H_SOURCE);
        displayFrameBinary(binSourceCurrent, binSourceNext);
        currentFrameDelay = 0;
        frameCount++;
      }
      if (currentFrameDelay > FRAME_DELAY)
      {
        memcpy(binSourceCurrent, binSourceNext, W_SOURCE * H_SOURCE * sizeof(uint8_t));
        file.read(binSourceNext, W_SOURCE * H_SOURCE);
        displayFrameBinaryRandom(binSourceCurrent, binSourceNext);
        // DPRINT("frame=");
        // DPRINT(frameCount);
        // DPRINT(" millis=");
        // DPRINTLN(currentFrameDelay);
        currentFrameDelay = 0;
        frameCount++;
      }
      else if (currentFrameDelay < FRAME_DELAY)
      {
        fadeRandom(currentFrameDelay);
      }
    }
    file.close();
    DPRINTLN("file closed");
  }
  else
  {
    Serial.println("Erreur lors de l'ouverture du fichier");
  }
  DPRINT("min value: ");
  DPRINTLN(valueMin);
  DPRINT("max value: ");
  DPRINTLN(valueMax);
  DPRINT("frametimer (microS) min: ");
  DPRINTLN(frameTimerMin);
  DPRINT("frametimer (microS) max: ");
  DPRINTLN(frameTimerMax);
  DPRINT("total microS variation span: ");
  DPRINTLN(frameTimerMax - frameTimerMin);
  DPRINT("total frame count: ");
  DPRINTLN(frameCount);
}

uint8_t gammaPixel(uint8_t maxValue, uint8_t initalValue, double gamma)
{
  return ((uint8_t)(maxValue * pow((float)initalValue / maxValue, gamma)));
}

void displayFrameBinary(byte (&binArray)[W_SOURCE * H_SOURCE], byte (&binArrayNext)[W_SOURCE * H_SOURCE])
{
  int i = 0;
  uint8_t buffer[192];
  clearBuffer(buffer, 192);

  for (int y = 0; y < 12; y++)
  {
    for (int x = 0; x < 12; x++)
    {
      binaryToDot(x, y, binArray, binArrayNext, i);
      if (dot[i].pwm < valueMin)
        valueMin = dot[i].pwm;
      if (dot[i].pwm > valueMax)
        valueMax = dot[i].pwm;
      writeToBuffer(buffer, dot[i]);
      i++;
    }
  }
  IS_I2C_BufferWrite(buffer, 192, 0, Addr_GND_GND);
}

void displayFrameBinaryRandom(byte (&binArray)[W_SOURCE * H_SOURCE], byte (&binArrayNext)[W_SOURCE * H_SOURCE])
{
  int i = 0;
  uint8_t buffer[192];
  clearBuffer(buffer, 192);

  for (int y = 0; y < 12; y++)
  {
    for (int x = 0; x < 12; x++)
    {
      binaryToDot(x, y, binArray, binArrayNext, i);
      if (dot[i].pwm < valueMin)
        valueMin = dot[i].pwm;
      if (dot[i].pwm > valueMax)
        valueMax = dot[i].pwm;
      // writeToBuffer(buffer, dot[i]);
      i++;
    }
  }
  // IS_I2C_BufferWrite(buffer, 192, 0, Addr_GND_GND);
}

void binaryToDot(int x, int y, byte (&binArray)[W_SOURCE * H_SOURCE], byte (&binArrayNext)[W_SOURCE * H_SOURCE], int i)
{
  // matrixDot dot;
  dot[i].row = y;
  dot[i].col = x;
#ifdef EXPO_PWM
  dot.pwm = expoPWM(lineBuffer[x + X_OFFSET + (y * W_SOURCE + (W_SOURCE * Y_OFFSET) + (12 - y))]);
  if (dot.pwm > 255)
    dot.pwm = 255;
  if (dot.pwmNext > 255)
    dot.pwmNext = 255;
#elif defined(GAMMA_PWM) // on lit le PWM passe d abord par l exponentiel, puis par le Gamma
  dot[i].pwm = gammaPixel(255, expoPWM(binArray[x + X_OFFSET + ((12 - y) * W_SOURCE) + (W_SOURCE * Y_OFFSET)]), GAMMA);
  dot[i].pwmNext = gammaPixel(255, expoPWM(binArrayNext[x + X_OFFSET + ((12 - y) * W_SOURCE) + (W_SOURCE * Y_OFFSET)]), GAMMA);
#ifdef MIN_LED_1
  if (dot.pwm == 0)
    dot.pwm = 1;
  if (dot.pwmNext == 0)
    dot.pwmNext = 1;
#endif
#else
  dot.pwm = lineBuffer[x + X_OFFSET + (y * W_SOURCE + (W_SOURCE * Y_OFFSET) + (12 - y))];
  if (dot.pwm == 1)
    dot.pwm = 0;
  if (dot.pwmNext == 0)
    dot.pwmNext = 1;
#endif
  // return (dot);
}

// pour test - a enlever
u_int16_t frameTimerMin = 0;
u_int16_t frameTimerMax = 0;

//

void processLine(String line)
{
  // Conversion de la ligne en tableau d'entiers
  int len = line.length();
  char *str = new char[len + 1];
  elapsedMicros frameTimer = 0;
  int i = 0;
  strcpy(str, line.c_str());
  char *token = strtok(str, ",");
  while (token != NULL)
  {
    values[i++] = atoi(token);
    token = strtok(NULL, ",");
  }
  free(str);
  free(token);
// Utilisez le tableau d'entiers comme nécessaire
// Par exemple, imprimez les valeurs lues
#ifdef VALUES_DEBUG
  // for (int j = 0; j < i; j++)
  // {
  // Serial.print(values[j]);
  // Serial.print(", ");
  // }
  // Serial.println();
  Serial.print("strlen= ");
  Serial.println(len);
  DPRINT("frameTimer=");
  DPRINTLN(frameTimer);
  delay(100);
#endif
  if (!frameCount)
  {
    frameTimerMin = frameTimer;
    frameTimerMax = frameTimer;
  }
  frameCount++;
  if (frameTimer < frameTimerMin)
    frameTimerMin = frameTimer;
  if (frameTimer > frameTimerMax)
    frameTimerMax = frameTimer;
}
