/*
 * Based on Obdev's AVRUSB code and under the same license.
 *
 * TODO: Make a proper file header. :-)
 * Modified for Digispark by Digistump
 */
#ifndef __keyboard_h__
#define __keyboard_h__

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#include "usbdrv/usbdrv.h"
#include "usbdrv/scancode-ascii-table.h"

/* Keyboard usage values, see usb.org's HID-usage-tables document, chapter
 * 10 Keyboard/Keypad Page for more codes.
 */
#define MOD_CONTROL_LEFT    (1<<0)
#define MOD_SHIFT_LEFT      (1<<1)
#define MOD_ALT_LEFT        (1<<2)
#define MOD_GUI_LEFT        (1<<3)
#define MOD_CONTROL_RIGHT   (1<<4)
#define MOD_SHIFT_RIGHT     (1<<5)
#define MOD_ALT_RIGHT       (1<<6)
#define MOD_GUI_RIGHT       (1<<7)

#define KEY_A       4
#define KEY_B       5
#define KEY_C       6
#define KEY_D       7
#define KEY_E       8
#define KEY_F       9
#define KEY_G       10
#define KEY_H       11
#define KEY_I       12
#define KEY_J       13
#define KEY_K       14
#define KEY_L       15
#define KEY_M       16
#define KEY_N       17
#define KEY_O       18
#define KEY_P       19
#define KEY_Q       20
#define KEY_R       21
#define KEY_S       22
#define KEY_T       23
#define KEY_U       24
#define KEY_V       25
#define KEY_W       26
#define KEY_X       27
#define KEY_Y       28
#define KEY_Z       29
#define KEY_1       30
#define KEY_2       31
#define KEY_3       32
#define KEY_4       33
#define KEY_5       34
#define KEY_6       35
#define KEY_7       36
#define KEY_8       37
#define KEY_9       38
#define KEY_0       39

#define KEY_ENTER   40

#define KEY_SPACE   44

#define KEY_F1      58
#define KEY_F2      59
#define KEY_F3      60
#define KEY_F4      61
#define KEY_F5      62
#define KEY_F6      63
#define KEY_F7      64
#define KEY_F8      65
#define KEY_F9      66
#define KEY_F10     67
#define KEY_F11     68
#define KEY_F12     69

#define KEY_ARROW_LEFT 0x50


typedef uint8_t byte;
#define BUFFER_SIZE 2 // Minimum of 2: 1 for modifiers + 1 for keystroke 
uchar reportBuffer[2];    // buffer for HID reports [ 1 modifier byte + (len-1) key strokes]

void sendKeyStroke(byte keyStroke, byte modifiers) {
   	while (!usbInterruptIsReady()) {
      // Note: We wait until we can send keystroke
      //       so we know the previous keystroke was
      //       sent.
    	usbPoll();
        _delay_ms(5);
    }
    
    memset(reportBuffer, 0, sizeof(reportBuffer));
		
    reportBuffer[0] = modifiers;
    reportBuffer[1] = keyStroke;
    
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
		
  	while (!usbInterruptIsReady()) {
      // Note: We wait until we can send keystroke
      //       so we know the previous keystroke was
      //       sent.
    	usbPoll();
    	_delay_ms(5);
    }
      
    // This stops endlessly repeating keystrokes:
    memset(reportBuffer, 0, sizeof(reportBuffer));      
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
}
 
size_t write(uint8_t chr) {
    uint8_t data = pgm_read_byte_near(ascii_to_scan_code_table + (chr - 8));
    sendKeyStroke(data & 0b01111111, data >> 7 ? MOD_SHIFT_RIGHT : 0);
    return 1;
}

const PROGMEM char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = { /* USB report descriptor */
  0x05, 0x01,                    // USAGE_PAGE (Generic Desktop) 
  0x09, 0x06,                    // USAGE (Keyboard) 
  0xa1, 0x01,                    // COLLECTION (Application) 
  0x05, 0x07,                    //   USAGE_PAGE (Keyboard) 
  0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl) 
  0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI) 
  0x15, 0x00,                    //   LOGICAL_MINIMUM (0) 
  0x25, 0x01,                    //   LOGICAL_MAXIMUM (1) 
  0x75, 0x01,                    //   REPORT_SIZE (1) 
  0x95, 0x08,                    //   REPORT_COUNT (8) 
  0x81, 0x02,                    //   INPUT (Data,Var,Abs) 
  0x95, 0x01,           //   REPORT_COUNT (simultaneous keystrokes) 
  0x75, 0x08,                    //   REPORT_SIZE (8) 
  0x25, 0x65,                    //   LOGICAL_MAXIMUM (101) 
  0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated)) 
  0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application) 
  0x81, 0x00,                    //   INPUT (Data,Ary,Abs) 
  0xc0                           // END_COLLECTION 
};

uchar usbFunctionSetup(uchar data[8]) {
    usbRequest_t    *rq = (usbRequest_t *)((void *)data);

    usbMsgPtr = reportBuffer; //
    if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
      if (rq->bRequest == USBRQ_HID_GET_REPORT) {
                return USB_NO_MSG;
      } else if (rq->bRequest == USBRQ_HID_SET_REPORT) {
            return USB_NO_MSG;			
      }
    } else {
      /* no vendor specific requests implemented */
    }
		
    return 0;
}

uchar usbFunctionWrite(uchar *data, uchar len) {
    DDRB |= (1 << PB1);
    PORTB |= (1 << PB1);
    for (int i = 0; i < 10; i++) {
        PORTB ^= (1 << PB1);
        _delay_ms(500);
    }

    return 1;  
}

uchar usbFunctionRead(uchar *data, uchar len) {
    DDRB |= (1 << PB1);
    PORTB |= (1 << PB1);
    for (int i = 0; i < 10; i++) {
        PORTB ^= (1 << PB1);
        _delay_ms(500);
    }

    return 1;  
}


#endif // __keyboard_h__




