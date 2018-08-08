
//Included files
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/uartstdio.h"

//Prototypes
void hardware_init(void);
uint8_t Find_Mean(void);
uint8_t Find_Range(void);
uint8_t IsMonotonic(void);

//Variables
uint8_t Readings[10] = {9,8,8,6,7,7,6,6,6,5};


int main(void)
{
	hardware_init();
	UARTprintf("The mean of the readings is %d.\n", Find_Mean());
	UARTprintf("The range of the readings is %d.\n", Find_Range());
	if(IsMonotonic() == 0)
	{
		UARTprintf("The readings are not non-increasing monotonic series.\n");
	}
	else if(IsMonotonic() == 1)
	{
		UARTprintf("The readings are non-increasing monotonic series.\n");
	}
}

void hardware_init(void)
{
	//40MHz clock
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	//UART init, USB, 115200 baudrate
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, 40000000, 115200,
	(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	UARTStdioConfig(0,115200,40000000);
}

uint8_t Find_Mean(void)
{
	int i=0;
	int sum = 0;
	int mean = 0;
	for(i = 0; i < 10 ; i++)
	{
		sum += Readings[i];
	}
	mean = sum/10;
	return mean;
}

uint8_t Find_Range(void)
{
	int i = 0;
	int min = Readings[0];
	int max = Readings[0];
	int range = 0;
	for(i = 0;i < 10; i++)
	{
		if(min >= Readings[i])
		{
			min = Readings[i];
		}
		if(max <= Readings[i])
		{
			max = Readings[i];
		}
	}
	range = max - min;
	return range;
}

uint8_t IsMonotonic(void)
{
	int i = 0;
	int result =1;
	for(i = 0; i < 9; i++)
	{
		if((Readings[i+1] - Readings[i]) > 0)
		{
			result = 0;
			break;
		}		
	}
	return result;
}
