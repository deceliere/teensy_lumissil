// #include "DFRobot_HT1632C.h"
#include "video2led.h"
#include "main.h"
#include <SD.h>
#include <I2C.h>
// #include <Arduino.h>
#include "ExponentMap.h"

const int bufferSize = W_SOURCE * H_SOURCE; // pixel q
// int pixel[bufferSize];
String buffer;
matrixDot dot[192];
// byte lineBuffer[W_SOURCE * H_SOURCE]; // Buffer pour stocker une ligne de pixels
// byte lineBufferNext[W_SOURCE * H_SOURCE];
// byte binSourceCurrent[W_SOURCE * H_SOURCE]; // Buffer pour stocker une ligne de pixels
// byte binSourceNext[W_SOURCE * H_SOURCE];
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
        // DPRINT("buffer: \n");
        // DPRINTLN(buffer);
        // DPRINTLN("end bufffer");
        // processLine(buffer);
        displayFrameBinary(binSourceCurrent, binSourceCurrent);
        frameDelay = 0;
        // delay(FRAME_DELAY);
      }
      // DPRINT("frame ");
      // DPRINTLN(readFrames);
      // readFrames++;
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
  DPRINT("loop duration (S): ");
  DPRINTLN(loopDur / 1000);
}


void fade(int currentFrameDelay)
{
  uint8_t buffer[192];
  clearBuffer(buffer, 192);
  if (!currentFrameDelay)
    currentFrameDelay = 1;

  for (int i = 0; i < W_OUTPUT * H_OUTPUT; i++)
  {
    DPRINT("row=");
    DPRINT(dot[i].row);
    DPRINT("\tcol=");
    DPRINT(dot[i].col);
    DPRINT("\tpwm=");
    DPRINT(dot[i].pwm);
    DPRINT("\tpwmNext=");
    DPRINT(dot[i].pwmNext);
    DPRINT("\tpwmFade=");
    DPRINTLN(dot[i].pwmFade);
    // DPRINT("pwm[");
    // DPRINT(1);
    // DPRINT("] current=");
    // DPRINT(dot[i].pwm);
    // DPRINT(" / next=");
    // DPRINTLN(dot[i].pwmNext);
    dot[i].pwmFade = ((float)(dot[i].pwmNext - dot[i].pwm) / (FRAME_DELAY + FRAME_DELAY * 5 / 100)  * currentFrameDelay) + dot[i].pwm;
    // dot[0].pwmFade = 255;
    writeFadeToBuffer(buffer, dot[i]);
    // DPRINT("pwmFade[");
    // DPRINT(0);
    // DPRINT("]=");
    // DPRINTLN(dot[0].pwmFade);
  }
  // DPRINT("pwm[");
  // DPRINT(sample);
  // DPRINT("] current=");
  DPRINT(dot[sample].pwm);
  // DPRINT(" / next=");
  DPRINT(" ");
  DPRINT(dot[sample].pwmFade);
  DPRINT(" ");
  DPRINTLN(dot[sample].pwmNext);
  // DPRINT("pwmFade[");
  // DPRINT(sample);
  // DPRINT("]=");
  DPRINT("currentFrameDelay=");
  DPRINTLN(currentFrameDelay);
  // while(1)
  // ;

  // for (int y = 0; y < 12; y++)
  // {
  //   for (int x = 0; x < 12; x++)
  //   {
  //     dot.pwm = 0;
  //     writeToBuffer(buffer, dot);
  //   }
  // }
  IS_I2C_BufferWrite(buffer, 192, 0, Addr_GND_GND);
  // return (dot);
}

void readAndProcessFileBinaryFade(const char *filename)
{
  byte binSourceCurrent[W_SOURCE * H_SOURCE]; // Buffer pour stocker une ligne de pixels
  byte binSourceNext[W_SOURCE * H_SOURCE];
  elapsedMillis currentFrameDelay = 0;
  File file = SD.open(filename);
  // int readFrames = 0;
  frameCount = 0;
  // matrixDot dot;
  // elapsedMillis loopDur = 0;
#ifdef SERIAL_DEBUG
  printExpoScale();
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
        // for(int i = 0; i < 192; i++)
        // {
        //   DPRINT("binSource["); DPRINT(i); DPRINT("] current="); DPRINT(binSourceCurrent[i]); DPRINT(" / next="); DPRINTLN(binSourceNext[i]);
        // }
        // while(1)
        //   ;
        displayFrameBinary(binSourceCurrent, binSourceNext);
        currentFrameDelay = 0;
        frameCount++;
      }
      if (currentFrameDelay > FRAME_DELAY)
      {
        memcpy(binSourceCurrent, binSourceNext, W_SOURCE * H_SOURCE * sizeof(uint8_t));
        file.read(binSourceNext, W_SOURCE * H_SOURCE);
        // buffer = file.readStringUntil('\n', bufferSize * 5); // 3 chiffres max + virgule + espace
        // DPRINT("buffer: \n");
        // DPRINTLN(buffer);
        // DPRINTLN("end bufffer");
        // processLine(buffer);
        displayFrameBinary(binSourceCurrent, binSourceNext);
        currentFrameDelay = 0;
        // delay(FRAME_DELAY);
        frameCount++;
      }
      else if (currentFrameDelay < FRAME_DELAY)
      {
        fade(currentFrameDelay);
      }
      // DPRINT("frame ");
      // DPRINTLN(readFrames);
      // readFrames++;
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
  // DPRINT("loop duration (S): ");
  // DPRINTLN(loopDur / 1000);
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
      dot[i] = binaryToDot(x, y, binArray, binArrayNext);
      if (dot[i].pwm < valueMin)
        valueMin = dot[i].pwm;
      if (dot[i].pwm > valueMax)
        valueMax = dot[i].pwm;
      writeToBuffer(buffer, dot[i]);
      i++;
    }
  }
  IS_I2C_BufferWrite(buffer, 192, 0, Addr_GND_GND);
  // return dot;
}

matrixDot binaryToDot(int x, int y, byte (&binArray)[W_SOURCE * H_SOURCE], byte (&binArrayNext)[W_SOURCE * H_SOURCE])
{
  matrixDot dot;
  dot.row = y;
  dot.col = x;
#ifdef EXPO_PWM
  dot.pwm = expoPWM(lineBuffer[x + X_OFFSET + (y * W_SOURCE + (W_SOURCE * Y_OFFSET) + (12 - y))]);
  if (dot.pwm > 255)
    dot.pwm = 255;
  if (dot.pwmNext > 255)
    dot.pwmNext = 255;
#elif defined(GAMMA_PWM) // on lit le PWM passe d abord par l exponentiel, puis par le Gamma
  dot.pwm = gammaPixel(255, expoPWM(binArray[x + X_OFFSET + ((12 - y) * W_SOURCE) + (W_SOURCE * Y_OFFSET)]), GAMMA);
  dot.pwmNext = gammaPixel(255, expoPWM(binArrayNext[x + X_OFFSET + ((12 - y) * W_SOURCE) + (W_SOURCE * Y_OFFSET)]), GAMMA);
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
  return (dot);
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
/*
void loop()
{
  // Lecture du fichier
  readAndProcessFile("test_processing.txt");
}
*/
