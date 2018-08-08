//ECE3436 Fall 2017
//Lab 5 Traffic Light Control
//Name: insert your name here or this looks silly

// Index implementation of a Moore finite state machine to operate a traffic light.  
//Write your own PLL.c and PLL.h to initialize the clock to 80MHz
//Use these ports to setup your circuit
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
//#include "TExaS.h"

// Declare your FSM linked structure here

void EnableInterrupts(void);

int main(void)
{
	volatile unsigned long delay;
  //PLL_Init();
  SysTick_Init();     
  EnableInterrupts();
  //FSM Engine
  while(1)
	{
 
  }
}

