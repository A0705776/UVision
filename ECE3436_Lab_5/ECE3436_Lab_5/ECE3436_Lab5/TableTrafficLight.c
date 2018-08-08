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

//	   \\
//	   (o>
//	\\_//)
// 	 \_/_)
//	  _|_

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "SysTick.h"
#include "MCU_Clock.h"

// Defination (these also defined inside the tm4c123gh6pm.h file)
#define GPIO_PORTB_DATA_R       (*((volatile uint32_t *)0x400053FC))
#define GPIO_PORTE_DATA_R       (*((volatile uint32_t *)0x400243FC))
#define GPIO_PORTF_DATA_R       (*((volatile uint32_t *)0x400253FC))

// Declare your FSM linked structure here
void initPins(void);			//initilize the pins


int main(void){ 
	
  PLL_Init(); 		//set system clock to 80 MHz
  SysTick_Init(); //initilize the delay clock   
	initPins(); 		//initlize the pin setting

	
  //FSM Engine
  while(1){
		//Write your code here
  }
}

void initPins(void){
	  //Port B, E, F init
		//Write your code here
}

