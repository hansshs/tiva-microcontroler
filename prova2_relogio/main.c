#include "Nokia5110.h"
#include "driverlib/ssi.h"
#include "inc/hw_memmap.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

//************************* CONSTS *****************************
#define NUM_HORAS 12
#define NUM_MIN 60
#define NUM_SEC 60
uint8_t hour = 12, minute = 58, second = 50; //Armistice WW1 -> "at the eleventh hour of the eleventh day of the eleventh month�the guns fell silent"
uint32_t ui32Period;
bool flipflop = true;

//************************* UART HANDLER *****************************
//Este handler, recebe os chars e executa rotina para armazenar na forma de tres variaveis hour, minute, second
void UARTIntHandler(void)
{
    static uint8_t currentField = 0;  // 0 - Hour, 1 - Minute, 2 - Second


    uint32_t ui32Status;
    ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status
    UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts

    while(UARTCharsAvail(UART0_BASE))
    {
        UARTCharPutNonBlocking(UART0_BASE, UARTCharGetNonBlocking(UART0_BASE)); //echo character
        SysCtlDelay(SysCtlClockGet() / (1000 * 3)); //delay ~1 msec

        char receivedChar = UARTCharGet(UART0_BASE);

        if (receivedChar == ':')
        {
            currentField++;  // Muda para o pr�ximo campo ao encontrar ":"
        }

        if (receivedChar == '-') { //Variavel de Reset
            hour   = 0;
            minute = 0;
            second = 0;
            currentField = 0;
        }
        else
        {
            // Converte o caractere para um n�mero
            uint8_t digit = receivedChar - '0';

            // Atualiza o campo apropriado do rel�gio
            switch (currentField)
            {
            case 0:  // Hour
                if (hour >=10){hour =0;}
                hour = hour * 10 + digit;
                break;
            case 1:  // Minute
                if (minute >=10){minute =0;}
                minute = minute * 10 + digit;
                break;
            case 2:  // Second
                if (second >=10){second =0;}
                second = second * 10 + digit;
                break;
            }
        }
    }
}

//************************* OPERAR RELOGIO *****************************

void updateClock(uint8_t *hour, uint8_t *minute, uint8_t *second)
{
    // Incrementa o segundo
    (*second)++;

    // Verifica se os limites foram ultrapassados e ajusta
    if (*second >= 60)
    {
        *second = 0;
        (*minute)++;

        if (*minute >= 60)
        {
            *minute = 0;
            (*hour)++;

            if (*hour >= 24)
            {
                *hour = 0;
            }
        }
    }
}

//************************* DIGITAL DISPLAY *****************************
void DigitalDisplay (void){
    updateClock(&hour, &minute, &second);

    lcdSetCursor(2, 2);
    lcdWriteUnsignedInt16(hour);
    lcdWriteString(":");
    lcdWriteUnsignedInt16(minute);
    lcdWriteString(":");
    lcdWriteUnsignedInt16(second);
}

//************************* ANALOG DISPLAY *****************************
void draw_bars(int altura, int width, int starti, int startj){
    for (int i =starti; i<altura; i++){
        int xPos = i*width;
        for (int j = 0; j<altura; j++){
            lcdBufferSetPixel(xPos, j);
        }
    }
    for(int j = startj; j<60; j++){
        lcdBufferSetPixel(j, startj);
        lcdBufferSetPixel(j, altura-1);
    }

}

void draw_box (int start, int end){
    for (int k = start; k < end; k++){
        lcdBufferSetPixel(0, k);
        lcdBufferSetPixel(60, k);
    }

    for(int g = 0; g<60; g++){
        lcdBufferSetPixel(g, start);
        lcdBufferSetPixel(g, end -1);
    }
}

void draw_lines(int start, int end, int startX, int endX) {
    for (int x = startX; x < endX; x++) {
        for (int k = start; k < end; k++) {
            lcdBufferSetPixel(x, k);
        }
    }
}

void erase_lines(int start, int end, int startX, int endX) {
    for (int x = startX; x < endX; x++) {
        for (int k = start; k < end; k++) {
            lcdBufferClearPixel(x, k);
        }
    }
}
int pos_horas[NUM_HORAS][4];
int pos_min[NUM_MIN][2];
int pos_sec [NUM_SEC][2];
void preencherPosHoras(int pos_horas[][4]) {
    for (int i = 0; i < NUM_HORAS; i++) {
        pos_horas[i][0] = 1;
        pos_horas[i][1] = 12;
        pos_horas[i][2] = i * 5 + 1;
        pos_horas[i][3] = i * 5 + 5;
        }
}

void preencherMS(void){
    for (int i = 0; i < NUM_MIN; i++){
        pos_min[i][0] = i;
        pos_sec[i][0] = i;

        pos_min[i][1] = i+1;
        pos_sec[i][1] = i+1;
    }

}

void AnalogDisplay (void) {
    int nhour;
    draw_bars(13, 5, 0, 0);
    draw_box(17,30);
    draw_box(34,48);
    updateClock(&hour, &minute, &second);


    if (hour > 12){
        nhour = hour - 12;} //transforma 24h para 12h
    else {
        nhour = hour;}

    draw_lines(1,12,pos_horas[nhour-1][2], pos_horas[nhour-1][3]);

    for (int i = minute; i>=0; i--){
        draw_lines(18,29, pos_min[i][0], pos_min[i][1]);
    }

    for (int i = second; i>=0; i--){
        draw_lines(35,47, pos_sec[i][0], pos_sec[i][1]);
    }

    lcdBufferDisplay();
    if (minute == 59){
        for (int i = 0; i<60; i++){
            erase_lines(17, 30, pos_min[i][0],pos_min[i][1]);
            erase_lines(1,12,pos_horas[nhour-1][2], pos_horas[nhour-1][3]);
        }

    }

    if (second == 59){
        for (int i = 0; i<60; i++){
            erase_lines(34, 48, pos_sec[i][0],pos_sec[i][1]);
        }

    }

}

//************************* TIMER HANDLER *****************************
int secondsCounter = 0;
void Timer0IntHandler(void)
{
    // Limpa a bandeira de interrup��o do temporizador
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    if (secondsCounter % 10 == 0) {
            flipflop = !flipflop;
    }

            // Alterna entre os modos anal�gico e digital a cada 10 segundos
    if (flipflop) {
        AnalogDisplay();
    } else {
        lcdBufferClear();
        lcdBufferDisplay();
        DigitalDisplay();
    }

        // Atualiza o contador de segundos
        secondsCounter++;

        // Certifique-se de reiniciar o contador de segundos ap�s 60 para evitar estouro
        if (secondsCounter >= 20) {
            secondsCounter = 0;
        }
}

//************************* MAIN *****************************
int main(void)
{
    //Configs Gerais
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Configs UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    IntMasterEnable(); //enable processor interrupts
    IntEnable(INT_UART0); //enable the UART interrupt
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and TX interrupts

    // Configs Timer
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    ui32Period = (SysCtlClockGet() / 1); // Periodo 1 segundo
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();
    TimerEnable(TIMER0_BASE, TIMER_A);

    UARTCharPut(UART0_BASE, 'E');
    UARTCharPut(UART0_BASE, 'n');
    UARTCharPut(UART0_BASE, 't');
    UARTCharPut(UART0_BASE, 'e');
    UARTCharPut(UART0_BASE, 'r');
    UARTCharPut(UART0_BASE, ' ');
    UARTCharPut(UART0_BASE, 'T');
    UARTCharPut(UART0_BASE, 'i');
    UARTCharPut(UART0_BASE, 'm');
    UARTCharPut(UART0_BASE, 'e');
    UARTCharPut(UART0_BASE, ':');
    UARTCharPut(UART0_BASE, ' ');

    //Configs Display
    lcdInit();
    preencherPosHoras(pos_horas);
    preencherMS();


    while(1)
    {
//        DigitalDisplay();
//        AnalogDisplay();
    }

}
