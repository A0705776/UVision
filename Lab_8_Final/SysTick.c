//SysTick.c
//Timer Periodic Interrupt
//By Tu Tran
//40Hz periodic interrupt
//32bit Timer 0, A, T0CCP0
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ADC.h"
#include "SysTick.h"
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
uint32_t Measure;
void DisableInterrupts(void);
void EnableInterrupts(void);
//Initialization
void SysTick_init(uint32_t frequency)
{
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = 80000000/frequency - 1;
	NVIC_ST_CURRENT_R = 0;
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x40000000;
	NVIC_ST_CTRL_R = 0x00000007;
	EnableInterrupts();
}

//Timer0A interrupt handler
void SysTick_Handler(void)
{
	PF2 ^= 0x04;
	Measure = ADC_In();
	Measure = (Measure/4096)*3000;
}
