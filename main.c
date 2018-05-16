#include <avr/wdt.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include "usbdrv/keyboard.h"


int main() {
    uchar   i;
    wdt_enable(WDTO_1S);
    DDRB = 0x00;
    PORTB |= (1<<0);

    usbInit();
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
    i = 0;
    while(--i){             /* fake USB disconnect for > 250 ms */
        wdt_reset();
        _delay_ms(1);
    }
    usbDeviceConnect();
    sei();
    for(;;){                /* main event loop */
        if (PINB == 0x00) {
            sendKeyStroke(KEY_F1, 0);
        }   
        wdt_reset();
        usbPoll();
    }
    return 0;
/*    memset(reportBuffer, 0, sizeof(reportBuffer));      
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));*/
}

