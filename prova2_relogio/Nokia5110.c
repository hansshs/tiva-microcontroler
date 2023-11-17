#include <Nokia5110.h>
#include "inc/tm4c123gh6pm.h"

typedef enum {
    LCD_COMMAND,
    LCD_DATA
} lcdMessageType_t;

// Bitmap ASCII characters
static const uint8_t ASCII[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 20
    {0x00, 0x00, 0x5f, 0x00, 0x00}, // 21 !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // 22 "
    {0x14, 0x7f, 0x14, 0x7f, 0x14}, // 23 #
    {0x24, 0x2a, 0x7f, 0x2a, 0x12}, // 24 $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // 25 %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // 26 &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // 27 '
    {0x00, 0x1c, 0x22, 0x41, 0x00}, // 28 (
    {0x00, 0x41, 0x22, 0x1c, 0x00}, // 29 )
    {0x14, 0x08, 0x3e, 0x08, 0x14}, // 2a *
    {0x08, 0x08, 0x3e, 0x08, 0x08}, // 2b +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // 2c ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // 2d -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // 2e .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // 2f /
    {0x3e, 0x51, 0x49, 0x45, 0x3e}, // 30 0
    {0x00, 0x42, 0x7f, 0x40, 0x00}, // 31 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 32 2
    {0x21, 0x41, 0x45, 0x4b, 0x31}, // 33 3
    {0x18, 0x14, 0x12, 0x7f, 0x10}, // 34 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 35 5
    {0x3c, 0x4a, 0x49, 0x49, 0x30}, // 36 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 37 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 38 8
    {0x06, 0x49, 0x49, 0x29, 0x1e}, // 39 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // 3a :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // 3b ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, // 3c <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // 3d =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // 3e >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // 3f ?
    {0x32, 0x49, 0x79, 0x41, 0x3e}, // 40 @
    {0x7e, 0x11, 0x11, 0x11, 0x7e}, // 41 A
    {0x7f, 0x49, 0x49, 0x49, 0x36}, // 42 B
    {0x3e, 0x41, 0x41, 0x41, 0x22}, // 43 C
    {0x7f, 0x41, 0x41, 0x22, 0x1c}, // 44 D
    {0x7f, 0x49, 0x49, 0x49, 0x41}, // 45 E
    {0x7f, 0x09, 0x09, 0x09, 0x01}, // 46 F
    {0x3e, 0x41, 0x49, 0x49, 0x7a}, // 47 G
    {0x7f, 0x08, 0x08, 0x08, 0x7f}, // 48 H
    {0x00, 0x41, 0x7f, 0x41, 0x00}, // 49 I
    {0x20, 0x40, 0x41, 0x3f, 0x01}, // 4a J
    {0x7f, 0x08, 0x14, 0x22, 0x41}, // 4b K
    {0x7f, 0x40, 0x40, 0x40, 0x40}, // 4c L
    {0x7f, 0x02, 0x0c, 0x02, 0x7f}, // 4d M
    {0x7f, 0x04, 0x08, 0x10, 0x7f}, // 4e N
    {0x3e, 0x41, 0x41, 0x41, 0x3e}, // 4f O
    {0x7f, 0x09, 0x09, 0x09, 0x06}, // 50 P
    {0x3e, 0x41, 0x51, 0x21, 0x5e}, // 51 Q
    {0x7f, 0x09, 0x19, 0x29, 0x46}, // 52 R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // 53 S
    {0x01, 0x01, 0x7f, 0x01, 0x01}, // 54 T
    {0x3f, 0x40, 0x40, 0x40, 0x3f}, // 55 U
    {0x1f, 0x20, 0x40, 0x20, 0x1f}, // 56 V
    {0x3f, 0x40, 0x38, 0x40, 0x3f}, // 57 W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 58 X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // 59 Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // 5a Z
    {0x00, 0x7f, 0x41, 0x41, 0x00}, // 5b [
    {0x02, 0x04, 0x08, 0x10, 0x20}, // 5c '\'
    {0x00, 0x41, 0x41, 0x7f, 0x00}, // 5d ]
    {0x04, 0x02, 0x01, 0x02, 0x04}, // 5e ^
    {0x40, 0x40, 0x40, 0x40, 0x40}, // 5f _
    {0x00, 0x01, 0x02, 0x04, 0x00}, // 60 `
    {0x20, 0x54, 0x54, 0x54, 0x78}, // 61 a
    {0x7f, 0x48, 0x44, 0x44, 0x38}, // 62 b
    {0x38, 0x44, 0x44, 0x44, 0x20}, // 63 c
    {0x38, 0x44, 0x44, 0x48, 0x7f}, // 64 d
    {0x38, 0x54, 0x54, 0x54, 0x18}, // 65 e
    {0x08, 0x7e, 0x09, 0x01, 0x02}, // 66 f
    {0x0c, 0x52, 0x52, 0x52, 0x3e}, // 67 g
    {0x7f, 0x08, 0x04, 0x04, 0x78}, // 68 h
    {0x00, 0x44, 0x7d, 0x40, 0x00}, // 69 i
    {0x20, 0x40, 0x44, 0x3d, 0x00}, // 6a j
    {0x7f, 0x10, 0x28, 0x44, 0x00}, // 6b k
    {0x00, 0x41, 0x7f, 0x40, 0x00}, // 6c l
    {0x7c, 0x04, 0x18, 0x04, 0x78}, // 6d m
    {0x7c, 0x08, 0x04, 0x04, 0x78}, // 6e n
    {0x38, 0x44, 0x44, 0x44, 0x38}, // 6f o
    {0x7c, 0x14, 0x14, 0x14, 0x08}, // 70 p
    {0x08, 0x14, 0x14, 0x18, 0x7c}, // 71 q
    {0x7c, 0x08, 0x04, 0x04, 0x08}, // 72 r
    {0x48, 0x54, 0x54, 0x54, 0x20}, // 73 s
    {0x04, 0x3f, 0x44, 0x40, 0x20}, // 74 t
    {0x3c, 0x40, 0x40, 0x20, 0x7c}, // 75 u
    {0x1c, 0x20, 0x40, 0x20, 0x1c}, // 76 v
    {0x3c, 0x40, 0x30, 0x40, 0x3c}, // 77 w
    {0x44, 0x28, 0x10, 0x28, 0x44}, // 78 x
    {0x0c, 0x50, 0x50, 0x50, 0x3c}, // 79 y
    {0x44, 0x64, 0x54, 0x4c, 0x44}, // 7a z
    {0x00, 0x08, 0x36, 0x41, 0x00}, // 7b {
    {0x00, 0x00, 0x7f, 0x00, 0x00}, // 7c |
    {0x00, 0x41, 0x36, 0x08, 0x00}, // 7d }
    {0x10, 0x08, 0x08, 0x10, 0x08}, // 7e ~
    {0x78, 0x46, 0x41, 0x46, 0x78}, // 7f DEL
    {0x1f, 0x24, 0x7c, 0x24, 0x1f}, // 7f UT sign
    {0x00, 0x06, 0x09, 0x09, 0x06}, // 80 celsius
};

// Buffer that stores the next image to be printed on the screen
static uint8_t screen_buffer[LCD_WIDTH * LCD_HEIGHT / 8];

static void lcdSend(lcdMessageType_t type, uint8_t data) {
    if (type == LCD_COMMAND) {
        while (SSIBusy(SSI0_BASE));
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, 0);
        SSIDataPut(SSI0_BASE, data);
        while (SSIBusy(SSI0_BASE));
    } else {
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_PIN_6);
        SSIDataPut(SSI0_BASE, data);
    }
}

void lcdInit(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA) || !SysCtlPeripheralReady(SYSCTL_PERIPH_SSI0));

    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5);

    SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 4000000, 8);
    SSIEnable(SSI0_BASE);

    // Reset the LCD to a known state
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_PIN_7);
    //[DB7|DB6|DB5|DB4|DB3|DB2|DB1|DB0]
    // Chip active (PD = 0); horizontal addressing mode (V = 0); use extended instruction set (H = 1)
    lcdSend(LCD_COMMAND, 0x21); // [0|0|1|0|0|0|0|1]
    // Set LCD Vop (contrast), which may require some tweaking (Eq. 2 on datasheet):
    // Try 0xB1 (for 3.3V red SparkFun), 0xB8 (for 3.3V blue SparkFun), 0xBF if your display is too dark, or 0x80 to 0xFF if experimenting
    lcdSend(LCD_COMMAND, 0xB8); // [1|0|1|1|1|0|0|0]
    // Set temp coefficient 0 (TC1 = 0 | TC0 = 0)
    lcdSend(LCD_COMMAND, 0x04); // [0|0|0|0|0|1|0|0]
    // LCD bias mode: try 0x13 (1:48) or 0x14 (1:40/1:34)
    lcdSend(LCD_COMMAND, 0x13); // [0|0|0|1|0|0|1|1]

    // We must send 0x20 before modifying the display control mode
    lcdSend(LCD_COMMAND, 0x20); // [0|0|1|0|0|0|0|0]
    // Set display control to normal mode: 0x0D for inverse
    lcdSend(LCD_COMMAND, 0x0C); // [0|0|0|0|1|1|0|0]

    // Ensure the buffer is empty
    lcdBufferClear();
    // Ensure the lcd is empty
    lcdClear();
}

// The character will be printed at the current cursor position, the cursor
// will automatically be updated, and it will wrap to the next row or back
// to the top if necessary. One blank column of pixels will be printed on
// either side of the character for readability.
void lcdWriteChar(char c) {
    lcdSend(LCD_DATA, 0x00);
    for (int i = 0; i < 5; i++)
        lcdSend(LCD_DATA, ASCII[c - 0x20][i]);
    lcdSend(LCD_DATA, 0x00);
}

// The string will automatically wrap, so padding spaces may
// be needed to make the output look optimal.
void lcdWriteString(const char *str) {
    while (*str)
        lcdWriteChar(*str++);
}

// Output a 16-bit number in unsigned decimal format with a
// fixed size of five right-justified digits of output.
void lcdWriteUnsignedInt16(uint16_t n) {
    if (n < 10) {
        //lcdWriteString("    ");
        lcdWriteChar(n + '0');
    } else if(n < 100) {
        //lcdWriteString("    ");
        lcdWriteChar(n / 10 + '0');
        lcdWriteChar(n % 10 + '0');
    } else if(n < 1000) {
        //lcdWriteString("  ");
        lcdWriteChar(n / 100 + '0'); n = n % 100;
        lcdWriteChar(n / 10 + '0');
        lcdWriteChar(n % 10 + '0');
    } else if(n < 10000) {
        lcdWriteChar(' ');
        lcdWriteChar(n / 1000 + '0'); n = n % 1000;
        lcdWriteChar(n / 100 + '0'); n = n % 100;
        lcdWriteChar(n / 10 + '0');
        lcdWriteChar(n % 10 + '0');
    } else {
        lcdWriteChar(n / 10000 + '0'); n = n % 10000;
        lcdWriteChar(n / 1000 + '0'); n = n % 1000;
        lcdWriteChar(n / 100 + '0'); n = n % 100;
        lcdWriteChar(n / 10 + '0');
        lcdWriteChar(n % 10 + '0');
    }
}

void lcdSetCursor(uint8_t x, uint8_t y) {
    if (x >= LCD_WIDTH / LCD_CHAR_WIDTH || y >= LCD_HEIGHT / LCD_CHAR_HEIGHT)
        return;
    lcdSend(LCD_COMMAND, 0x80 | (x * LCD_CHAR_WIDTH));
    lcdSend(LCD_COMMAND, 0x40 | y);
}

void lcdClear(void) {
    lcdSetCursor(0, 0);
    for (int i = 0; i < (LCD_WIDTH * LCD_HEIGHT / 8); i++)
        lcdSend(LCD_DATA, 0x00);
}

// Draws a 84x48 bitmap image
void lcdDrawFull(const uint8_t *data) {
    lcdSetCursor(0, 0);
    for (int i = 0; i < (LCD_WIDTH * LCD_HEIGHT / 8); i++)
        lcdSend(LCD_DATA, data[i]);
}

void lcdDraw(const uint8_t *data, uint8_t x, uint8_t y, uint8_t rows, uint8_t columns) {
    for (int r = 0; r < rows; r++) {
        lcdSetCursor(x, y + r);
        for (int c = 0; c < columns; c++) {
            lcdSend(LCD_DATA, data[r * columns + c]);
        }
    }
}

void lcdBufferClear(void) {
    for (int i = 0; i < LCD_WIDTH * LCD_HEIGHT / 8; i++)
        screen_buffer[i] = 0;
}

void lcdBufferDisplay(void) {
    lcdDrawFull(screen_buffer);
}

void lcdBufferClearPixel(uint16_t x, uint16_t y) {
    screen_buffer[LCD_WIDTH * (y >> 3) + x] &= ~(1 << (y & 7));
}

void lcdBufferSetPixel(uint16_t x, uint16_t y) {
    screen_buffer[LCD_WIDTH * (y >> 3) + x] |= (1 << (y & 7));
}

bool lcdIsPixelOn(uint16_t x, uint16_t y) {
    return ((screen_buffer[LCD_WIDTH * (y >> 3) + x] & (1 << (y & 7))) == (1 << (y & 7)));
}

void startSSI0()//This one works
{
      volatile unsigned long delay;
      SYSCTL_RCGC2_R |= 0x00000001;         //  activate clock for Port A
      SYSCTL_RCGCSSI_R|=SYSCTL_RCGCSSI_R0;              //  activate clock for SSI0
      delay = SYSCTL_RCGC2_R;               //  allow time for clock to stabilize
      GPIO_PORTA_DIR_R |= 0xD0;             // make PA4,6,7 out
      GPIO_PORTA_AFSEL_R |= 0x2C;           // enable alt funct on PA2,3,5
      GPIO_PORTA_AFSEL_R &= ~0xC0;          // disable alt funct on PA6,7
      GPIO_PORTA_DEN_R |= 0xFC;             // enable digital I/O on PA2,3,4,5,6,7
                                            // configure PA2,3,5 as SSI
      GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFF0F00FF)+0x00202200;
                                            // configure PA6,7 as GPIO
      GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0x00FFFFFF)+0x00000000;
      GPIO_PORTA_AMSEL_R &= ~0xFC;          // disable analog functionality on PA2,3,4,5,6,7
      SSI0_CR1_R&=~SSI_CR1_SSE;       // 3)Disable SSI while configuring it
      SSI0_CR1_R&=~SSI_CR1_MS;        // 4) Set as Master
      SSI0_CC_R|=SSI_CC_CS_M; // 5) Configure clock source
      SSI0_CC_R|=SSI_CC_CS_SYSPLL; // 5) Configure clock source
      SSI0_CC_R|=SSI_CPSR_CPSDVSR_M;// 6) Configure prescale divisor
      SSI0_CPSR_R = (SSI0_CPSR_R&~SSI_CPSR_CPSDVSR_M)+2; // must be even number
      SSI0_CR0_R |=0x7000;
      SSI0_CR0_R &= ~(SSI_CR0_SPH | SSI_CR0_SPO);
      //SSI0_CPSR_R = (SSI0_CPSR_R&~SSI_CR0_SCR_M)+7; // must be even number
      SSI0_CR0_R = (SSI0_CR0_R&~SSI_CR0_FRF_M)+SSI_CR0_FRF_MOTO;
                                            // DSS = 8-bit data
      SSI0_CR0_R = (SSI0_CR0_R&~SSI_CR0_DSS_M)+SSI_CR0_DSS_8;
      SSI0_CR1_R|=SSI_CR1_SSE;        // 3)Enable SSI
      RESET = RESET_LOW;                    // reset the LCD to a known state
      for(delay=0; delay<10; delay=delay+1);// delay minimum 100 ns
      RESET = RESET_HIGH;                   // negative logic

}
void enable_backlight(void){GPIO_PORTA_DATA_R&=~0x10;}

