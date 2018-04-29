/*

* Team Id: 07

* Author List: Neeladrishekhar Kanjilal

* Filename: lab-3.c 

* Functions: setup(), pin_config(), detectKeyPress(port, pin, index), setNextColor(), setPulseWidth(), timer0IntSetup(), Timer0IntHandler(), main()  

* Global Variables: sw_state, swNumRelease, timer0IntFreq, ui32Load, colorWheelTheta, dTheta, ui8AdjustR, ui8AdjustG, ui8AdjustB, ui8Adjust, currMode, mayBeModeManualBlue

*/

/*
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"

#define PWM_FREQUENCY 55

uint32_t ui32Load;
uint32_t ui32PWMClock;
uint8_t ui8Adjust;

int main(void)
{
	ui8Adjust = 83;
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	// GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
	// GPIOPinConfigure(GPIO_PD0_M1PWM0);
	GPIOPinConfigure(GPIO_PF1_M1PWM5);

	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	ui32PWMClock = SysCtlClockGet() / 64;
	ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;

	// PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
	// PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, ui32Load);
	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);

	// PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
	// PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);
	// PWMGenEnable(PWM1_BASE, PWM_GEN_0);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_2);

	while(1)
	{
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
		{
			ui8Adjust--;
			if (ui8Adjust < 10)
			{
				ui8Adjust = 10;
			}
			// PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust * ui32Load / 1000);
		}
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
		{
			ui8Adjust++;
			if (ui8Adjust > 250)
			{
				ui8Adjust = 250;
			}
			// PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust * ui32Load / 1000);
		}
		SysCtlDelay(100000);
	}
}
*/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"

#define PWM_FREQUENCY 50

typedef enum {PIN_0_STATE_INDEX, PIN_4_STATE_INDEX} swState_i;
typedef enum {IDLE_STATE, PRESS_STATE, RELEASE_STATE} bounce_state;
typedef enum {AUTO, MANUAL_R, MANUAL_G, MANUAL_B, SERVO} mode;
bounce_state sw_state[3] = {IDLE_STATE, IDLE_STATE, IDLE_STATE};
uint32_t swNumRelease[3] = {0, 0, 0};

uint32_t timer0IntFreq = 100; // 100Hz

uint32_t ui32Load;
// uint32_t ui32PWMClock;
uint32_t colorWheelTheta = 0;	// 360*4=1440 degrees make one circle
uint32_t dTheta = 200; 			// 25 degrees per sec (25*4)
								// .. better be a multiple of timer0IntFreq AND never less than it
uint8_t ui8AdjustR = 250;
// uint8_t ui8AdjustR = 10;
uint8_t ui8AdjustG = 10;
uint8_t ui8AdjustB = 10;
uint8_t ui8Adjust = 75;

// mode currMode = AUTO;
mode currMode = SERVO;
bool mayBeModeManualBlue = false;

/*

* Function Name: setup

* Input: none

* Output: none

* Logic:  Set crystal frequency,enable GPIO Peripherals and unlock Port F pin 0 (PF0)

* Example Call: setup();

*/
void setup(void) {
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
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
void pin_config(void) {

	// GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	GPIOPinConfigure(GPIO_PF1_M1PWM5);
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
	GPIOPinConfigure(GPIO_PF3_M1PWM7);
	GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
	GPIOPinConfigure(GPIO_PD0_M1PWM0);

	// GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	uint32_t ui32PWMClock = SysCtlClockGet() / 64;
	ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
	PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, ui32Load);
	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);
	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);

	PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);
	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
	PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);
	PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_0);
	PWMGenEnable(PWM1_BASE, PWM_GEN_2);
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);
}

/*

* Function Name: detectKeyPress

* Input: uint32_t, uint8_t

* Output: unsigned char{0,1}

* Logic:  Handle debounce key press by checking key press two times at a 10ms gap

* Example Call: detectKeyPress(GPIO_PORTF_BASE, GPIO_PIN_0, PIN_0_STATE_INDEX);

*/
unsigned char detectKeyPress(uint32_t port, uint8_t pin, swState_i index) {
	unsigned char flag = 0;
	bool keyPressed = !GPIOPinRead(port, pin);

	// desciption of the switch debouncing state machine
	switch(sw_state[index]) {
		case IDLE_STATE:
			if(keyPressed){sw_state[index] = PRESS_STATE;} else {sw_state[index] = IDLE_STATE;} break;
		case PRESS_STATE:
			if(keyPressed){sw_state[index] = RELEASE_STATE; flag = 1;} else {sw_state[index] = IDLE_STATE;} break;
		case RELEASE_STATE:
			if(keyPressed){sw_state[index] = RELEASE_STATE; swNumRelease[index]+=1; if(swNumRelease[index] > 100){flag=2;}} else {sw_state[index] = IDLE_STATE; swNumRelease[index]=0;} break;
	}
	return flag;
}

/*

* Function Name: timer0IntSetup

* Input: none

* Output: none

* Logic:  Set Timer0 to run at 100Hz(10ms) by enabling Timer0 Peripherals

* Example Call: timer0IntSetup();

*/
void timer0IntSetup(void) {
	// enable Timer0 peripherals
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	// set timer for 100Hz at 50% duty cycle
	uint32_t ui32Period = (SysCtlClockGet() / timer0IntFreq) / 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);

	// enable the interrupt
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	// enable the timer
	TimerEnable(TIMER0_BASE, TIMER_A);
}

/*

* Function Name: setNextColor

* Input: none

* Output: none

* Logic:  Set the next color in a cycle of Red then Green then Blue and repeat

* Example Call: setNextColor();

*/
void setNextColor(void) {
	colorWheelTheta += (dTheta / timer0IntFreq);
	if (colorWheelTheta >= 1440) {colorWheelTheta %= 1440;}
	uint8_t section = colorWheelTheta / 240;
	switch(section) {
		case 0: ui8AdjustG = 10+colorWheelTheta; ui8AdjustR = 250; ui8AdjustB = 10; break;
		case 1: ui8AdjustR = 10+240-(colorWheelTheta-(section*240)); ui8AdjustG = 250; ui8AdjustB = 10; break;
		case 2: ui8AdjustB = 10+colorWheelTheta-(section*240); ui8AdjustG = 250; ui8AdjustR = 10; break;
		case 3: ui8AdjustG = 10+240-(colorWheelTheta-(section*240)); ui8AdjustB = 250; ui8AdjustR = 10; break;
		case 4: ui8AdjustR = 10+colorWheelTheta-(section*240); ui8AdjustB = 250; ui8AdjustG = 10; break;
		case 5: ui8AdjustB = 10+240-(colorWheelTheta-(section*240)); ui8AdjustR = 250; ui8AdjustG = 10; break;
	}
}

/*

* Function Name: setPulseWidth

* Input: none

* Output: none

* Logic:  Set the next color's duration in a cycle of Red then Green then Blue and repeat

* Example Call: setPulseWidth();

*/
void setPulseWidth(void) {
	if (currMode == SERVO) {
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
		ui8AdjustR = 1; ui8AdjustB = 1; ui8AdjustG = 1;
	}
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustR * ui32Load / 1000);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustB * ui32Load / 1000);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustG * ui32Load / 1000);
}

/*

* Function Name: Timer0IntHandler

* Input: none

* Output: none

* Logic: The intterupt handler function for Timer0 as set up in timer0IntSetup()

* Example Call: Timer0IntHandler()// although applicable;

*/
void Timer0IntHandler(void) {
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	unsigned char detectSW1 = detectKeyPress(GPIO_PORTF_BASE, GPIO_PIN_4, PIN_4_STATE_INDEX);
	unsigned char detectSW2 = detectKeyPress(GPIO_PORTF_BASE, GPIO_PIN_0, PIN_0_STATE_INDEX);
	bool modeChange = false;
	switch(detectSW2) {
		case 0: 
		case 1: mayBeModeManualBlue = false; break;//normal sw2 pressed
		case 2: 
			if (detectSW1 == 1) { modeChange = true;
				if (mayBeModeManualBlue) {currMode = MANUAL_B;} else {currMode = MANUAL_R;}
				mayBeModeManualBlue = true;
			} else if (detectSW1 == 2) {
				currMode = MANUAL_G; modeChange = true;
			}
			break;
	}

	if (!modeChange) {
		switch(currMode) {
			case AUTO: 
				if (detectSW1 == 1) {dTheta += timer0IntFreq;}
				if (detectSW2 == 1) {dTheta -= timer0IntFreq; if(dTheta < timer0IntFreq) {dTheta = timer0IntFreq;}}
				setNextColor();
				break;
			case MANUAL_R: ui8AdjustG = 10; ui8AdjustB = 10;
				if (detectSW1 == 1) {ui8AdjustR += 10; if(ui8AdjustR > 240) {ui8AdjustR = 240;}}
				if (detectSW2 == 1) {ui8AdjustR -= 10; if(ui8AdjustR < 10) {ui8AdjustR = 10;}}
				break;
			case MANUAL_G: ui8AdjustR = 10; ui8AdjustB = 10;
				if (detectSW1 == 1) {
					ui8AdjustG += 10;
					if(ui8AdjustG > 240) {ui8AdjustG = 240;}
				}
				if (detectSW2 == 1) {ui8AdjustG -= 10; if(ui8AdjustG < 10) {ui8AdjustG = 10;}}
				break;
			case MANUAL_B: ui8AdjustG = 10; ui8AdjustR = 10;
				if (detectSW1 == 1) {ui8AdjustB += 10; if(ui8AdjustB > 240) {ui8AdjustB = 240;}}
				if (detectSW2 == 1) {ui8AdjustB -= 10; if(ui8AdjustB < 10) {ui8AdjustB = 10;}}
				break;
			case SERVO:
				if (detectSW1 == 1) {ui8Adjust += 5; if(ui8Adjust > 120) {ui8Adjust = 120;}}
				if (detectSW2 == 1) {ui8Adjust -= 5; if(ui8Adjust < 30) {ui8Adjust = 30;}}
				break;
		}
		setPulseWidth();
	}
}

/*

* Function Name: main()

* Input: none

* Output: none

* Logic: main function of the code to handle the lab experiment. 

* Example Call: N.A ;

*/
int main(void) {

	setup();
	pin_config();
	timer0IntSetup();

	setPulseWidth();

	while(1)
	{
	}
}
