// Lab6.c
// Program written by: ***Alamin Ahmed & Tu Tran***
// Date Created: 7/8/2017
// Last Modified: 7/11/2017
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// MOOC lab 13 or EE319K lab6 starter
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 3/6/17 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

//	   \\
//	   (o>
//	\\_//)
// 	 \_/_)
//	  _|_




#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "SysTick.h"
#include "MCU_Clock.h"
#include "dac.h"



// basic functions defined at end of startup.s
void PinClockInit(void);
void heartBeatInit(void);
void heartBeat(void);

int main(void){      
  PLL_Init(); 		//set system clock to 80 MHz
  SysTick_Init(); //initilize the delay clock  
	PinClockInit();	//initilize the clock for the pins we are going to use
  Piano_Init();		//initilize the piano keys (button on PE0,PE1,PE2)
	DAC_Init();			//initilize the output (pin PB0,PB1,PB2,PB3)
	heartBeatInit();//init heart beat
  // other initialization

  while(1){ 
		//SysTick_Wait10ms(100);
		switch(GPIO_PORTE_DATA_R & 0x07){
			case 1:  Sound_Play(C); break;
			case 2:  Sound_Play(B); break;
			case 3:  Sound_Play(BF); break;
			case 4:  Sound_Play(A); break;
			case 5:  Sound_Play(AF); break;
			case 6:  Sound_Play(G); break;
			case 7:  Sound_Play(GF); break;
			default: break;
		}
		
		heartBeat();
  } 
}

void heartBeatInit(void){
	GPIO_PORTF_DIR_R |= 0x02; //PIN 1 OUTPUT
	GPIO_PORTF_DEN_R |= 0x02; //DIGITAL IO
}

void heartBeat(void){
	GPIO_PORTF_DATA_R ^= 0x02;
}

void PinClockInit(void){
	//Port B, E, F init
	SYSCTL_RCGCGPIO_R |= 0x00000032; //ACTIVATE CLOCK FOR PORT B, E, F
	while((SYSCTL_PRGPIO_R&0x00000032)==0) //WAIT
	{
	}
}


