#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

uint8_t ui8PinData=2; /*Creates an integer variable called ui8PinData and
initializes it to 2. This will be used to cycle through the three LEDs, lighting them one at
a time*/



int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //Configure the system clock to run using a 16MHz crystal on the main oscillator

    //Como os leds estao em PF1, PF2 e PF3, primeiro liberamos a porta F e dps os respectivos pinos(1,2 e 3)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    while(1)
    {
        if(ui8PinData==20) {ui8PinData=2;} else {ui8PinData++;}
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8PinData);

        SysCtlDelay(5e6);

//        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
//        SysCtlDelay(10e6);

    }
}
