// Piano.c
// This software configures the off-board piano keys
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 3/6/17 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "dac.h"
#define PE03 (*((volatile uint32_t *) 0x4002403C))
// **************Piano_Init*********************
// Initialize piano key inputs, called once 
// Input: none 
// Output: none
void Piano_Init(void)
{
	//PortE init
		SYSCTL_RCGCGPIO_R |= 0x00000010; //ACTIVATE CLOCK FOR PORT E
		while((SYSCTL_PRGPIO_R&0x00000010)==0) //WAIT
		{
		}
		GPIO_PORTE_CR_R = 0x0F; //PORT E PIN 0-3
		GPIO_PORTE_DIR_R &= ~0x0F; //PIN 0-3 INPUT
		GPIO_PORTE_DEN_R = 0x0F; //DIGITAL IO
}

// **************Piano_In*********************
// Input from piano key inputs 
// Input: none 
// Output: 0 to 7 depending on keys
// 0x01 is just Key0, 0x02 is just Key1, 0x04 is just Key2
uint32_t Piano_In(void)
{
	uint32_t result;
	switch(PE03)
	{
		case 1: //PE0 is pressed
			result = 1;
			break;
		case 2: //PE1 is pressed
			result = 2;
			break;
		case 4: //PE2 is pressed
			result = 4;
			break;
		case 8: //PE3 is pressed
			result = 8;
			break;
		default:
			result = 0;
	}
	return result;
}
