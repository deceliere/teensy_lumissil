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
byte lineBuffer[W_SOURCE * H_SOURCE]; // Buffer pour stocker une ligne de pixels
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

void readAndProcessFile(const char *filename)
{
  File file = SD.open(filename);
  int readFrames = 0;
  frameCount = 0;
  elapsedMillis loopDur = 0;

  if (file)
  {
    // Boucle de lecture des lignes du fichier
    while (file.available())
    {
      buffer = file.readStringUntil('\n', bufferSize * 5); // 3 chiffres max + virgule + espace
      // DPRINT("buffer: \n");
      // DPRINTLN(buffer);
      // DPRINTLN("end bufffer");
      processLine(buffer);
      displayFrame();
      delay(FRAME_DELAY);
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
  DPRINT("loop duration (S): ");
  DPRINTLN(loopDur / 1000);
}

void readAndProcessFileBinary(const char *filename)
{
  File file = SD.open(filename);
  // int readFrames = 0;
  frameCount = 0;
  elapsedMillis loopDur = 0;
#ifdef SERIAL_DEBUG
  printExpoScale();
#endif
  if (file)
  {
    // Boucle de lecture des lignes du fichier
    while (file.available())
    {
      file.read(lineBuffer, W_SOURCE * H_SOURCE);

      // buffer = file.readStringUntil('\n', bufferSize * 5); // 3 chiffres max + virgule + espace
      // DPRINT("buffer: \n");
      // DPRINTLN(buffer);
      // DPRINTLN("end bufffer");
      // processLine(buffer);
      displayFrameBinary();
      delay(FRAME_DELAY);
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

uint8_t gammaPixel(uint8_t maxValue, uint8_t initalValue, double gamma)
{
  return ((uint8_t)(maxValue * pow((float)initalValue / maxValue, gamma)));
}

void displayFrameBinary()
{
  matrixDot dot;
  uint8_t buffer[192];
  clearBuffer(buffer, 192);

  for (int y = 0; y < 12; y++)
  {
    for (int x = 0; x < 12; x++)
    {
      dot.row = y;
      dot.col = x;
#ifdef EXPO_PWM
      dot.pwm = expoPWM(lineBuffer[x + X_OFFSET + (y * W_SOURCE + (W_SOURCE * Y_OFFSET) + (12 - y))]);
      if (dot.pwm > 255)
        dot.pwm = 255;
#elif defined(GAMMA_PWM) // on lit le PWM passe d abord par l exponentiel, puis par le Gamma
      dot.pwm = gammaPixel(255, expoPWM(lineBuffer[x + X_OFFSET + (y * W_SOURCE + (W_SOURCE * Y_OFFSET) + (12 - y))]), GAMMA);
#else
      dot.pwm = lineBuffer[x + X_OFFSET + (y * W_SOURCE + (W_SOURCE * Y_OFFSET) + (12 - y))];
      if (dot.pwm == 1)
        dot.pwm = 0;
#endif
      if (dot.pwm < valueMin)
        valueMin = dot.pwm;
      if (dot.pwm > valueMax)
        valueMax = dot.pwm;
      writeToBuffer(buffer, dot);
    }
  }
  IS_I2C_BufferWrite(buffer, 192, 0, Addr_GND_GND);
}
/*
void displayFrame()
{
  matrixDot dot;
  uint8_t buffer[192];
  clearBuffer(buffer, 192);

  for (int y = 0; y < 12; y++)
  {
    for (int x = 0; x < 12; x++)
    {
      dot.row = y;
      dot.col = x;
#ifdef EXPO_PWM
      dot.pwm = expoPWM(values[x + X_OFFSET + (y * W_SOURCE + (W_SOURCE * Y_OFFSET) + (12 - y))]);
      if (dot.pwm > 255)
        dot.pwm = 255;
#elif defined(GAMMA_PWM)
      dot.pwm = gammaPixel(255, expoPWM(values[x + X_OFFSET + (y * W_SOURCE + (W_SOURCE * Y_OFFSET) + (12 - y))]), GAMMA);
#else
      dot.pwm = values[x + X_OFFSET + (y * W_SOURCE + (W_SOURCE * Y_OFFSET) + (12 - y))];
      if (dot.pwm == 1)
        dot.pwm = 0;
#endif
      if (dot.pwm < valueMin)
        valueMin = dot.pwm;
      if (dot.pwm > valueMax)
        valueMax = dot.pwm;
      writeToBuffer(buffer, dot);
    }
  }
  IS_I2C_BufferWrite(buffer, 192, 0, Addr_GND_GND);
}
*/

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
