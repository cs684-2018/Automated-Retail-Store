/*

* Team Id: 07

* Author List: Neeladrishekhar Kanjilal

* Filename: lab-2.c 

* Functions: setup(), pin_config(), detectKeyPress(port, pin, index), setNextColor(), timer0IntSetup(), Timer0IntHandler(), main()  

* Global Variables: ui8LED, num_sw2_pressed, sw_state

*/
// #include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
// #include "inc/hw_ints.h"
// #include <time.h>

uint8_t ui8LED = 2;
int num_sw2_pressed = 0;
typedef enum {PIN_0_STATE_INDEX, PIN_4_STATE_INDEX} swState_i;
typedef enum {IDLE_STATE, PRESS_STATE, RELEASE_STATE} bounce_state;
bounce_state sw_state[3] = {IDLE_STATE, IDLE_STATE, IDLE_STATE};

/*

* Function Name: setup

* Input: none

* Output: none

* Logic:  Set crystal frequency,enable GPIO Peripherals and unlock Port F pin 0 (PF0)

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

* Function Name: pin_config

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

* Function Name: detectKeyPress

* Input: uint32_t, uint8_t

* Output: unsigned char{0,1}

* Logic:  Handle debounce key press by checking key press two times at a 10ms gap

* Example Call: detectKeyPress(GPIO_PORTF_BASE, GPIO_PIN_0, PIN_0_STATE_INDEX);

*/
unsigned char detectKeyPress(uint32_t port, uint8_t pin, swState_i index)
{
    unsigned char flag = 0;
    bool keyPressed = !GPIOPinRead(port, pin);

    // desciption of the switch debouncing state machine
    switch(sw_state[index]) {
        case IDLE_STATE:
            if(keyPressed){sw_state[index] = PRESS_STATE;} else {sw_state[index] = IDLE_STATE;} break;
        case PRESS_STATE:
            if(keyPressed){sw_state[index] = RELEASE_STATE; flag = 1;} else {sw_state[index] = IDLE_STATE;} break;
        case RELEASE_STATE:
            if(keyPressed){sw_state[index] = RELEASE_STATE;} else {sw_state[index] = IDLE_STATE;} break;
    }
    return flag;
}

/*

* Function Name: setNextColor

* Input: none

* Output: none

* Logic:  Set the next color in a cycle of Red then Green then Blue and repeat

* Example Call: setNextColor();

*/
void setNextColor(void)
{
    if (ui8LED == 2) {
        ui8LED = 8;
    } else {
        ui8LED = ui8LED/2;
    }
}

/*

* Function Name: timer0IntSetup

* Input: none

* Output: none

* Logic:  Set Timer0 to run at 100Hz(10ms) by enabling Timer0 Peripherals

* Example Call: timer0IntSetup();

*/
void timer0IntSetup(void)
{
    // enable Timer0 peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    
    // set timer for 100Hz at 50% duty cycle
    uint32_t ui32Period = (SysCtlClockGet() / 100) / 2;
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
    
    // enable the interrupt
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();

    // enable the timer
    TimerEnable(TIMER0_BASE, TIMER_A);
}

/*

* Function Name: Timer0IntHandler

* Input: none

* Output: none

* Logic: The intterupt handler function for Timer0 as set up in timer0IntSetup()

* Example Call: Timer0IntHandler()// although applicable;

*/
void Timer0IntHandler(void)
{
    // Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    if (detectKeyPress(GPIO_PORTF_BASE, GPIO_PIN_0, PIN_0_STATE_INDEX)) {
        num_sw2_pressed++;
    }
    if (detectKeyPress(GPIO_PORTF_BASE, GPIO_PIN_4, PIN_4_STATE_INDEX)) {
        setNextColor();
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
    }
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
    // ui8LED = 2;
    // num_sw2_pressed = 0;
    // sw_state = {IDLE_STATE, IDLE_STATE, IDLE_STATE};

    setup();
    pin_config();
    timer0IntSetup();

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);

    while(1)
    {
    }
}
