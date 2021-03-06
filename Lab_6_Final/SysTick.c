// SysTick.c
// Implements two busy-wait based delay routines
#include <stdint.h>
// Initialize SysTick with busy wait running at bus clock.
#define NVIC_ST_CTRL_R      (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))
void SYSTICK_INIT(void)
{
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = 0x00FFFFFF;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0x00000005;
}
// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SYSTICK_WAIT(uint32_t delay)
{
	NVIC_ST_RELOAD_R = delay - 1;
	NVIC_ST_CURRENT_R = 0;
	while((NVIC_ST_CTRL_R&0x00010000)==0)
	{
	}
}

// Time delay using busy wait.
// waits for count*10ms
// 10000us equals 10ms
void SYSTICK_10MS(uint32_t delay)
{
	uint32_t i;
	for(i=0; i < delay; i++)
	{
		SYSTICK_WAIT(800000);
	}
}

