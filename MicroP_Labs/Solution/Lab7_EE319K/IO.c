// IO.c
// This software configures the switch and LED
// You are allowed to use any switch and any LED, 
// although the Lab suggests the SW1 switch PF4 and Red LED PF1
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 
// Last Modified:  
// Lab number: 6


#include "tm4c123gh6pm.h"
#include "SysTick.h"
#include <stdint.h>

//------------IO_Init------------
// Initialize GPIO Port for a switch and an LED
// Input: none
// Output: none
void IO_Init(void) {
 // --UUU-- Code to initialize PF4 and PF2
	SYSCTL_RCGCGPIO_R |= 0x20; //ACTIVATE CLOCK FOR PORT F
	while((SYSCTL_PRGPIO_R&0x20)==0) {/*WAIT*/}
	
	GPIO_PORTF_DIR_R |= 0x04; //PIN 2 as output
	GPIO_PORTF_DEN_R |= 0x04; //DIGITAL IO
	
	GPIO_PORTF_DIR_R &= ~0x10;//PIN 4 as input
	GPIO_PORTF_PUR_R |= 0x10; //Pullup
	GPIO_PORTF_DEN_R |= 0x10; //DIGITAL IO
	
	SysTick_Init();
}

//------------IO_HeartBeat------------
// Toggle the output state of the  LED.
// Input: none
// Output: none
void IO_HeartBeat(void) {
 // --UUU-- PF2 is heartbeat
	GPIO_PORTF_DATA_R ^= 0x04;
}


//------------IO_Touch------------
// wait for release and press of the switch
// Delay to debounce the switch
// Input: none
// Output: none
void IO_Touch(void) {
 // --UUU-- wait for release; delay for 20ms; and then wait for press
	while((GPIO_PORTF_DATA_R & 0x10)==0x10){}
	SysTick_Wait10ms(2);
	while((GPIO_PORTF_DATA_R & 0x10)!=0x10){}
}  

