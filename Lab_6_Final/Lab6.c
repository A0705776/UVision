// Lab6.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// MOOC lab 13 or EE319K lab6 starter
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 3/6/17 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********


#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"
#include "dac.h"
#include "SysTick.h"
#define SYSDIV2 4
int sound_1[16] = {0,2,4,6,8,10,12,14,15,14,12,10,8,6,4,2};
// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void HW_init(void);

int main(void)
{      
  SYSTICK_INIT();
	HW_init();    // bus clock at 80 MHz
	DAC_Init();
  Piano_Init();
  Sound_Init(0);
  // other initialization
  EnableInterrupts();
  while(1)
	{
		switch(Piano_In())
		{
			case 1: //PE0 is pressed, play Mid-C at 262Hz
				Sound_Play(262);
				break;
			case 2: //PE1 is press, play D at 294Hz
				Sound_Play(294);
				break;
			case 4: //PE2 is press, play E at 330Hz
				Sound_Play(330);
				break;
			case 8: //PE3 is press, play F at 349Hz
				Sound_Play(349);
				break;	
		}			
  }    
}

void HW_init(void)
{
	 // 0) configure the system to use RCC2 for advanced features
  //    such as 400 MHz PLL and non-integer System Clock Divisor
  SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2;
  // 1) bypass PLL while initializing
  SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;
  // 2) select the crystal value and oscillator source
  SYSCTL_RCC_R &= ~SYSCTL_RCC_XTAL_M;   // clear XTAL field
  SYSCTL_RCC_R += SYSCTL_RCC_XTAL_16MHZ;// configure for 16 MHz crystal
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_OSCSRC2_M;// clear oscillator source field
  SYSCTL_RCC2_R += SYSCTL_RCC2_OSCSRC2_MO;// configure for main oscillator source
  // 3) activate PLL by clearing PWRDN
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_PWRDN2;
  // 4) set the desired system divider and the system divider least significant bit
  SYSCTL_RCC2_R |= SYSCTL_RCC2_DIV400;  // use 400 MHz PLL
  SYSCTL_RCC2_R = (SYSCTL_RCC2_R&~0x1FC00000) // clear system clock divider field
                  + (SYSDIV2<<22);      // configure for 80 MHz clock
  // 5) wait for the PLL to lock by polling PLLLRIS
  while((SYSCTL_RIS_R&SYSCTL_RIS_PLLLRIS)==0){}
  // 6) enable use of PLL by clearing BYPASS
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2;
		
	//Heartbeat LED debug init, blue LED
		SYSCTL_RCGCGPIO_R |= 0x00000020; //ACTIVATE CLOCK FOR PORT F
		while((SYSCTL_PRGPIO_R&0x00000020)==0) //WAIT
		{
		}
		GPIO_PORTF_CR_R = 0x08; //PORT F PIN 3
		GPIO_PORTF_DIR_R = 0x08; //PIN 3 OUTPUT
		GPIO_PORTF_DEN_R = 0x08; //DIGITAL IO
}
