/**********************************************************************
* Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
**********************************************************************/

#include "mbed.h"
#include "max32630fthr.h"
#include "Adafruit_SSD1306.h"
#include "USBSerial.h"

MAX32630FTHR pegasus(MAX32630FTHR::VIO_3V3);

I2C i2c(P3_4, P3_5); // SDA, SCL

// Hardware serial port over DAPLink
Serial daplink(P2_1, P2_0);

// Virtual serial port over USB
USBSerial microUSB;

DigitalOut rLED(LED1);
DigitalOut gLED(LED2);
DigitalOut bLED(LED3);

DigitalIn aButton(P5_3, PullUp);
DigitalIn bButton(P3_3, PullUp);
DigitalIn cButton(P3_2, PullUp);

/* Analog inputs 0 and 1 have internal dividers to allow measuring 5V signals  
 * The dividers are selected by using inputs AIN_5 and AIN_5 respectively.
 * The full scale range for AIN0-3 is 1.2V
 * The full scale range for AIN4-5 is 6.0V
 */
AnalogIn ain0(AIN_4); 
AnalogIn ain1(AIN_5); 
AnalogIn ain2(AIN_2); 
AnalogIn ain3(AIN_3);


// main() runs in its own thread in the OS
// (note the calls to Thread::wait below for delays)
int main()
{
    int c;
    c = ' ';

    daplink.printf("daplink serial port\r\n");
    microUSB.printf("micro USB serial port\r\n");
    rLED = LED_ON;
    gLED = LED_ON;
    bLED = LED_OFF;


    Thread::wait(50);  // Give the supplies time to settle before initializing the display
    Adafruit_SSD1306_I2c featherOLED(i2c);  
    featherOLED.printf("%ux%u OLED Display\r\n", featherOLED.width(), featherOLED.height());
    featherOLED.printf("HelloWorld \r");
    featherOLED.display();


    while(1) {
        Thread::wait(250);
        if (microUSB.readable()) {
            c = microUSB.getc();
            microUSB.putc(c);
            daplink.putc(c);
        }
        if (daplink.readable()) {
            c = daplink.getc();
            microUSB.putc(c);
            daplink.putc(c);
        }
        rLED = aButton;
        gLED = bButton;
        bLED = cButton;
        featherOLED.clearDisplay();
        featherOLED.setTextCursor(0,0);
        featherOLED.printf("MAX32630FTHR OLED\n");
        featherOLED.printf("AIN0/1: %1.2f, %1.2f\n", (6.0f * ain0), (6.0f * ain1) );  // analog inputs 0,1
        featherOLED.printf("AIN2/3: %1.2f, %1.2f\n", (1.2f * ain2), (1.2f * ain3) );  // analog inputs 2,3
        featherOLED.printf("UART:%c  A:%d  B:%d  C:%d\n", c, aButton.read(), bButton.read(), cButton.read());  // UART and Buttons
        featherOLED.display();

    }
}

