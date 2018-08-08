// dac.c
// This software configures DAC output
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 3/6/17 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#include <stdint.h>
#include "tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

static uint32_t privousData = 0;

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
	GPIO_PORTB_DIR_R |= 0x0F; //PIN 0-3 OUTPUT
	GPIO_PORTB_DEN_R |= 0x0F; //DIGITAL IO
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(uint32_t data){
	GPIO_PORTB_DATA_R &= ~privousData; //clearing the pins
	GPIO_PORTB_DATA_R |= data;	//setting the pins
	privousData = data;	//storing the privous pins state
}
