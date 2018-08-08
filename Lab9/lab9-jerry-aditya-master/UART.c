// UART1.c
// Runs on LM4F120/TM4C123
// Use UART1 to implement bidirectional data transfer to and from 
// another microcontroller in Lab 9.  This time, interrupts and FIFOs
// are used.


// U1Rx (VCP receive) connected to PC4
// U1Tx (VCP transmit) connected to PC5
#include <stdint.h>
#include "FiFo.h"
#include "UART.h"
#include "tm4c123gh6pm.h"

uint32_t DataLost; 
int RxCounter;
int ErrorCount;
// Initialize UART1
// Baud rate is 115200 bits/sec
// Make sure to turn ON UART1 Receiver Interrupt (Interrupt 6 in NVIC)
// Write UART1_Handler
void UART_Init(void){
	// write UART1 init with Port C 4 and Port C 5
	SYSCTL_RCGCUART_R |= 0x00002; // activate UART Clock
	SYSCTL_RCGCGPIO_R |= 0x0004; // activate port C
	FiFo_Init();
	volatile int delay;
	delay++; delay++; delay--;delay--;
	UART1_CTL_R &= ~ 0x0001; //disable UART
	UART1_IBRD_R = 43; // 80MHz
	// IBRD = init(80000000/16*115,200)) = int (43.4028)
	UART1_FBRD_R = 26;
	// FBRD = round ((0.4028*64)) =26 ;
	UART1_LCRH_R = 0x70; // 8-bit length, enable FIFO?
	UART1_IFLS_R = 0x00010; //1/2 full
	UART1_IM_R |= 0x10;
	NVIC_PRI1_R &= 0xFF1FFFFF; // set priority to 1
	NVIC_EN0_R=NVIC_EN0_INT6; // interrupt 6
	UART1_CTL_R |= 0x301; //enable RXE, Tx and UART
	GPIO_PORTC_AFSEL_R |= 0x30; // PC 5 and PC 4
	GPIO_PORTC_DEN_R|= 0x30; //digital I/O pn PC 5 and PC 4
	GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xFF00FFFF) + 0x00220000; 
	GPIO_PORTC_AMSEL_R &= ~0x30; //No analog on PC 4 and PC 5
	ErrorCount =0;  // clearing global error count
}

// input ASCII character from UART
// spin if RxFifo is empty
char UART_InChar(void){
	while((UART1_FR_R&0x0010)!=0){/* wait until RXFE is 0*/}
	return((unsigned char)(UART1_DR_R&0xFF));
}
//------------UART1_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART_OutChar(char data){
  while((UART1_FR_R&0x0020)!=0);// waiting till TXFF is 0
	UART1_DR_R = data; 
}

// hardware RX FIFO goes from 7 to 8 or more items
// UART receiver Interrupt is triggered; This is the ISR
int full=0;
void UART1_Handler(void){
  // --UUU-- complete with your code
	GPIO_PORTF_DATA_R ^= 0x04;
	GPIO_PORTF_DATA_R ^= 0x04;
	while((UART1_FR_R & 0x0010) == 0 ) {
		full = FiFo_Put(UART1_DR_R& 0xFF);
		if(full ==0)
			ErrorCount++;
	}
	RxCounter++;
	UART1_ICR_R = 0x10; // clear to acknowledge the interrupt
	GPIO_PORTF_DATA_R ^= 0x04;
}
