#include <Arduino.h>
// #include<Wire.h>
#include <I2C.h>
#include <Wire.h>
#include <avr/pgmspace.h>
#include <image.h>
#include <main.h>
#include "video2led.h"
#include <SD.h>

#define WAITMILLIS 500

void IS31FL3737B_Test_mode1(void);
void IS31FL3737B_init(void);
uint8_t ledMatrix[192];

void setup()
{
  // #ifdef DEBUG_VIDEO
#ifdef WAIT_SERIAL
  Serial.begin(115200);
  while (!Serial)
    ;
#endif

  Wire.begin();
  // Wire.setClock(800000UL); // I2C 800kHz
  // randomSeed(analogRead(1));
  pinMode(13, OUTPUT);
  IS31FL3737B_init();
  digitalWrite(13, 20);
  DPRINT("Serial ok\n");
  initExp(); // initialisation de la fonction exponentielle
  // Initialisation de la carte SD
  if (!SD.begin(BUILTIN_SDCARD))
  {
    Serial.println("Erreur lors de l'initialisation de la carte SD");
    while (1)
      ;
  }

  allLedPWMfull();
  delay(200);
}

void loop()
{
  // resetALlLedPWM();
  // IS31FL3737B_Test_mode1(); // breath mode
  // delay(1000);
  // bufferTest();
  // resetALlLedPWM();
  // delay(1000);
  // verticalTest();
  // drop();

  // studipTest();
  // noiseTest();
  // readAndProcessFile("test_processing_255.txt");
  // readAndProcessFileBinary("image_data_binary.bin");
  readAndProcessFileBinaryFade("image_data_binary.bin");
  // readAndProcessFileBinary("coucou_led_30_17.bin");
  // xfadeTest();

  // breathAllLed();
  // twelveBaseTest();
}

void IS_IIC_WriteByte(uint8_t Dev_Add, uint8_t Reg_Add, uint8_t Reg_Dat)
{
  Wire.beginTransmission(Dev_Add / 2); // transmit to device address
  Wire.write(Reg_Add);                 // sends register address
  Wire.write(Reg_Dat);                 // sends register data
  Wire.endTransmission();              // stop transmitting
}

void IS31FL3737B_init(void) // white LED
{
  uint8_t i;
  IS_IIC_WriteByte(Addr_GND_GND, 0xFE, 0xc5); // Unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND, 0xFD, 0x03); // Turn to page 3: function registers
  IS_IIC_WriteByte(Addr_GND_GND, 0x00, 0x00); // Enable software shutdown

  IS_IIC_WriteByte(Addr_GND_GND, 0xFE, 0xc5); // Unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND, 0xFD, 0x00); // Turn to page 0: control registers
  for (i = 0; i < 0x18; i = i + 1)
    IS_IIC_WriteByte(Addr_GND_GND, i, 0xff); // open all LED
  // delay(1000);
  // can use buffer write type as figure 7 in datasheet

  IS_IIC_WriteByte(Addr_GND_GND, 0xFE, 0xc5); // unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND, 0xFD, 0x01); // Turn to page 1: PWM registers
  for (i = 0; i < 192; i++)
    IS_IIC_WriteByte(Addr_GND_GND, i, 0x00); // Set PWM data to 0
  // can use buffer write type as figure 7 in datasheet

  IS_IIC_WriteByte(Addr_GND_GND, 0xFE, 0xc5);           // unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND, 0xFD, 0x03);           // Turn to page 3: function registers
  IS_IIC_WriteByte(Addr_GND_GND, 0x01, GLOABL_CURRENT); // global current
  IS_IIC_WriteByte(Addr_GND_GND, 0x00, 0x11);           // Release software shutdown to normal operation
}

void IS31FL3737B_Test_mode1(void) // white LED
{
  int led, pwd;
  int del = 10;
  // while (1)
  // {
  // IS_IIC_WriteByte(Addr_GND_GND, 0xFE, 0xc5); // unlock FDh
  // IS_IIC_WriteByte(Addr_GND_GND, 0xFD, 0x01); // Turn to page 1: PWM registers
  int max_led = 192;
  pwd = 122;
  // while (1)
  // {
  /*
  for (led = 0; led < max_led + 1; led++)
  {
    if (led == 6)
    {
      led = 8;
      IS_IIC_WriteByte(Addr_GND_GND, 5, 0); // update all PWM with 0x10
    }
    if (!led)
    {
      IS_IIC_WriteByte(Addr_GND_GND, max_led, 0); // update all PWM with 0x10
      IS_IIC_WriteByte(Addr_GND_GND, led, pwd);   // update all PWM with 0x10
    }
    if (led > 0)
      IS_IIC_WriteByte(Addr_GND_GND, led - 1, 0); // update all PWM with 0x10
    IS_IIC_WriteByte(Addr_GND_GND, led, pwd);     // update all PWM with 0x10
    delay(del);
  }
  */
  // }

  // delay(500);
  IS_IIC_WriteByte(Addr_GND_GND, 0xFE, 0xc5); // unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND, 0xFD, 0x00); // write page 0
  for (int i = 0; i < 24; i++)
    IS_IIC_WriteByte(Addr_GND_GND, i, 0xff);  // open all led
  IS_IIC_WriteByte(Addr_GND_GND, 0xFE, 0xc5); // unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND, 0xFD, 0x01); // write page 1
  IS_I2C_BufferWrite((uint8_t *)image, 192, 0, Addr_GND_GND);
  // Serial.println("end of buffer tx");
  IS_IIC_WriteByte(Addr_GND_GND, 0xFE, 0xc5); // Unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND, 0xFD, 0x03); // Turn to page 3: function registers
  // while (1)
  // ;
  // for(pwd=0;pwd<=255;pwd++)
  // {
  // for(led=0;led<14;led++)IS_IIC_WriteByte(Addr_GND_GND,led,pwd);//update all PWM with 0x10
  // led = random(192);
  // IS_IIC_WriteByte(Addr_GND_GND,led,pwd);//update all PWM with 0x10
  // delay(10);
  // if (pwd<=255)
  // pwd=0;
  // }
  // delay(1000);              // wait for a second

  // IS_IIC_WriteByte(Addr_GND_GND,0xFE,0xc5);//unlock FDh
  // IS_IIC_WriteByte(Addr_GND_GND,0xFD,0x01);//Turn to page 1: PWM registers
  // for(t=255;t>=0;t--)
  // {
  // for(i=0;i<192;i++)IS_IIC_WriteByte(Addr_GND_GND,i,t);//update all PWM with 0x10
  // }
  // delay(1000);              // wait for a second
}

void allLedPWMfull(void)
{
  IS_IIC_WriteByte(Addr_GND_GND, 0xFE, 0xc5); // unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND, 0xFD, 0x00); // write page 0
  for (int i = 0; i < 24; i++)
    IS_IIC_WriteByte(Addr_GND_GND, i, 0xFF); // open all led
  int led = 192;
  IS_IIC_WriteByte(Addr_GND_GND, 0xFE, 0xc5); // unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND, 0xFD, 0x01); // Turn to page 1: PWM registers
  while (led--)
    IS_IIC_WriteByte(Addr_GND_GND, led, 255); // update all PWM
}

void resetALlLedPWM(void)
{
  IS_IIC_WriteByte(Addr_GND_GND, 0xFE, 0xc5); // unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND, 0xFD, 0x01); // Turn to page 1: PWM registers
  for (uint8_t i = 0; i < 192; i++)
    IS_IIC_WriteByte(Addr_GND_GND, i, 0x00); // Set PWM data to 0
}

/* conversion de base 12 a base 16 */
int resolve(int led)
{
  return (led += (led / 6) * 2);
}

uint8_t pwm = 0;
void twelveBaseTest(void)
{
  int led;
  IS_IIC_WriteByte(Addr_GND_GND, 0xFE, 0xc5); // unlock FDh
  IS_IIC_WriteByte(Addr_GND_GND, 0xFD, 0x01); // Turn to page 1: PWM registers
  for (led = 0; led < 144; led++)
  {
    IS_IIC_WriteByte(Addr_GND_GND, resolve(led), pwm++); // update PWM
    if (!led)
      IS_IIC_WriteByte(Addr_GND_GND, resolve(143), 0); // update PWM
    else
      IS_IIC_WriteByte(Addr_GND_GND, resolve(led - 1), 0); // update PWM
    delay(100);
  }
}

uint8_t IS_I2C_BufferWrite(uint8_t *pBuffer, int length, int WriteAddress, int DeviceAddress)
{
  int seg = length / 32; // six segments aka 192/32 (Wire transmission = max 32 bytes)
  for (int i = 0; i < seg; i++)
  {
    Wire.beginTransmission(DeviceAddress / 2); // transmit to device address
    Wire.write(WriteAddress + (i * 32));       // sends register address
    for (int i = 32; i > 0; i--)
      Wire.write(*pBuffer++); // sends register data
    Wire.endTransmission();   // stop transmitting
  }
  return 1;
}

// void randomDur(dropTimer *drop)
// {
//   for (int i = 0; i < 12; i++)
//   {
//     drop[i].onDuration = random(200) + 200;
//     drop[i].pauseDuration = random(500) + 500;
//   }
// }

void printBuffer(uint8_t *buffer)
{
  for (int i = 0; i < 192; i++)
  {
    DPRINT("buffer[");
    DPRINT(i);
    DPRINT("]= ");
    DPRINTLN(buffer[i]);
  }
}

void printDot(matrixDot dot)
{
  DPRINT(" col=");
  DPRINT(dot.col);
  DPRINT(" row=");
  DPRINT(dot.row);
  DPRINT(" pwm=");
  DPRINTLN(dot.pwm);
}

void printDrop(dropTimer drop)
{
  DPRINT("prev=");
  DPRINT(drop.prev);
  DPRINT(" curr=");
  DPRINT(drop.current);
  DPRINT(" dur=");
  DPRINT(drop.onDuration);
  DPRINT(" pauseDur=");
  DPRINT(drop.pauseDuration);
  DPRINT(" pauseCounter=");
  DPRINT(drop.current - drop.prev);
  DPRINT(" active=");
  DPRINT(drop.active);

  // DPRINT(" pausePrev=");
  // DPRINT(drop.pausePrev);
  // DPRINT(" pauseCurrent=");
  // DPRINT(drop.pauseCurrent);
  printDot(drop.dot);
}

void bufferTest(void)
{
  uint8_t buffer[192];
  clearBuffer(buffer, 192);
  dropTimer drop[12];
  drop[0].dot.col = 1;
  drop[0].dot.row = 4;
  drop[0].dot.pwm = 255;
  writeToBuffer(buffer, drop[0].dot);
  printBuffer(buffer);
  drop[1].dot.col = 1;
  drop[1].dot.row = 1;
  drop[1].dot.pwm = 110;
  writeToBuffer(buffer, drop[1].dot);
  drop[2].dot.col = 0;
  drop[2].dot.row = 0;
  drop[2].dot.pwm = 11;
  writeToBuffer(buffer, drop[2].dot);
  printBuffer(buffer);
  while (1)
    ;
}

uint16_t randomDrop(void)
{
  return random(RANDOM_DROP_DUR) + RANDOM_DROP_MIN;
}

uint16_t randomPause(void)
{
  return random(RANDOM_PAUSE) + RANDOM_PAUSE_MIN;
}

uint8_t randomPWM(void)
{
  return random(RANDOM_MAX_PWM) + RANDOM_PWM_MIN;
}

void initDrop(dropTimer *drop)
{
  for (int i = 0; i < 12; i++)
  {
    drop[i].dot.col = i;
    drop[i].dot.row = 11;
    drop[i].dot.pwm = 0;
    drop[i].onDuration = randomDrop();
    drop[i].pauseDuration = randomPause();
    drop[i].current = millis();
    // drop[i].fadeCurrent = millis();
    drop[i].prev = random(drop[i].current);
    drop[i].fadeDuration = 4;
    // drop[i].pausePrev = millis();
    drop[i].active = 0;
    printDrop(drop[i]);
  }
}

void drop(void)
{
  uint8_t buffer[192];
  int i;
  // int j = 12;
  dropTimer drop[13]; // drop[12] creates issue with last drop, to be debugged
  initDrop(drop);
  while (1)
  {
    clearBuffer(buffer, 192);
    for (i = 0; i < 12; i++)
    {
      drop[i].current = millis();
      // drop[i].pauseCurrent = millis();
      // printDrop(drop[i]);
      if (!drop[i].active)
      {
        if (drop[i].current - drop[i].prev > drop[i].pauseDuration)
        {
          drop[i].active = 1;
          // drop[i].pausePrev = millis();
          drop[i].prev = millis();
          // drop[i].fadePrev = millis();
          drop[i].dot.pwm = randomPWM();
          drop[i].dot.row = 11;
          drop[i].pauseDuration = randomPause();
        }
      }
      else if (drop[i].current - drop[i].prev > drop[i].onDuration && drop[i].active)
      {
        drop[i].dot.row--;
        if (drop[i].dot.row == 0)
        {
          drop[i].onDuration *= 5;
          drop[i].fadePrev = millis();
        }
        drop[i].prev = millis();
        // drop[i].dot.pwm = (random(180) + 20);
        if (drop[i].dot.row < 0)
        {
          drop[i].onDuration = randomDrop();
          // drop[i].dot.pwm = 0;
          drop[i].active = 0;
        }
      }
      else if (drop[i].current - drop[i].prev < drop[i].onDuration && drop[i].dot.row == 0)
      {
        drop[i].fadeCurrent = millis();
        if (drop[i].fadeCurrent - drop[i].fadePrev > drop[i].fadeDuration)
        {
          drop[i].dot.pwm -= 1;
          drop[i].fadePrev = millis();
        }
        if (drop[i].dot.pwm == 255)
          drop[i].dot.pwm = 0;
      }
      writeToBuffer(buffer, drop[i].dot);
      // printBuffer(buffer);

      if (i == 11)
        printDrop(drop[i]);
      // if (drop[i].active)
    }
    IS_I2C_BufferWrite(buffer, 192, 0, Addr_GND_GND);
  }
}

void clearBuffer(uint8_t *buffer, int buffLength)
{
  for (uint8_t i = 0; i < buffLength; i++)
    buffer[i] = 0;
}

void writeToBuffer(uint8_t *buffer, matrixDot dot)
{
  buffer[resolve(dot.col) + resolve(dot.row * 12)] = dot.pwm;
}

void writeFadeToBuffer(uint8_t *buffer, matrixDot dot)
{
  buffer[resolve(dot.col) + resolve(dot.row * 12)] = dot.pwmFade;
  // DPRINTLN("buffer0=");
  // DPRINTLN(buffer[0]);
}

void verticalTest(void)
{
  matrixDot dot;
  uint8_t buffer[192];
  while (1)
  {
    for (int col = 0; col < 12; col++)
    {
      for (int i = 0; i < 12; i++)
      {
        dot.col = col;
        dot.row = i;
        dot.pwm = 255;
        clearBuffer(buffer, 192);
        writeToBuffer(buffer, dot);
        IS_I2C_BufferWrite(buffer, 192, 0, Addr_GND_GND);
        delay(10);
      }
    }
  }
}

void noiseTest(void)
{
  matrixDot dot;
  uint8_t buffer[192];
  while (1)
  {
    clearBuffer(buffer, 192);
    for (int col = 0; col < 12; col++)
    {
      for (int i = 0; i < 12; i++)
      {
        dot.col = col;
        dot.row = i;
        dot.pwm = random(255);
        writeToBuffer(buffer, dot);
      }
    }
    IS_I2C_BufferWrite(buffer, 192, 0, Addr_GND_GND);
    delay(100);
  }
}

void studipTest(void)
{
  matrixDot dot;
  uint8_t buffer[192];
  clearBuffer(buffer, 192);

  dot.col = 0;
  dot.row = 0;
  dot.pwm = 255;
  writeToBuffer(buffer, dot);
  IS_I2C_BufferWrite(buffer, 192, 0, Addr_GND_GND);
  while (1)
    ;
}
/*
while((0x02==G_Demo_NO))//play Love
    {
      I2C_WriteByte(Addr_GND_GND,0xFE,0xc5);//unlock FDh
      I2C_WriteByte(Addr_GND_GND,0xFD,0x00);//write page 0
      for(i=0;i<24;i++)
      I2C_WriteByte(Addr_GND_GND,i,0xff);//open all led
      I2C_WriteByte(Addr_GND_GND,0xFE,0xc5);//unlock FDh
      I2C_WriteByte(Addr_GND_GND,0xFD,0x01);//write page 1
      I2C_BufferWrite((uint8_t*)Love_pwm[g_love_NO],192,00,Addr_GND_GND);//Love_pwm[g_fireworks_NO][j];
      I2C_WriteByte(Addr_GND_GND,0xFE,0xc5);//unlock FDh
      I2C_WriteByte(Addr_GND_GND,0xFD,0x03);//write page 3

      I2C_WriteByte(Addr_GND_GND,0x00,0x01);//normal operation

      I2C_WriteByte(Addr_GND_GND,0x01,0xFF);//set current
      g_love_NO++;
        Delay_us(10000);//100ms
      if(g_love_NO==18)
      {
        g_love_NO=0;
        Delay_us(20000);
      }
*/