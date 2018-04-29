/*

* Team Id: 07

* Author List: Neeladrishekhar Kanjilal

* Filename: lab-0.c 

* Functions: setup(), led_pin_config(), main()  

* Global Variables: ui8LED

*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

uint8_t ui8LED;
/*

* Function Name: setup()

* Input: none

* Output: none

* Logic: Set crystal frequency and enable GPIO Peripherals  

* Example Call: setup();

*/
void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

/*

* Function Name: led_pin_config()

* Input: none

* Output: none

* Logic: Set PORTF Pin 1, Pin 2, Pin 3 as output. On this pin Red, Blue and Green LEDs are connected. 

* Example Call: led_pin_config();

*/
void led_pin_config(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
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

	setup();
	led_pin_config();

	while(1)
	{
		// Turn on the LED
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
		// Delay for a bit
		SysCtlDelay(20000000);
		// Cycle through Red, Green and Blue LEDs
		if (ui8LED == 8)
		{
			ui8LED = 2;
		}
		else
		{
			ui8LED = ui8LED*2;
		}
	}
}
