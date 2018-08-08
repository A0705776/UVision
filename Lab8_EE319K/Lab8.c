// Lab8.c
// Runs on LM4F120 or TM4C123

// Analog Input connected to PE2=ADC1
// displays on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats

#include <stdint.h>

#include "ST7735.h"
#include "TExaS.h"
#include "ADC.h"
#include "print.h"
#include "tm4c123gh6pm.h"

int ADCMail;
int ADCStatus;
uint32_t Data;        // 12-bit ADC
uint32_t Position;    // 32-bit fixed-point 0.001 cm
void SysTick_Init(void);
void SysTick_Handler(void);

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on Nokia
// main3 adds your convert function, position data is no Nokia

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x20; 						//port F clock
	while( (SYSCTL_PRGPIO_R&0x20)==0){};	//wait for the clock
	GPIO_PORTF_DIR_R |= 0x0E;							//output
	GPIO_PORTF_DEN_R |= 0x0E;							//digital i/o
}



uint32_t Convert(uint32_t input){ //Convert reading from ADC_In to distance along slide pot
  double slope = 0.47806; 			//y = 0.0046806x+0.093609
	double intercept = 93.509;		//r = 0.992
	int result = (int) (slope*input + intercept);
	
	return result;
	
}
  
int main(void){
  TExaS_Init();//80MHz clk
  // your Lab 8
	DisableInterrupts();
	ST7735_InitR(INITR_REDTAB);
	PortF_Init();
	ADC_Init();
	SysTick_Init();
	EnableInterrupts();
	while( (SYSCTL_PRGPIO_R&0x20)==0){};//delay
  while(1){
		while(ADCStatus == 0){}
		Data = ADCMail;
		ADCStatus = 0;
    ST7735_SetCursor(0,0);
    LCD_OutDec(Data); ST7735_OutString("    "); 
    ST7735_SetCursor(6,0);
    LCD_OutFix(Convert(Data)); ST7735_OutString(" cm"); 
  }
}

void SysTick_Init(void){
  NVIC_ST_CTRL_R &=0x0;						//disable systic during setup
	NVIC_ST_RELOAD_R = 800000; 			//10 ms = 400,000 for 40mhz
	NVIC_ST_CURRENT_R = 0;					//clear current
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; //Set priority 2
	NVIC_ST_CTRL_R = 0x07;					//enable with core clock
}

void SysTick_Handler(void){
	PF2 ^= 0x04;										//heartbeat
	ADCMail = ADC_In();							//get adc
	ADCStatus = 1;									//start displaying;
}

