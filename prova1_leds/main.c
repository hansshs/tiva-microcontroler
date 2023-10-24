#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"

#define PWM_BASE_FREQ 100 //Hz
uint16_t colorIndex = 0;
uint8_t index = 0;
uint32_t intensity [] = {100, 200, 400, 600, 1};

void SysTick_Handler(void)
{
    if (!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)) // Verifica o estado do SW1 (GPIO_PIN_4) para mudar de cor
    {
        colorIndex++; // Alterna entre 10 cores
    }
    if (colorIndex == 10){colorIndex = 0;}


    if (!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)) // Verifica o estado do SW2 (GPIO_PIN_0) para mudar a intensidade
    {
       index++; // Alterna a intensidade
       PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT | PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);
    }

    if (index == 4)
    {
        PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT | PWM_OUT_6_BIT | PWM_OUT_7_BIT, false);
        index = 0;
    }
}

int main(void)
{
    uint32_t val_load, pwm_clk;
    uint16_t duty = 1;
    uint16_t colors[10][3] = {
        {100, 1, 1},    // VERMELHO
        {1, 100, 1},    // VERDE
        {1, 1, 100},    // AZUL
        {1, 100, 100},  // AZUL CLARO
        {100, 1, 100},  //AMARELO
        {100, 100, 1},  // ROSA
        {100, 100, 100},// BRANCO
        {100, 1, 20},   // LARANJA
        {50, 100, 1},   // MAGENTA
        {20, 1, 70}     // LIMA
    };

    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); //Seta o clock para 80 MHz
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);            //Set Clock PWM

    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);     //Aciona PWM1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);    //Aciona gpioF

    // Configurar PF0 e PF4 como entrada
    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_4, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_4);


    // Configurar o SysTick para interrupção
    SysTickPeriodSet(SysCtlClockGet()); // 1s
    SysTickEnable();
    SysTickIntEnable();

    //Seta os pinos PF1, PF2, PF3 como output PWM
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);

        // Info no datasheet do TIVA sobre M1PWM
    GPIOPinConfigure(GPIO_PF1_M1PWM5);
    GPIOPinConfigure(GPIO_PF2_M1PWM6);
    GPIOPinConfigure(GPIO_PF3_M1PWM7);

    pwm_clk = SysCtlClockGet() / 64;
    val_load = (pwm_clk / PWM_BASE_FREQ) - 1;       //contador pwm freq

    // Configurar gerador PWM GEN_2 para PF1 (M1PWM5)
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, val_load);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, duty * val_load/100);  // set largura pf1
    PWMGenEnable(PWM1_BASE, PWM_GEN_2);

    // Configurar gerador PWM GEN_3 para PF2 (M1PWM6) e PF3 (M1PWM7)
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, val_load);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, duty * val_load/100);  // set largura pf2
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, duty * val_load/100);  // set largura pf3
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);

    while(1)
    {
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, colors[colorIndex][0] * val_load / intensity[index]);
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, colors[colorIndex][1] * val_load / intensity[index]);
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, colors[colorIndex][2] * val_load / intensity[index]);
    }
}
