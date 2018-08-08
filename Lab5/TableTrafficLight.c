// TableTrafficLight.c EE319KLab 5
// Program written by: ***Alamin Ahmed & Tu Tran***
// Date Created: 7/1/2017
// Last Modified: 7/3/2017
// Runs on LM4F120 or TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  

/* solution, do not post

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "SysTick.h"
#include "MCU_Clock.h"

// Defination (these also defined inside the tm4c123gh6pm.h file)
#define GPIO_PORTB_DATA_R       (*((volatile uint32_t *)0x400053FC))
#define GPIO_PORTE_DATA_R       (*((volatile uint32_t *)0x400243FC))
#define GPIO_PORTF_DATA_R       (*((volatile uint32_t *)0x400253FC))
	
#define S_G (*((volatile unsigned long *)0x40005004)) //PB0 =1
#define S_Y (*((volatile unsigned long *)0x40005008)) //PB1 =2
#define S_R (*((volatile unsigned long *)0x40005010)) //4
#define W_G (*((volatile unsigned long *)0x40005020)) //8
#define W_Y (*((volatile unsigned long *)0x40005040)) //16
#define W_R (*((volatile unsigned long *)0x40005080)) //PB5 =32
#define W_B (*((volatile unsigned long *)0x40024004)) //PE0 =1
#define S_B (*((volatile unsigned long *)0x40024008)) //PE1 =2
#define P_B (*((volatile unsigned long *)0x40024010)) //PE2 =4
#define P_R (*((volatile unsigned long *)0x40025008)) //PF1 =2
#define P_G (*((volatile unsigned long *)0x40025020)) //PF3 =8

// Declare your FSM linked structure here
void initPins(void);

int main(void){ 
	volatile unsigned long delay;
	
  PLL_Init(); //set system clock to 80 MHz
  SysTick_Init(); //initilize the delay clock   
	initPins(); //initlize the pin setting

	GPIO_PORTB_DATA_R |= 0x01;
	//S_G = 1;
  //FSM Engine
  while(1){
		
  }
}

void initPins(void){
	  //Port B, E, F init
		SYSCTL_RCGCGPIO_R |= 0x00000032; //ACTIVATE CLOCK FOR PORT B, E, F
		while((SYSCTL_PRGPIO_R&0x00000032)==0) //WAIT
		{
		}
		GPIO_PORTB_CR_R = 0x3F; //OPEN PORT B PIN 0-5
		GPIO_PORTB_DIR_R = 0x3F; //PIN 0-5 OUTPUT
		GPIO_PORTB_DEN_R = 0x3F; //DIGITAL IO
		
		GPIO_PORTE_CR_R = 0x07; //PORT E PIN 0-2
		GPIO_PORTE_DIR_R &= ~0x07; //PIN 0-2 INPUT
		GPIO_PORTE_DEN_R = 0x07; //DIGITAL IO
		
		GPIO_PORTF_CR_R = 0x0A; //PORT F PIN 1,3
		GPIO_PORTF_DIR_R = 0x0A; //PIN 1,3 OUTPUT
		GPIO_PORTF_DEN_R = 0x0A; //DIGITAL IO
}

