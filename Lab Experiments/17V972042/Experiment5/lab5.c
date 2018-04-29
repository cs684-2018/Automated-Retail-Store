/*
 * Lab 5 : Sample Code
 * Course : CS-684
 * Created on: Jan 27, 2018
 * Author: Lab staff ERTS LAB
 * This sample code will help you in interfacing glcd 
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/adc.h"
#include "mickey.h"
#include "logo.h"
#include "one.h.txt"
#include "two.h.txt"
#include "three.h.txt"
#include "four.h.txt"
#include "five.h.txt"
#include "six.h.txt"
#include "seven.h.txt"
#include "eight.h.txt"


/*To display image include an array with hex values and index it accordingly*/

/* void glcd_cmd(cmd)
* This function sends commands to the GLCD.
* Value of RS is 0
* Command is written on data lines.
* Enable is made 1 for a short duration.
*/
void glcd_cmd(unsigned char cmd)
{
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,0x00);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,0x00);

    /* RS = 0 */
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6,0x00);

    /* Put command on data lines */
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,cmd);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,cmd);

    /* Generate a high to low pulse on enable */
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0,0x01);
    SysCtlDelay(1340);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0,0x00);

}




/* void glcd_data(data)
* This function sends data to the GLCD.
* Value of RS is 1
* Data is written on data lines.
* Enable is made 1 for a short duration.
*/
void glcd_data(unsigned char data)
{
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,0x00);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,0x00);

    /* RS = 1 */
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6,0x40);

    /* Put command on data lines */
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,data);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,data);

    /* Generate a high to low pulse on enable */
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0,0x01);
    SysCtlDelay(1340);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0,0x00);
}



/* void glcd_init()
* This function initializes the GLCD.
* Always call this function at the beginning of main program after configuring the port pins.
*/
void glcd_init()
{
    SysCtlDelay(6700000/50);                            // creates ~10ms delay - TivaWare fxn
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0x00);    //cbi(GPORTC_2,GLCD_RST);
    SysCtlDelay(6700000/50);
    /* Set RST */
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_5,0x20);

    /* Set CS1 (CS1=1 and CS2=0) The right side is selected(column>64) */
    GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x00);

    /* Select the start line */
    glcd_cmd(0xC0);
    //SysCtlDelay(6700);

    /* Send the page */
    glcd_cmd(0xB8);
    // SysCtlDelay(6700);

    /*Send the column */
    glcd_cmd(0x40);
    //SysCtlDelay(6700);

    /* Send glcd on command */
    glcd_cmd(0x3F);


    /* Initialize the right side of GLCD */
    /* Set CS2 (CS2=1 and CS1=0) The right side is selected(column>64) */
    GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x08);

    /* Select the start line */
    glcd_cmd(0xC0);
    SysCtlDelay(6700);

    /* Send the page */
    glcd_cmd(0xB8);
    //  SysCtlDelay(6700);

    /*Send the column */
    glcd_cmd(0x40);
    //  SysCtlDelay(6700);

    /* Send glcd on command */
    glcd_cmd(0x3F);
    //  SysCtlDelay(6700);
}




/* void glcd_setpage(page)
* This function selects page number on GLCD.
* Depending on the value of column number CS1 or CS2 is made high.
*/
void glcd_setpage(unsigned char page)
{
    /* Set CS1 (CS1=1 and CS2=0) The right side is selected(column>64) */
    GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x00);
    glcd_cmd(0xB8 | page);
    SysCtlDelay(100);

    /* Set CS2 (CS2=1 and CS1=0) The right side is selected(column>64) */
    GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x08);
    glcd_cmd(0xB8 | page);
    SysCtlDelay(100);
}



/* void glcd_setcolumn(column)
* This function selects column number on GLCD.
* Depending on the value of column number CS1 or CS2 is made high.
*/
void glcd_setcolumn(unsigned char column)
{
    if(column < 64)
    {
        /* Set CS1 (CS1=1 and CS2=0) The right side is selected(column>64) */
        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x00);
        glcd_cmd(0x40 | column);
        SysCtlDelay(6700);
    }
    else
    {
        /* Set CS2 (CS2=1 and CS1=0) The right side is selected(column>64) */
        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x08);
        glcd_cmd(0x40 | (column-64));
        SysCtlDelay(6700);
    }

}



/* void glcd_cleardisplay()
* This function clears the data on GLCD by writing 0 on all pixels.
*/
void glcd_cleardisplay()
{
    unsigned char i,j;
    for(i=0;i<8;i++)
    {
        glcd_setpage(i);
        for(j=0;j<128;j++)
        {
            glcd_setcolumn(j);
            glcd_data(0x00);
        }
    }
}

void setup(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
}

void pin_config(void) {

    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4);
}

void print_square(unsigned char start_page, unsigned char start_column)
{
   unsigned char i,j;
   for(i=0;i<1;i++)
   {
       glcd_setpage(start_page + i);
       for(j=0;j<8;j++)
       {
           glcd_setcolumn(start_column + j);
           glcd_data(0xFF);
       }
   }
    return;
}


void print_image(unsigned char image[])
{
    glcd_cleardisplay();
    unsigned char i,j;
    int k = 0;
    for(i=0;i<8;i++)
    {
        glcd_setpage(i);
        for(j=0;j<128;j++)
        {
            glcd_setcolumn(j);
            glcd_data(image[k]);
            k++;
        }
    }
    return;
}

void print_image_number(int image_number)
{
    switch(image_number)
        {
        case 1 :
            print_image(one);
            break;
        case 2:
            print_image(two);
            break;
        case 3:
            print_image(three);
            break;
        case 4:
            print_image(four);
            break;
        case 5:
            print_image(five);
            break;
        case 6:
            print_image(six);
            break;
        case 7:
            print_image(seven);
            break;
        case 8:
            print_image(eight);
            break;
        }
}

uint32_t ui32PD0Avg;
uint32_t ui32PD1Avg;
unsigned char JoyX, JoyY;
uint32_t delay = 6700000/4;

int main()
{

    /* Enable all the peripherals */
    /* PORTS A,E,F,C,D,B */
    uint32_t ui32ADC0Value[4];
    uint32_t ui32ADC1Value[4];

    setup();

    /* Unlock pin PF0 */
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK)= GPIO_LOCK_KEY;    // unlocking sw2 switch
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK)= 0;

    /* Configure Enable pin as output */
    pin_config();

    /* Configure PE5 (RST), PE0 to PE3 (D0 to D3) and PB4 to PB7(D4 to D7) as output pins */


        /* Configure RS as output */


        /* Configure CS1 or CS2 as output */


        /*initialize glcd*/
    glcd_init();
    glcd_cleardisplay();


    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH7); // Input from pd0
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH7);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH7);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH7|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 1);

    ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

    ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_CH6); // Input from pd1
    ADCSequenceStepConfigure(ADC1_BASE, 1, 1, ADC_CTL_CH6);
    ADCSequenceStepConfigure(ADC1_BASE, 1, 2, ADC_CTL_CH6);
    ADCSequenceStepConfigure(ADC1_BASE, 1, 3, ADC_CTL_CH6|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC1_BASE, 1);

    // IntMasterEnable();

    /*This ensures buzzer remains OFF, since PC4 when logic 0 turns ON buzzer */
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4,16);
    
       /* Select a page and display lines on it from column 0 to 127 */
    ///////////////////////////////////////////// part 1
    // print_image(mickey);
    // print_image(logo);
    // while (1) { }
    
    int image_number = 1;
    while (1)
    {
        ADCIntClear(ADC0_BASE, 1);
        ADCProcessorTrigger(ADC0_BASE, 1);
        ADCIntClear(ADC1_BASE, 1);
        ADCProcessorTrigger(ADC1_BASE, 1);
        
        while(!ADCIntStatus(ADC0_BASE, 1, false) || !ADCIntStatus(ADC1_BASE, 1, false)) { }
        
        ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
        ui32PD0Avg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;

        ADCSequenceDataGet(ADC1_BASE, 1, ui32ADC1Value);
        ui32PD1Avg = (ui32ADC1Value[0] + ui32ADC1Value[1] + ui32ADC1Value[2] + ui32ADC1Value[3] + 2)/4;

        JoyY = (unsigned char)(ui32PD0Avg/512);
        JoyY = 7 - JoyY;
        if (ui32PD1Avg > 3840) {ui32PD1Avg = 3840;}
        JoyX = (unsigned char)(ui32PD1Avg/32);
        JoyX = 120 - JoyX;
        ////////////////////////////////////////part 2
        // glcd_cleardisplay();
        // print_square(JoyY, JoyX);
        // SysCtlDelay(delay);

        //////////////////////////////////////// part 3
		print_image_number(image_number);
		image_number++;
		if (image_number == 9)
		{
		   image_number = 1;
		}
		if (ui32PD0Avg < 200) {delay*=2;}
		if (ui32PD0Avg > 3900) {delay/=2;}
		SysCtlDelay(delay);
    }
    
}
