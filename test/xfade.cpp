#include <Arduino.h>
// #include<Wire.h>
#include <I2C.h>
#include <Wire.h>
#include <avr/pgmspace.h>
#include <image.h>
#include <main.h>

typedef struct matrixDotFade
{
    int8_t row;
    uint8_t col;
    uint8_t pwm;
    bool ascending;
} matrixDotFade;

uint8_t buffer[192];

void xfadeDotInit(matrixDot *dot)
{
    for (int i = 0; i < 13; i++)
    {
        dot[i].col = 0;
        dot[i].row = i;
        dot[i].pwm = 0;
        // dot[i].ascending = 1;
    }
}

void dotTest(void)
{
    while (1)
    {
        for (int i = 0; i < 144; i++)
        {
            clearBuffer(buffer, 192);
            buffer[resolve(i)] = 133;
            buffer[resolve(i + 5)] = 133;
            IS_I2C_BufferWrite(buffer, 192, 0, Addr_GND_GND);
            delay(200);
        }
    }
}

void xfadeTest(void)
{
    matrixDot dot[13];
    clearBuffer(buffer, 192);
    xfadeDotInit(dot);
    int i = 0;
    int pwmStep = 10;
    while (1)
    {
        // xfadeDotInit(dot);
        for (i = 0; i < 12; i++)
        {

            // for (int x = 0; x < 255; x += 10)
            // {
            //     dot[i].pwm = x;
            //     writeToBuffer(buffer, dot[i]);
            //     // dot[i - 1].pwm = x - 127;
            //     // delay(10);
            //     // writeToBuffer(buffer, dot[i - 1]);
            //     IS_I2C_BufferWrite(buffer, 192, 0, Addr_GND_GND);
            //     clearBuffer(buffer, 192);
            // }
            for (int x = 255; x > 0; x -= pwmStep)
            {
                // int tmp = dot[i].pwm;
                dot[i].pwm = x - 10;
                if (x < dot[i].pwm)
                    dot[i].pwm = 10;
                DPRINT("x=");
                DPRINT(x);
                DPRINT(" dot[i].pwm=");
                DPRINT(dot[i].pwm);
                writeToBuffer(buffer, dot[i]);
                if (i == 11)
                {
                    int tmp = dot[0].pwm;
                    dot[0].pwm += pwmStep;
                    if (dot[0].pwm < tmp)
                        dot[0].pwm = 255;
                    writeToBuffer(buffer, dot[0]);
                }
                else
                {
                    int tmp = dot[i + 1].pwm;
                    dot[i + 1].pwm += pwmStep;
                    if (dot[i + 1].pwm < tmp)
                        dot[i + 1].pwm = 255;
                    writeToBuffer(buffer, dot[i + 1]);
                }
                DPRINT(" dot[i + 1].pwm=");
                DPRINTLN(dot[i + 1].pwm);
                IS_I2C_BufferWrite(buffer, 192, 0, Addr_GND_GND);
                clearBuffer(buffer, 192);
                // delay(5);
            }
        }

        // for (int x = 255; x > 0; i--)
        // {
        //     dot[i].pwm = x;
        //     delay(50);
        // }
    }

    // dotTest();
    // IS_I2C_BufferWrite(buffer, 192, 0, Addr_GND_GND);
}