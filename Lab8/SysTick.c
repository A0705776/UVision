// SysTick.c
// Implements two busy-wait based delay routines
#include <stdint.h>
#include "SysTick.h"
#include "tm4c123gh6pm.h"




void SysTick_Init(void){
  NVIC_ST_CTRL_R &=0x0;
	NVIC_ST_RELOAD_R = 10000000; //10 ms = 400,000
	NVIC_ST_CURRENT_R = 0;
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; //Set priority 2
	NVIC_ST_CTRL_R |= 0x7;//Start and arm
}

void SysTick_Wait25ms(){
	NVIC_ST_CURRENT_R = 0;
	while((NVIC_ST_CTRL_R&0x00010000)==0){	}
}
int getADC(){ return ADCMailbox;}

void SysTick_Handler(void){
	PF2 ^= 0x04;
	PF2 ^= 0x04;
	ADCMailbox = 0;
	
}
