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
void initState(void);			//initilize the states of the leds
void initPins(void);			//initilize the pins
void activateEW(void);		//change state from NS to EW
void activateNS(void);		//change state from EW to NS
void activateP(void);			//turn on pedestrian
void toggleEWNS(void);		//toggle EW to NS ans vise versa
void deactivateP(void);		//turn off pedestrian with warning
void blinkP(void);				//blink pedestrian red led
void switchEW(void);			//check and activateEW
void switchNS(void);			//check and activateNS
void toggleAll(void);			//toggle everything 
void toggleP(void);				//activate and deactivate pedestrian eith 2sec delay
void togglePWithEW(void);	//togggle pedestrian with switchEW
void togglePWithNS(void);	//togggle pedestrian with switchNS

int main(void){ 
	volatile unsigned long delay;
	
  PLL_Init(); 		//set system clock to 80 MHz
  SysTick_Init(); //initilize the delay clock   
	initPins(); 		//initlize the pin setting
	initState(); 		//initialize states
	
  //FSM Engine
  while(1){
		//x  SP SN SE
		//E3 E2 E1 E0
		//0  1  1  1
		switch(GPIO_PORTE_DATA_R & 0x07){ 
			case 1: switchEW(); break; 			//east/west
			case 2: switchNS(); break; 			//noth/south
			case 3: toggleEWNS(); break; 		//both east/west & noth/south
			case 4: activateP(); break; 		//pedestrian
			case 5: togglePWithEW(); break; //pedestrian & east/west
			case 6: togglePWithNS(); break; //pedestrian & noth/south
			case 7: toggleAll(); break; 		//all
			default: break;
		}
		SysTick_Wait10ms(100); //1sec delay to debounce and update the sensor reading
  }
}
void toggleP(void){
	activateP();
	SysTick_Wait10ms(200);
	deactivateP();
}
void togglePWithNS(void){
	switchNS();
	toggleP();
}
void togglePWithEW(void){
	switchEW();
	toggleP();
}
void toggleAll(void){
	toggleEWNS();
	activateP();
}
void switchEW(void){
	//PG x  PR x
	//F3 F2 F1 F0
	//1  0  0  0
	//x  x  ER EY EG NR NY NG
	//B7 B6 B5 B4 B3 B2 B1 B0
	//0  0  0  0  1  0  0  0
	switch((GPIO_PORTB_DATA_R & 0x08) | ((GPIO_PORTF_DATA_R & 0x08)>>1)){ //EG PG x x
		case 4: deactivateP(); activateEW(); break;
		case 8: break;
		case 12: break;
		default: activateEW(); break;
	}
}
void switchNS(void){
	//PG x  PR x
	//F3 F2 F1 F0
	//1  0  0  0
	//x  x  ER EY EG NR NY NG
	//B7 B6 B5 B4 B3 B2 B1 B0
	//0  0  0  0  0  0  0  1
	switch((GPIO_PORTB_DATA_R & 0x01) | (GPIO_PORTF_DATA_R & 0x08)){ //PG x x NG
		case 1: break;
		case 8: deactivateP(); activateNS(); break;
		case 9: break;
		default: activateNS(); break;
	}
}
void blinkP(void){
	//PG x  PR x
	//F3 F2 F1 F0
	//1  0  1  0
	GPIO_PORTF_DATA_R &= ~0x02;
	SysTick_Wait10ms(50);
	GPIO_PORTF_DATA_R |= 0x02;
	SysTick_Wait10ms(50);
	GPIO_PORTF_DATA_R &= ~0x02;
	SysTick_Wait10ms(50);
	GPIO_PORTF_DATA_R |= 0x02;
	SysTick_Wait10ms(50);
	GPIO_PORTF_DATA_R &= ~0x02;
	SysTick_Wait10ms(50);
	GPIO_PORTF_DATA_R |= 0x02;
	SysTick_Wait10ms(50);
	GPIO_PORTF_DATA_R &= ~0x02;
	SysTick_Wait10ms(50);
	GPIO_PORTF_DATA_R |= 0x02;
	SysTick_Wait10ms(50);
}
void deactivateP(void){
	//change from:
	//PG x  PR x
	//F3 F2 F1 F0
	//1  0  0  0
	
	//to
	//PG x  PR x
	//F3 F2 F1 F0
	//0  0  1  0
	GPIO_PORTF_DATA_R &= ~0x08;
	GPIO_PORTF_DATA_R |= 0x02;
	blinkP(); //blink 4 times
}
void activateP(void){
	//change from:
	//PG x  PR x
	//F3 F2 F1 F0
	//0  0  1  0
	
	//to
	//PG x  PR x
	//F3 F2 F1 F0
	//1  0  0  0
	GPIO_PORTF_DATA_R &= ~0x02;
	GPIO_PORTF_DATA_R |= 0x08;
}
void toggleEWNS(void){
	SysTick_Wait10ms(200);
	//PG x  PR x
	//F3 F2 F1 F0
	//1  0  0  0
	//x  x  ER EY EG NR NY NG
	//B7 B6 B5 B4 B3 B2 B1 B0
	//0  0  0  0  1  0  0  1
	switch((GPIO_PORTB_DATA_R & 0x09) | ((GPIO_PORTF_DATA_R & 0x08)>>1)){ //EG PG x NG
		case 1: activateEW(); break;
		case 5: deactivateP(); activateEW();break;
		case 8: activateNS(); break;
		case 12: deactivateP(); activateNS(); break;
		default: break;
	}
	
}
void activateEW(void){
	//change from:
	//x  x  ER EY EG NR NY NG
	//B7 B6 B5 B4 B3 B2 B1 B0
	//0  0  1  0  0  0  0  1
	
	//to:
	//x  x  ER EY EG NR NY NG
	//B7 B6 B5 B4 B3 B2 B1 B0
	//0  0  1  0  0  0  1  0
	GPIO_PORTB_DATA_R &= ~0x21;
	GPIO_PORTB_DATA_R |= 0x22;
	SysTick_Wait10ms(200);
	
	//to:
	//x  x  ER EY EG NR NY NG
	//B7 B6 B5 B4 B3 B2 B1 B0
	//0  0  1  0  0  1  0  0
	GPIO_PORTB_DATA_R &= ~0x22;
	GPIO_PORTB_DATA_R |= 0x24;
	SysTick_Wait10ms(100);
	
	//to:
	//x  x  ER EY EG NR NY NG
	//B7 B6 B5 B4 B3 B2 B1 B0
	//0  0  0  0  1  1  0  0
	GPIO_PORTB_DATA_R &= ~0x24;
	GPIO_PORTB_DATA_R |= 0x0C;
	
}
void activateNS(void){
	//change from:
	//x  x  ER EY EG NR NY NG
	//B7 B6 B5 B4 B3 B2 B1 B0
	//0  0  0  0  1  1  0  0
	
	//to:
	//x  x  ER EY EG NR NY NG
	//B7 B6 B5 B4 B3 B2 B1 B0
	//0  0  0  1  0  1  0  0
	GPIO_PORTB_DATA_R &= ~0x0C;
	GPIO_PORTB_DATA_R |= 0x14;
	SysTick_Wait10ms(200);
	
	//to:
	//x  x  ER EY EG NR NY NG
	//B7 B6 B5 B4 B3 B2 B1 B0
	//0  0  1  0  0  1  0  0
	GPIO_PORTB_DATA_R &= ~0x14;
	GPIO_PORTB_DATA_R |= 0x24;
	SysTick_Wait10ms(100);
	
	//to:
	//x  x  ER EY EG NR NY NG
	//B7 B6 B5 B4 B3 B2 B1 B0
	//0  0  1  0  0  0  0  1
	GPIO_PORTB_DATA_R &= ~0x24;
	GPIO_PORTB_DATA_R |= 0x21;
	
}
void initState(void){
	//x  x  ER EY EG NR NY NG
	//B7 B6 B5 B4 B3 B2 B1 B0
	//0  0  1  0  0  0  0  1
	GPIO_PORTB_DATA_R |= 0x21; //set the initial state
	
	//PG x  PR x
	//F3 F2 F1 F0
	//0  0  1  0
	GPIO_PORTF_DATA_R |= 0x02;
}
void initPins(void){
	  //Port B, E, F init
		SYSCTL_RCGCGPIO_R |= 0x00000032; //ACTIVATE CLOCK FOR PORT B, E, F
		while((SYSCTL_PRGPIO_R&0x00000032)==0) //WAIT
		{
		}
		//GPIO_PORTB_CR_R = 0x3F; //OPEN PORT B PIN 0-5
		GPIO_PORTB_DIR_R |= 0x3F; //PIN 0-5 OUTPUT
		GPIO_PORTB_DEN_R |= 0x3F; //DIGITAL IO
		
		//GPIO_PORTE_CR_R = 0x07; //PORT E PIN 0-2
		GPIO_PORTE_DIR_R &= ~0x07; //PIN 0-2 INPUT
		GPIO_PORTE_DEN_R |= 0x07; //DIGITAL IO
		
		//GPIO_PORTF_CR_R = 0x0A; //PORT F PIN 1,3
		GPIO_PORTF_DIR_R |= 0x0A; //PIN 1,3 OUTPUT
		GPIO_PORTF_DEN_R |= 0x0A; //DIGITAL IO
}

