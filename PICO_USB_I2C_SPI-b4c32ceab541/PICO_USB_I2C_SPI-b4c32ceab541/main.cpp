#include "mbed.h"
#include "rtos.h"
#include "USBSerial.h"
#include "max32625pico.h"
#include "SerialInterface.h"

#define USB_MAX_RESP_LENGTH  511
#define USB_MAX_CMD_LENGTH  511

// configure VDDIOH to local 3.3V supply, set dipVio and swdVio to 1.8V supply
MAX32625PICO pico(MAX32625PICO::IOH_3V3, MAX32625PICO::VIO_1V8, MAX32625PICO::VIO_1V8);

// Virtual serial port over USB
USBSerial microUSB(0x0B6A, 0x4360);

// Serial Interfaces
I2C i2c(P1_6, P1_7);
SPI spi(P0_5, P0_6, P0_4);
DigitalInOut gpio[] = {P0_0, P0_1, P0_2, P0_3, P4_4, P4_5, P4_6, P4_7};
AnalogIn ain[] = {AIN_0, AIN_1, AIN_2, AIN_3, AIN_4, AIN_5, AIN_6, AIN_7};

// Serial Interface Adapter
SerialInterface serInt(&i2c, &spi, gpio, ain);

// Threads
Thread threadUSB;

DigitalOut rLED(LED1);
DigitalOut gLED(LED2);
DigitalOut bLED(LED3);

void usb_thread()
{
    char obuf[USB_MAX_RESP_LENGTH +1];
    char ibuf[USB_MAX_CMD_LENGTH +1];
    int i = 0;

    microUSB.printf("micro USB serial port\r\n");

    while(1) {
        if (microUSB.readable()) {
            ibuf[i]=microUSB.getc();
            if (ibuf[i]!='\r') {
                if (i < USB_MAX_CMD_LENGTH) {
                    i += 1;
                }
            } else {
                bLED = LED_ON;
                if (i < USB_MAX_CMD_LENGTH) {
                    ibuf[i]=0;
//                    microUSB.printf("UART CMD:  %s=", ibuf);
                    serInt.call(ibuf, obuf);
                    microUSB.printf("%s\r\n", obuf);
                } else {
                    microUSB.printf("[-1]\r\n");
                }
                i=0;
                bLED = LED_OFF;
            }
        }
    }
}


// main() runs in its own thread in the OS
// (note the calls to Thread::wait below for delays)
int main()
{
    rLED = LED_ON;
    gLED = LED_ON;
    bLED = LED_OFF;

//  Configure P4_4 through P4_7 for 3.3V I/O
    pico.vddioh(P4_4, MAX32625PICO::VIO_IOH);
    pico.vddioh(P4_5, MAX32625PICO::VIO_IOH);
    pico.vddioh(P4_6, MAX32625PICO::VIO_IOH);
    pico.vddioh(P4_7, MAX32625PICO::VIO_IOH);

    rLED = LED_OFF;

// Start USB serial thread
    threadUSB.start(usb_thread);

// Start main loop
    while(1) {
        Thread::wait(500);
        gLED = !gLED;
    }
}

