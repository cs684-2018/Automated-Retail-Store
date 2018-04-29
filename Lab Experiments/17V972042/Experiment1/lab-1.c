/*

* Team Id: 07

* Author List: Neeladrishekhar Kanjilal

* Filename: lab-1.c

* Functions: setup(), config(), main()

* Global Variables: sw2Status, ui8LED

*/

int sw2Status;

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_ints.h"
#include <time.h>


uint8_t ui8LED;
/*

* Function Name: setup()

* Input: none

* Output: none

* Logic: Set crystal frequency,enable GPIO Peripherals and unlock Port F pin 0 (PF0)

* Example Call: setup();

*/
void setup(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    
	//unlock PF0 based on requirement
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK)= GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01 ;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK)= 0 ;
}

/*

* Function Name: pin_config()

* Input: none

* Output: none

* Logic: Set Port F Pin 1, Pin 2, Pin 3 as output. On this pin Red, Blue and Green LEDs are connected.
		 Set Port F Pin 0 and 4 as input, enable pull up on both these pins.

* Example Call: pin_config();

*/

void pin_config(void)
{
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

/*

* Function Name: main()

* Input: none

* Output: none

* Logic: main function of the code to handle the lab experiment. 

* Example Call: N.A ;

*/
int main(void)
{
    ui8LED = 2;
    bool pressing_sw1 = false;
    bool pressing_sw2 = false;
    uint32_t factor = 1;
    uint32_t cycle = 0;
    bool showC = false;
    setup();
    pin_config();

    while(1)
    {
		/*
			Refer to helper document for writing logic for different parts of the lab-1 problem statement
		
		*/
        // SysCtlDelay(6700000/5);
        if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)) {
            if (!pressing_sw2) {
                sw2Status += 1;
                if (ui8LED == 2) {
                    ui8LED = 8;
                } else {
                    ui8LED = ui8LED/2;
                }
            }
            pressing_sw2 = true;
        } else {pressing_sw2 = false;}
        if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)) {
            if (!pressing_sw1) {
                if (factor == 4) {
                    factor = 1;
                } else {
                    factor = factor*2;
                }
                if (cycle > factor) {
                    cycle = factor;
                }
            }
            // GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
            pressing_sw1 = true;
        } else {
            // GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
            // if (pressing_sw1) {
            //     if (ui8LED == 2) {
            //         ui8LED = 8;
            //     } else {
            //         ui8LED = ui8LED/2;
            //     }
            // }
            pressing_sw1 = false;
        }
        if (cycle == factor) {
            cycle = 0;
            if (showC) {
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
                showC = false;
            } else {
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
                showC = true;
            }
        }        
        cycle += 1;
        SysCtlDelay(6700000);
    }
}
