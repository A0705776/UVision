// Lab9.c
// Runs on LM4F120 or TM4C123
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 4/10/2017 

// Analog Input connected to PE2=ADC1
// displays on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats
// UART1 on PC4-5
// * Start with where you left off in Lab8. 
// * Get Lab8 code working in this project.
// * Understand what parts of your main have to move into the UART1_Handler ISR
// * Rewrite the SysTickHandler
// * Implement the s/w FiFo on the receiver end 
//    (we suggest implementing and testing this first)

#include <stdint.h>

#include "ST7735.h"
#include "TExaS.h"
#include "ADC.h"
#include "print.h"
#include "tm4c123gh6pm.h"
#include "UART.h"
#include "FiFo.h"

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on Nokia
// main3 adds your convert function, position data is no Nokia

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
uint32_t Data;      // 12-bit ADC
uint32_t Position;  // 32-bit fixed-point 0.001 cm
uint32_t TxCounter;
uint32_t Mail;
uint32_t Status;
uint32_t RxCounter;


// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
	//Turn on clock for port f
	SYSCTL_RCGCGPIO_R |= 0x20;
//Wait for clock to stabilize using while loop
	while((SYSCTL_RCGCGPIO_R & 0x20) == 0){}
//initialize PE2
	GPIO_PORTF_DIR_R   |= 0x0E;
	GPIO_PORTF_DEN_R 	 |= 0x0E;
	GPIO_PORTF_AFSEL_R &= ~0x0E;
	GPIO_PORTF_PUR_R  |= 0x0E;

// Intialize PortF for hearbeat
}

// Get fit from excel and code the convert routine with the constants
// from the curve-fit
uint32_t Convert(uint32_t input){
	uint32_t num;
	num=((input*(439))/1000)+178;
  return num; //replace with your calibration code
}


/* SysTick ISR
*/
void SysTick_Init(uint32_t period){
NVIC_ST_CTRL_R = 0; // disable SysTick during setup
NVIC_ST_RELOAD_R = period - 1;// reload value
NVIC_ST_CURRENT_R = 0; // any write to current clears it
NVIC_ST_CTRL_R = 0x00000007; // enable with core clock and interrupts
}
void SysTick_Handler(void){ // every 25 ms
 uint32_t ADC_Message[8];
	
//heartbeat toggle on or off
	PF3^=0xFF;
	PF3^=0xFF;
// Sample ADC
	Mail=ADC_In();
	//Set Flag
	Status=1;
	//Toggle Heartbeat again
	PF3^=0xFF;
//Send measurements to UART_OutChar 
//First convert data from Mail
	Mail=Convert(Mail);
//put what message is transmitted
	ADC_Message[0] = 0x02; //start with 0x02
	ADC_Message[1] = (Mail/1000); //n/1000
	ADC_Message[1] =ADC_Message[1]+ 0x30; // n/1000+0x30
	ADC_Message[2] = 0x2E;
	ADC_Message[3] = Mail/100; //n/100
	ADC_Message[3] = ADC_Message[3]+0x30; //n/100+0x30
	ADC_Message[4] = Mail/10; // n/10
	ADC_Message[4] =ADC_Message[4]+ 0x30; //n/10+0x30
	ADC_Message[5] = Mail; //n
	ADC_Message[5] = ADC_Message[5]+ 0x30; //n+0x30
	ADC_Message[6] = 0x0D; // '\r'
	ADC_Message[7] = 0x03; //end with 0x03
	//for loop to add info to message array
	for(uint32_t i=0; i<8; i++){
		UART_OutChar(ADC_Message[i]);}
	//increment TxCounter
	TxCounter++;
	//Toggle Heartbeat
	PF3^=0xFF;
}
// hardware RX FIFO goes from 7 to 8 or more items
// UART receiver Interrupt is triggered; This is the ISR
void UART1_Handler(void){
//Toggle Heart Beat 
  PF2 ^= 0xFF;
	PF2 ^= 0xFF;
	while((UART1_FR_R & 0x0010) == 0){
		FiFo_Put(UART_InChar());
	}
	RxCounter++;
	UART1_ICR_R = 0x10;   // this clears bit 4 (RXRIS) in the RIS register
	PF2 ^= 0xFF;
}

//Status allows us to see when FIFO is empty
	char data;
  uint32_t FiFo_Get(char* datapt);


// final main program for bidirectional communication
// Sender sends using SysTick Interrupt
// Receiver receives using RX
int main(void){ 
  
  TExaS_Init();       // Bus clock is 80 MHz 
  ST7735_InitR(INITR_REDTAB);
  ADC_Init();    // initialize to sample ADC
  PortF_Init();
  UART_Init();       // initialize UART
	ST7735_SetCursor(0,0);
  LCD_OutFix(0);
  ST7735_OutString(" cm");
 // Enable SysTick Interrupt by calling SysTick_Init()
  SysTick_Init(40);
 EnableInterrupts();
  while(1){
		//--UUU--Complete this  - see lab manual
	}
}
/*
uint32_t Status[20];             // entries 0,7,12,19 should be false, others true
char GetData[10];  // entries 1 2 3 4 5 6 7 8 should be 1 2 3 4 5 6 7 8
int mainfifo(void){ // Make this main to test FiFo
  FiFo_Init(); // Assuming a buffer of size 6
  for(;;){
    Status[0]  = FiFo_Get(&GetData[0]);  // should fail,    empty
    Status[1]  = FiFo_Put(1);            // should succeed, 1 
    Status[2]  = FiFo_Put(2);            // should succeed, 1 2
    Status[3]  = FiFo_Put(3);            // should succeed, 1 2 3
    Status[4]  = FiFo_Put(4);            // should succeed, 1 2 3 4
    Status[5]  = FiFo_Put(5);            // should succeed, 1 2 3 4 5
    Status[6]  = FiFo_Put(6);            // should succeed, 1 2 3 4 5 6
    Status[7]  = FiFo_Put(7);            // should fail,    1 2 3 4 5 6 
    Status[8]  = FiFo_Get(&GetData[1]);  // should succeed, 2 3 4 5 6
    Status[9]  = FiFo_Get(&GetData[2]);  // should succeed, 3 4 5 6
    Status[10] = FiFo_Put(7);            // should succeed, 3 4 5 6 7
    Status[11] = FiFo_Put(8);            // should succeed, 3 4 5 6 7 8
    Status[12] = FiFo_Put(9);            // should fail,    3 4 5 6 7 8 
    Status[13] = FiFo_Get(&GetData[3]);  // should succeed, 4 5 6 7 8
    Status[14] = FiFo_Get(&GetData[4]);  // should succeed, 5 6 7 8
    Status[15] = FiFo_Get(&GetData[5]);  // should succeed, 6 7 8
    Status[16] = FiFo_Get(&GetData[6]);  // should succeed, 7 8
    Status[17] = FiFo_Get(&GetData[7]);  // should succeed, 8
    Status[18] = FiFo_Get(&GetData[8]);  // should succeed, empty
    Status[19] = FiFo_Get(&GetData[9]);  // should fail,    empty
  }
}

*/