#include "Nokia5110.h"
#include "driverlib/ssi.h"
#include "inc/hw_memmap.h"
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

uint8_t hour = 0, minute = 0, second = 0;

//Este handler, recebe os chars e executa rotina para armazenar na forma de tres variaveis hour, minute, second
void UARTIntHandler(void)
{
    static uint8_t currentField = 0;  // 0 - Hour, 1 - Minute, 2 - Second, 3- Reset

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
            currentField++;  // Muda para o próximo campo ao encontrar ":"
        }
        else
        {
            // Converte o caractere para um número
            uint8_t digit = receivedChar - '0';

            // Atualiza o campo apropriado do relógio
            switch (currentField)
            {
            case 0:  // Hour
                //if (hour >=24){hour =0;}
                hour = hour * 10 + digit;
                break;
            case 1:  // Minute
                //if (minute >=60){minute =0;}
                minute = minute * 10 + digit;
                break;
            case 2:  // Second
                //if (second >=60){second =0;}
                second = second * 10 + digit;
                break;
            case 3:
                hour = 0;
                minute = 0;
                second = 0;
                currentField = 0;
            }
        }
    }
}

void updateClock(uint8_t *hour, uint8_t *minute, uint8_t *second)
//void updateClock(uint16_t *hour, uint16_t *minute, uint16_t *second)
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

//void Timer0IntHandler(void)
//{
//    // Limpa a bandeira de interrupção do temporizador
//    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
//
//    updateClock(&hour, &minute, &second);
//
//    lcdInit();
//    lcdSetCursor(3, 2);
//    lcdWriteUnsignedInt16(hour);
//    lcdWriteString(":");
//    lcdWriteUnsignedInt16(minute);
//    lcdWriteString(":");
//    lcdWriteUnsignedInt16(second);
//
//    lcdClear();
//}
int main(void)
{
    //Configs Gerais
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
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

//    // Configs Timer
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
//    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); // Configuração do Timer0 para contar até a frequência do clock do sistema
//    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet());
//    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0IntHandler);// Registra a função de tratamento de interrupção
//    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // Habilita a interrupção do temporizador quando o valor do temporizador atinge 0
//    TimerEnable(TIMER0_BASE, TIMER_A);// Habilita o temporizador


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


    while(1)
    {
        updateClock(&hour, &minute, &second);
        lcdSetCursor(3, 2);

        lcdWriteUnsignedInt16(hour);
        lcdWriteString(":");
        lcdWriteUnsignedInt16(minute);
        lcdWriteString(":");
        lcdWriteUnsignedInt16(second);

        SysCtlDelay(SysCtlClockGet()/4);
        lcdClear();
    }

}
