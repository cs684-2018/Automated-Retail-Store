/*

* Team Id: 07

* Author List: Neeladrishekhar Kanjilal

* Filename: lab-4.c 

* Functions: main()  

* Global Variables: PD0Char, PD1Char, ui32PD0Avg, ui32PD1Avg, ui32ADC0Value, ui32ADC1Value

*/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"

unsigned char PD0Char;
unsigned char PD1Char;
uint32_t ui32PD0Avg;
uint32_t ui32PD1Avg;
uint32_t ui32ADC0Value[4];
uint32_t ui32ADC1Value[4];

/*

* Function Name: main()

* Input: none

* Output: none

* Logic: main function of the code to handle the lab experiment. 

* Example Call: N.A ;

*/
int main(void) {

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);

    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH7);	// Input from pd0
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH7);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH7);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH7|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);

	ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

	ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_CH6);	// Input from pd1
	ADCSequenceStepConfigure(ADC1_BASE, 1, 1, ADC_CTL_CH6);
	ADCSequenceStepConfigure(ADC1_BASE, 1, 2, ADC_CTL_CH6);
	ADCSequenceStepConfigure(ADC1_BASE, 1, 3, ADC_CTL_CH6|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC1_BASE, 1);
	
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 57600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	IntMasterEnable();
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); 

	/*This ensures buzzer remains OFF, since PC4 when logic 0 turns ON buzzer */
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4);
	GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4,16);

	/*This ensures LEDs remains OFF.*/
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);

	while(1) {
		ADCIntClear(ADC0_BASE, 1);
		ADCProcessorTrigger(ADC0_BASE, 1);
		ADCIntClear(ADC1_BASE, 1);
		ADCProcessorTrigger(ADC1_BASE, 1);
		
		while(!ADCIntStatus(ADC0_BASE, 1, false) || !ADCIntStatus(ADC1_BASE, 1, false)) { }
		
		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
		ui32PD0Avg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
		PD0Char = ui32PD0Avg/16;
		ADCSequenceDataGet(ADC1_BASE, 1, ui32ADC1Value);
		ui32PD1Avg = (ui32ADC1Value[0] + ui32ADC1Value[1] + ui32ADC1Value[2] + ui32ADC1Value[3] + 2)/4;
		PD1Char = ui32PD1Avg/16;

		UARTCharPut(UART0_BASE, PD1Char);
		UARTCharPut(UART0_BASE, ',');
		UARTCharPut(UART0_BASE, PD0Char);
		UARTCharPut(UART0_BASE, '\n');

		SysCtlDelay(2500000);
	}
}
