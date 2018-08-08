;****************** main.s ***************
; Program written by: ***Alamin Ahmed & Tu Tran***
; Date Created: 6/22/2017
; Last Modified: 6/22/2017
; Brief description of the program
;   The LED toggles at 8 Hz and a varying duty-cycle
; Hardware connections (External: One button and one LED)
;  PE1 is Button input  (1 means pressed, 0 means not pressed)
;  PE0 is LED output (1 activates external9 LED on protoboard)
;  PF4 is builtin button SW1 on Launchpad (Internal) 
;        Negative Logic (0 means pressed, 1 means not pressed)
; Overall functionality of this system is to operate like this
;   1) Make PE0 an output and make PE1 and PF4 inputs.
;   2) The system starts with the the LED toggling at 8Hz,
;      which is 8 times per second with a duty-cycle of 20%.
;      Therefore, the LED is ON for (0.2*1/8)th of a second
;      and OFF for (0.8*1/8)th of a second.
;   3) When the button on (PE1) is pressed-and-released increase
;      the duty cycle by 20% (modulo 100%). Therefore for each
;      press-and-release the duty cycle changes from 20% to 40% to 60%
;      to 80% to 100%(ON) to 0%(Off) to 20% to 40% so on
;   4) Implement a "breathing LED" when SW1 (PF4) on the Launchpad is pressed:
;      a) Be creative and play around with what "breathing" means.
;         An example of "breathing" is most computers power LED in sleep mode
;         (e.g., https://www.youtube.com/watch?v=ZT6siXyIjvQ).
;      b) When (PF4) is released while in breathing mode, resume blinking at 8Hz.
;         The duty cycle can either match the most recent duty-
;         cycle or reset to 20%.
;      TIP: debugging the breathing LED algorithm and feel on the simulator is impossible.

;	   \\ 
;	   (o>
;	\\_//)
; 	 \_/_)
;	  _|_

;---------------------------------------------------------------- Definition ---------------------------------------------------------------
; PortE device registers	
GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_AFSEL_R EQU 0x40024420
GPIO_PORTE_DEN_R   EQU 0x4002451C
; PortF device registers
GPIO_PORTF_DATA_R  EQU 0x400253FC
GPIO_PORTF_DIR_R   EQU 0x40025400
GPIO_PORTF_AFSEL_R EQU 0x40025420
GPIO_PORTF_PUR_R   EQU 0x40025510
GPIO_PORTF_DEN_R   EQU 0x4002551C
;peripheral clock registers
SYSCTL_RCGCGPIO_R  EQU 0x400FE608
;address for the pins PE0, PE1, and PF4
;can be used instead of ..._DATA_R, this way we don't have to mask to specific pin
PE0 EQU 0x40024004
PE1	EQU 0x40024008  ;0x02
PF4 EQU	0x40025040
;------------------------------------------------------------ End of Definition -----------------------------------------------------------
		THUMB							;using THUMB assembly instruction set
			
		IMPORT PLL_Init					;From MCU_Clock to setup 80MHz processor clock 
		IMPORT Delay_10us				;From SysTick to delay 10us
		IMPORT SysTick_Init				;From SysTick to initialize Ticks for Delay
		IMPORT Delay_1250ns				;From SysTick to delay 1250ns
			
		AREA DATA,ALIGN=2
			
DataBuffer  	SPACE 		50			;Storing the PE1 (the button) and the PE0 (LED) state for debugging 
TimeBuffer 		SPACE 		4*50		;Storing the time stamp of each state for debugging 
DataPt 			SPACE 		4			;DataBuffer pointer for the 8 bit array
TimePt 			SPACE 		4			;TimeBuffer pointer for the 32 bit array
	
LedOnTime		SPACE 		1			;Store the LedOnTime (8 bit) in the RAM
LedOffTime		SPACE 		1			;Store the LedOffTime (8 bit) in the RAM

		AREA    |.text|, CODE, READONLY, ALIGN=2
			
		EXPORT  Start
			
;------------------------------------------------------------------ Start ------------------------------------------------------------------
Start
		BL Debug_Init
		BL PLL_Init						;Setup the system clock
		BL SysTick_Init					;Setup the Delay Function
		BL init							;Initialize all the pins
		BL init_variables				;Initialize global variables
;------------------------------------------------------------------ Loop -------------------------------------------------------------------
loop  
		BL check_pin_F4					;Check to see if the button SW1 was pressed, If yes then implement Breath
		BL check_pin_E1					;Check to see if the button on PE1 was pressed
		BL blink_8Hz					;Blink LED at 8Hz		
		B    loop						;Infinite loop
;-------------------------------------------------------------- End of Loop ----------------------------------------------------------------

;--------------------------------------------------------------- Debug_Init ----------------------------------------------------------------
Debug_Init  
		LDR R2,=DataBuffer				;R1 = &DataBuffer
		LDR R3,=TimeBuffer
		LDR R1,=DataBuffer
		ADD R1,#50
Debug_Init_loop
		MOV R0,#0xFF
		STRB R0,[R2]
		ADD R2,#1
		MOV R0,#0xEEEEEEEE
		STR R0,[R3]
		ADD R3,#4
		CMP R3, R1
		BLO Debug_Init_loop
		
		
;----------------------------------------------------------- End of Debug_Init -------------------------------------------------------------

;-------------------------------------------------------------- check_pin_F4 ---------------------------------------------------------------
check_pin_F4 
		PUSH {LR}						;Save the LR
		MOV R2, #600					;R2 = 600, running at 166.66 Hz
		MOV R3, R2						;R3 = R2
check_btn
		LDR R1, =PF4					;R1 = &PF4
		LDR R0, [R1]					;R0 = the value of the address pointed by R1
		CMP	R0, #0						;Check to see if the pin is HIGH
		BEQ	start_breath				;If LOW then go to start_breath	
;End	
		POP {PC}						;Restore LR to PC, act as BX LR
start_breath
		MOV R0, R3						;R0 = R3, for the delay function
		BL Delay_10us					;Delay R0*10us
		BL toggle_led					;turn on/off the LED
		RSB R0, R3, #600				;Reverse Subtract, R0 = 600 - R3
		BL Delay_10us					;Delay R0*10us
		BL toggle_led					;turn on/off the LED
		CMP R2, #0						;compare R2 with 0
		BHI breath_down					;If higher than 0 then go to breath_down
		CMP R3, #600					;compare R3 with 600
		BLO breath_up					;If less than 30 then go to breath_up
;Reset		
		MOV R2, #600					;R2 = 600
		MOV R3, R2						;R3 = R2
		B check_btn						;Go Back to check the button
breath_down
		SUB R2, #1						;R2 = R2 - 1
		MOV R3, R2						;R3 = R2
		B check_btn						;Go Back to check the button
breath_up
		ADDS R3, #1						;R3 = R3 + 1
		B check_btn						;Go Back to check the button
;---------------------------------------------------------- End of check_pin_F4 ------------------------------------------------------------

;--------------------------------------------------------------- blink_8Hz -----------------------------------------------------------------
;Each paulse takes 125ms
blink_8Hz
		PUSH {LR}						;Save the LR
		BL led_on						;Turn on the LED
		LDR R1, =LedOnTime				;R1 = &LedOnTime
		LDRB R0, [R1]					;R0 = the value of the address pointed by R1, B for 8 bit
		BL Delay_1250ns					;Delay (R0*1250)ns
		BL led_off						;Turn off the LED
		LDR R1, =LedOffTime				;R1 = &LedOffTime
		LDRB R0, [R1]					;R0 = the value of the address pointed by R1, B for 8 bit
		BL Delay_1250ns					;Delay (R0*1250)ns
;End	
		POP {PC}						;Restore LR to PC, act as BX LR
;		BX LR							;Go Back
;------------------------------------------------------------ End of blink_8Hz -------------------------------------------------------------

;--------------------------------------------------------------- switch_duty ---------------------------------------------------------------
switch_duty
		MOV R0, #20000					;R0 = 20000
		BL Delay_10us					;Debounch Delay 20000*10us = 200ms
		LDR R1, =LedOnTime				;R1 = &LedOnTime
		LDRB R0, [R1]					;R0 = the value of the address pointed by R1, B for 8 bit
		CMP R0, #100					;Check to see if R0 = 100
		BEQ reset_duty					;If equal then go to reset_duty
		ADD R0, #20						;R0 = R0 + 20
		STRB R0, [R1]					;Store Value of R0 into the address of R1, B for 8 bit		
		LDR R1, =LedOffTime				;R1 = &LedOffTime
		LDRB R0, [R1]					;R0 = the value of the address pointed by R1, B for 8 bit
		SUB R0, #20						;R0 = R0 - 20
		STRB R0, [R1]					;Store Value of R0 into the address of R1, B for 8 bit	
		B End_check_pin_E1				;Go Back to check_pin_E1
;---------------------------------------------------------- End of switch_duty ------------------------------------------------------------

;-------------------------------------------------------------- reset_duty ----------------------------------------------------------------
reset_duty
		LDR R1, =LedOnTime				;R1 = &LedOnTime
		MOV R0, #0						;R0 = 1
		STRB R0, [R1]					;Store Value of R0 into the address of R1, B for 8 bit
		LDR R1, =LedOffTime				;R1 = &LedOffTime
		MOV R0, #100					;R0 = 1
		STRB R0, [R1]					;Store Value of R0 into the address of R1, B for 8 bit	
		B End_check_pin_E1				;Go Back to check_pin_E1
;----------------------------------------------------------- End of reset_duty -------------------------------------------------------------

;-------------------------------------------------------------- check_pin_E1 ---------------------------------------------------------------
check_pin_E1
		PUSH {LR}						;Save the LR
		LDR R1, =GPIO_PORTE_DATA_R		;R1 = &GPIO_PORTE_DATA_R
		LDR R0, [R1]					;R0 = the value of the address pointed by SYSCTL_RCGCGPIO_R
		AND	R0, R0, #0x02				;Mask bit 2 for pin E1
		CMP	R0, #0x02					;Check to see if the pin is HIGH
		BEQ	switch_duty					;If HIGH then go to switch_state 
End_check_pin_E1
		POP {PC}						;Restore LR to PC, act as BX LR
;		BX LR							;Go Back
;---------------------------------------------------------- End of check_pin_E1 ------------------------------------------------------------

;---------------------------------------------------------------- led_on -------------------------------------------------------------------
led_on
		LDR R1, =GPIO_PORTE_DATA_R		;R1 = &GPIO_PORTE_DATA_R
		LDR R0, [R1]					;R0 = the value of the address pointed by SYSCTL_RCGCGPIO_R
		ORR	R0, R0, #0x01				;Set bit 1 for pin E0
		STR R0, [R1]					;Store Value of R0 into the address of R1
;End	
		BX LR							;Go Back
;------------------------------------------------------------- End of led_on ---------------------------------------------------------------

;---------------------------------------------------------------- led_off ------------------------------------------------------------------
led_off
		LDR R1, =GPIO_PORTE_DATA_R		;R1 = &GPIO_PORTE_DATA_R
		LDR R0, [R1]					;R0 = the value of the address pointed by SYSCTL_RCGCGPIO_R
		AND	R0, R0, #~0x01				;Reset bit 1 for pin E0
		STR R0, [R1]					;Store Value of R0 into the address of R1
;End	
		BX LR							;Go Back
;------------------------------------------------------------ End of led_off ---------------------------------------------------------------

;--------------------------------------------------------------- toggle_led ----------------------------------------------------------------
toggle_led
		LDR R1, =GPIO_PORTE_DATA_R		;R1 = &GPIO_PORTE_DATA_R
		LDR R0, [R1]					;R0 = the value of the address pointed by SYSCTL_RCGCGPIO_R
		EOR	R0, R0, #0x01				;Toggle bit 1 for pin E0
		STR R0, [R1]					;Store Value of R0 into the address of R1
;End	
		BX LR							;Go Back
;----------------------------------------------------------- End of toggle_led -------------------------------------------------------------

;------------------------------------------------------------- init_variables --------------------------------------------------------------
init_variables
		LDR R1, =LedOnTime				;R1 = &LedOnTime
		MOV R0, #20						;R0 = 1
		STRB R0, [R1]					;Store Value of R0 into the address of R1, B for 8 bit
		LDR R1, =LedOffTime				;R1 = &LedOffTime
		MOV R0, #80						;R0 = 1
		STRB R0, [R1]					;Store Value of R0 into the address of R1, B for 8 bit
;End	
		BX LR							;Go Back
;--------------------------------------------------------- End of init_variables -----------------------------------------------------------

;----------------------------------------------------------------- init --------------------------------------------------------------------
init
;PortF_and_E_Clock						;Activate clock for Port F and E
		LDR R1, =SYSCTL_RCGCGPIO_R		;R1 = &SYSCTL_RCGCGPIO_R
		LDR R0, [R1]					;R0 = the value of the address pointed by SYSCTL_RCGCGPIO_R
		ORR	R0, R0, #0x30				;Set bit 5 (for F) and bit 4 (for E) to turn on the clock
		STR R0, [R1]					;Store Value of R0 into the address of R1
		NOP								;no operation (1 clock cycle), requre 2 clock cycle to finish setting up
		NOP								;no operation (1 clock cycle)
		NOP
		NOP
		
;PortF4_DIR_setup						;Activate pin F4 as Input
		LDR R1, =GPIO_PORTF_DIR_R		;R1 = &GPIO_PORTF_DIR_R
		LDR R0, [R1]					;R0 = the value of the address pointed by GPIO_PORTF_DIR_R
		AND	R0, R0, #~0x10				;Set bit 4 (for F4), 0 means input 1 means output (this ensure only bit 4 changes)
		STR R0, [R1]					;Store Value of R0 into the address of R1
		
;PortF4_PUR_setup						;Add pull-up configuration on F4
		LDR R1, =GPIO_PORTF_PUR_R		;R1 = &GPIO_PORTF_PUR_R
		LDR R0, [R1]					;R0 = the value of the address pointed by GPIO_PORTF_PUR_R
		ORR	R0, R0, #0x10				;Set bit 4 (for F4),1 means pull-up 0 means none (this ensure only bit 4 changes)
		STR R0, [R1]					;Store Value of R0 into the address of R1
		
;PortF4_DEN_setup						;Enable F4
		LDR R1, =GPIO_PORTF_DEN_R		;R1 = &GPIO_PORTF_DEN_R
		LDR R0, [R1]					;R0 = the value of the address pointed by GPIO_PORTF_DEN_R
		ORR	R0, R0, #0x10				;set bit 4 (for F4),1 means enable 0 means disable (this ensure only bit 4 changes)
		STR R0, [R1]					;Store Value of R0 into the address of R1
		
;PortE0_and_E1_DIR_setup				;Activate pin E0 Output (LED) and E1 as Input
		LDR R1, =GPIO_PORTE_DIR_R		;R1 = &GPIO_PORTE_DIR_R
		LDR R0, [R1]					;R0 = the value of the address pointed by GPIO_PORTE_DIR_R
		AND	R0, R0, #~0x02				;Set bit 1 (for E1), 0 means input 1 means output (this ensure only bit 1 changes)
		ORR	R0, R0, #0x01				;Set bit 0 (for E0),0 means input 1 means output (this ensure only bit 0 changes)
		STR R0, [R1]					;Store Value of R0 into the address of R1
		
;PortE0_and_E1_DEN_setup				;Enable E0 and E1
		LDR R1, =GPIO_PORTE_DEN_R		;R1 = &GPIO_PORTE_DEN_R
		LDR R0, [R1]					;R0 = the value of the address pointed by GPIO_PORTE_DEN_R
		ORR	R0, R0, #0x03				;Set bit 0 and 1 (for E0 and E1),1 means enable 0 means disable (this ensure only bit 0 and 1 changes)
		STR R0, [R1]					;Store Value of R0 into the address of R1
;End	
		BX LR							;Go Back
;------------------------------------------------------------------ End of init --------------------------------------------------------------

		ALIGN      ; make sure the end of this section is aligned
		END        ; end of file

