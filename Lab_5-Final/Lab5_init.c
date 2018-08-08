//Lab 5 initialization
//Set clock to 80MHz
//Initialize the pins on portB

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#define SYSDIV2 4


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
  while((SYSCTL_RIS_R&SYSCTL_RIS_PLLLRIS)==0){};
  // 6) enable use of PLL by clearing BYPASS
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2;
		
	//PortB init
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
