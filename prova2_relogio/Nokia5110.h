// ######################################################### //
//                                                           //
// ------------------------- PINOS ------------------------- //
//                                                           //
//             --------------------------------              //
//             |        Blue Nokia 5110       |              //
//             --------------------------------              //
//             |    DISPLAY    |     TIVA     |              //
//             --------------------------------              //
//             |      RST     <->     PA7     |              //
//             |      CE      <->     PA3     |              //
//             |      DC      <->     PA6     |              //
//             |      DIN     <->     PA5     |              //
//             |      CLK     <->     PA2     |              //
//             |      VCC     <->    +3.3V    |              //
//             |     LIGHT    <->      -      |              //
//             |      GND     <->     GND     |              //
//             --------------------------------              //
//                                                           //
// ######################################################### //

#ifndef NOKIA5110_H
#define NOKIA5110_H

#include <stdbool.h>
#include <stdint.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/ssi.h>
#include <driverlib/sysctl.h>
#include <inc/hw_memmap.h>
#include <inc/tm4c123gh6pm.h>

#define LCD_WIDTH       84
#define LCD_HEIGHT      48
#define LCD_CHAR_WIDTH  7
#define LCD_CHAR_HEIGHT 8

#define RESET (*((volatile unsigned long *)0x40004200)) //For SSI0
#define RESET_LOW               0
#define RESET_HIGH              0x80 //For SSI0

void lcdInit(void);

void lcdWriteChar(char c);
void lcdWriteString(const char *str);
void lcdWriteUnsignedInt16(uint16_t n);

void lcdClear(void);
void lcdSetCursor(uint8_t x, uint8_t y);
void lcdDraw(const uint8_t *data, uint8_t x, uint8_t y, uint8_t rows, uint8_t columns);
void lcdDrawFull(const uint8_t *data);

void lcdBufferClear(void);
void lcdBufferDisplay(void);
void lcdBufferClearPixel(uint16_t x, uint16_t y);
void lcdBufferSetPixel(uint16_t x, uint16_t y);
bool lcdIsPixelOn(uint16_t x, uint16_t y);

void enable_backlight(void);
void startSSI0();

#endif
