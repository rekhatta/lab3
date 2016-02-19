//****************** EE383lab4.c ***************
// Program written by: Your Names
// Date Created: 11/03/2013 
// Original Author: Chris Mitchell (cpmitc4@gmail.com)
// Section 9-10am     TA: Ying Yu
// Lab number: 4
//
// Brief description of the program:
// This lab demonstrates a simple enviornmental monitoring unit.  This 
// program will read the internal analog temperature sensor on the TivaWare
// Launchpad development board through the onboard ADC.  The temperature will
// then be converted to Celsius and Farenheit and printed to the console.  
  
//
// The following UART signals are configured only for displaying console
// messages for this example.  These are not required for operation of the
// ADC.
// - UART0 peripheral
// - GPIO Port A peripheral (for UART0 pins)
// - UART0RX - PA0
// - UART0TX - PA1
//
//
// NOTE: The internal temperature sensor is not calibrated.  This example
// just takes the raw temperature sensor sample and converts it using the
// equation found in the datasheet.
//
//*****************************************************************************


//Includes
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "inc/tm4c123gh6pm.h"



//Prototypes
void InitConsole(void); //Instructor Provided
void ADCInit (void);    //Instructor Provided

//Student Written Functions
uint32_t convertToFahrenheit( float TempC);  //To be completed
void PrintTemps (uint32_t TempC);//To be modified
void PrintFahrenheit(uint32_t TempF);

char listener(){
	return UARTgetc();
}	


//Determines the sample rate of the ADC 
uint32_t clkscalevalue = 6;

//*****************************************************************************
//
// Configure ADC0 for the temperature sensor input with a single sample.  Once
// the sample is done, an interrupt flag will be set, and the data will be
// read then displayed on the console via UART0.
//
//*****************************************************************************
int main(void)
{
    //
    // This array is used for storing the data read from the ADC FIFO. It
    // must be as large as the FIFO for the sequencer in use.  
		uint32_t ui32ADC0Value[4];

    // These variables are used to store the temperature conversions for
    // AVG, Celsius and Fahrenheit.
		uint32_t ui32TempAvg;
    uint32_t ui32TempValueC;
		uint32_t ui32TempValueF;
    

    // Set the clocking to run at 20 MHz (200 MHz / 5) using the PLL.  When
    // using the ADC, you must either use the PLL or supply a 16 MHz clock
    // source.
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ); //changed from SYSCTL_SYSDIV_10 (CM)

    // Set up the serial console to use for displaying messages.  This is just
    // for this lab program and is not needed for ADC operation.
    InitConsole();
   
    // Display the setup on the console.
    UARTprintf("EE383 Lab 4: Introduction to C Programming, ADC, UART\n");
		UARTprintf("*****************************************************\n");
    UARTprintf("Analog Input: Internal Temperature Sensor\n");
    
		//Init the ADC
		ADCInit();
    
		
		UARTprintf("Initialization Complete...\n");

    // Sample the temperature sensor forever.  Display the value on the
    // console.
    while(1)
    {
				char key = 'b';
        // Trigger the ADC conversion.
        ADCProcessorTrigger(ADC0_BASE, 1);

        // Wait for conversion to be completed.
        while(!ADCIntStatus(ADC0_BASE, 1, false))
        {
        }

        // Clear the ADC interrupt flag.
        ADCIntClear(ADC0_BASE, 1);

        // Read ADC Values.
        ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value); 
				
				//Average the 4 Samples 
				ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4; 
				
				//Convert Raw Data to Temp Celsius
				ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10; 
				ui32TempValueF = convertToFahrenheit((1475 - ((2475 * ui32TempAvg)) / 4096)/10);

        // Display the temperature value on the console.
        
				
				//PrintTemps (ui32TempValueC);

        //
        // This function provides a means of generating a constant length
        // delay.  The function delay (in cycles) = 3 * parameter.  Delay
        // 250ms arbitrarily.
        //
        SysCtlDelay(SysCtlClockGet() / clkscalevalue);
				
				// Switch statement for key listeing input
				key = listener();
				switch(key) {
					case 'c': 
						PrintTemps( ui32TempValueC);
					break;
					case 'f':
						PrintFahrenheit(ui32TempValueF);
					break;
					default :
					break;
				}
    }
}
//***************************************************************************
//NOTE: Fill in The Following Functions
//***************************************************************************

//This function converts the 32bit unsigned value representation of Celsius
//Temperature from the ADC and converts it to Farenheit
uint32_t convertToFahrenheit( float TempC)
{
		//Fill out this function
		uint32_t TempF=(1.8)*TempC+32.0;
		return TempF;
}

//This function prints the two passed temperatures to the console in an 
//easy human readable format. 
void PrintTemps (uint32_t TempC)
{
	
	UARTprintf("Temperature C = %3d*C\n", TempC);	

}

void PrintFahrenheit (uint32_t TempF) {
	
	UARTprintf("Temperature F = %3d*F\n", TempF);

}	

//PROVIDED FUNCTIONS.  NO NEED TO MODIFY
//*****************************************************************************
//
// This function sets up UART0 to be used for a console to display information
// as the lab is running.
//
//*****************************************************************************
void InitConsole(void)
{
    // Enable GPIO port A which is used for UART0 pins.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Configure the pin muxing for UART0 functions on port A0 and A1.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    // Enable UART0 so that we can configure the clock.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
   

		// Select the alternate (UART) function for these pins.   
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Initialize the UART for console I/O. 9600 BAUD
    UARTStdioConfig(0, 9600, 16000000);
}

//*****************************************************************************
//
// This function configures the ADC0 Peripheral for EE383LABS 4 and 5
// Configuration:
//	ADC0, Sequence 1(4 Samples)
//
//*****************************************************************************
void ADCInit(void)
{
    // The ADC0 peripheral must be enabled for use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enable sample sequence 1 with a processor signal trigger.  Sequence 1
    // will do a four samples when the processor sends a singal to start the
    // conversion.  Each ADC module has 4 programmable sequences, sequence 0
    // to sequence 3.  This lab is arbitrarily using sequence 1.
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

    // Configure  ADC0 on sequence 1.  Sample the temperature sensor
    // (ADC_CTL_TS) and configure the interrupt flag (ADC_CTL_IE) to be set
    // when the sample is done.  Tell the ADC logic that this is the last
    // conversion on sequence 3 (ADC_CTL_END).  Sequence 1 has 4
    // programmable steps.  Sequence 1 and 2 have 4 steps, and sequence 0 has
    // 8 programmable steps. Sequence3 has 1 step. For more information on the
    // ADC sequences and steps, reference the datasheet.

    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS); 
		ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS); 
		ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS); 
		ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END); 

    // Since sample sequence 1 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 1);

    // Clear the interrupt status flag.  This is done to make sure the
    // interrupt flag is cleared before we sample.
    ADCIntClear(ADC0_BASE, 1);	
}
//EOF
