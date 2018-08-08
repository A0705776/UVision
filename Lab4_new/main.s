;****************** main.s ***************
; Program written by: ***Alamin Ahmed & Tu Tran***
; Date Created: 6/22/2017
; Last Modified: 6/26/2017
; Brief description of the program
;   The LED toggles at 8 Hz and a varying duty-cycle
;   Repeat the functionality from Lab2-3 but now we want you to 
;   insert debugging instruments which gather data (state and timing)
;   to verify that the system is functioning as expected.
; Hardware connections (External: One button and one LED)
;  PE1 is Button input  (1 means pressed, 0 means not pressed)
;  PE0 is LED output (1 activates external LED on protoboard)
;  PF2 is Blue LED on Launchpad used as a heartbeat
; Instrumentation data to be gathered is as follows:
; After Button(PE1) press collect one state and time entry. 
; After Buttin(PE1) release, collect 7 state and
; time entries on each change in state of the LED(PE0): 
; An entry is one 8-bit entry in the Data Buffer and one 
; 32-bit entry in the Time Buffer
;  The Data Buffer entry (byte) content has:
;    Lower nibble is state of LED (PE0)
;    Higher nibble is state of Button (PE1)
;  The Time Buffer entry (32-bit) has:
;    24-bit value of the SysTick's Current register (NVIC_ST_CURRENT_R)
; Note: The size of both buffers is 50 entries. Once you fill these
;       entries you should stop collecting data
; The heartbeat is an indicator of the running of the program. 
; On each iteration of the main loop of your program toggle the 
; LED to indicate that your code(system) is live (not stuck or dead).

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
PE1	EQU 0x40024008
PF4 EQU	0x40025040
;8 bit Address Length (depends on the placement of the DataBuffer in the RAM, need to 1st thing iitialize)
NEntries EQU 0x20000032
;Systicks timer
NVIC_ST_CURRENT_R  EQU 0xE000E018
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
DebugCounter	SPACE		1			;Save 7 Debug_Capture counter
	
LedOnTime		SPACE 		1			;Store the LedOnTime (8 bit) in the RAM
LedOffTime		SPACE 		1			;Store the LedOffTime (8 bit) in the RAM

		AREA    |.text|, CODE, READONLY, ALIGN=2
			
		EXPORT  Start
			
;------------------------------------------------------------------ Start ------------------------------------------------------------------
Start
		BL Debug_Init					;Initialize Debug
		BL PLL_Init						;Setup the system clock
		BL SysTick_Init					;Setup the Delay Function
		BL init							;Initialize all the pins
		BL init_variables				;Initialize global variables
;------------------------------------------------------------------ Loop -------------------------------------------------------------------
loop  
		;BL check_pin_F4					;Check to see if the button SW1 was pressed, If yes then implement Breath
		BL check_pin_E1					;Check to see if the button on PE1 was pressed
		BL blink_8Hz					;Blink LED at 8Hz
		BL toggle_blue_led				;Heartbeat debugging
		B  loop							;Infinite loop
;-------------------------------------------------------------- End of Loop ----------------------------------------------------------------

;------------------------------------------------------------- Debug_Capture ---------------------------------------------------------------
Debug_Capture 
		PUSH {R0-R6}					;Save the registers
		
		LDR R2, =DebugCounter			;R2 = &DebugCounter
		LDRB R3, [R2]					;R3 = value of R2
		CMP R3,#0						;Compare the counter
		BLS Debug_Capture_End			;If R3 > 0 then go to Debug_Capture
		SUB R3, #1						;Subtruct one from the counter
		STRB R3, [R2]					;Save the counter
		
		LDR R1,=NEntries				;R1 = &NEntries, use for the NEntries of the loop 
		LDR R2,=DataPt					;R2 = &DataPt
		LDR R5, [R2]					;R5 = value of R2
		CMP R5, R1						;Compare R5 with R1
		BHS	Debug_Capture_End			;If R5 >= R1 then exit
		LDR R3,=TimePt					;R3 = &TimePt
		LDR R6, [R3]					;R6 = value of R3
		
		LDR R1,=GPIO_PORTE_DATA_R		;R1 = &GPIO_PORTE_DATA_R
		LDR R0, [R1]					;R0 = the value of the address pointed by R1
		AND R4, R0, #0x02				;R4 = R0 & 00000010
		LSL R4, R4, #3					;R4 = R4 << 3
		AND R0, #0x01					;R0 = R0 & 00000001
		ORR R4, R0						;R4 = R4 | R0
		STRB R4, [R5]					;Store the PE0 and PE1 State
		
		LDR R1,=NVIC_ST_CURRENT_R		;R1 = &NVIC_ST_CURRENT_R
		LDR R0, [R1]					;R0 = the value of the address pointed by R1
		STR R0, [R6] 					;R3 = R0
		ADD R5, #1						;Increment R2 by 1 for 8 bit
		ADD R6, #4						;Increment R3 by 4 for 32 bit
		STR R5, [R2]					;Store R2
		STR R6, [R3]					;Store R3
Debug_Capture_End
		POP {R0-R6}						;Restore the registers and go back
		BX LR							;Go Back			
;--------------------------------------------------------- End of Debug_Capture ------------------------------------------------------------

;--------------------------------------------------------------- Debug_Init ----------------------------------------------------------------
Debug_Init 
		LDR R0,=DataPt					;R0 = &DataPt
		LDR R1,=TimePt					;R1 = &TimePt
		LDR R2,=DataBuffer				;R2 = &DataBuffer
		LDR R3,=TimeBuffer				;R3 = &TimeBuffer
		STR R2, [R0]					;R0 = R2, initializing R0
		STR R3, [R1]					;R1 = R3, initializing R1
		
		LDR R1,=NEntries				;R1 = &NEntries, use for the NEntries of the loop
		
Debug_Init_loop
		MOV R0,#0xFF					;R0 = 0xFF
		STRB R0,[R2]					;R2 = R0, initializing R2
		ADD R2,#1						;R2 = R2 + 1, move to next 8 bit address
		MOV R0,#0xFFFFFFFF				;R0 = 0xFFFFFFFF
		STR R0,[R3]						;R3 = R0, initializing R3
		ADD R3,#4						;R3 = R3 + 4, move to next 32 bit address
		CMP R2, R1						;Compare R2 with R1
		BLO Debug_Init_loop				;If R2 < R1 then go to Debug_Init_loop
;End	
		BX LR							;Go Back			
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
		
		BL Debug_Capture				;If R3 > 0 then go to Debug_Capture
		
		BL led_on						;Turn on the LED
		LDR R1, =LedOnTime				;R1 = &LedOnTime
		LDRB R0, [R1]					;R0 = the value of the address pointed by R1, B for 8 bit
		BL Delay_1250ns					;Delay (R0*1250)ns
		
		BL Debug_Capture				;If R3 > 0 then go to Debug_Capture
		
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
		LDR R2, =DebugCounter			;R1 = &DebugCounter
		MOV R3, #8						;R0 = 8, start Debug_Capture, 8 because we are calling Debug_Capture within this function
		STRB R3, [R2]					;Store Value of R0 into the address of R1, B for 8 bit
		BL Debug_Capture				;Capture Data
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
		LDR R0, [R1]					;R0 = the value of the address pointed by R1
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

;------------------------------------------------------------ toggle_blue_led --------------------------------------------------------------
toggle_blue_led
		LDR R1, =GPIO_PORTF_DATA_R		;R1 = &GPIO_PORTE_DATA_R
		LDR R0, [R1]					;R0 = the value of the address pointed by SYSCTL_RCGCGPIO_R
		EOR	R0, R0, #0x04				;Toggle bit 1 for pin E0
		STR R0, [R1]					;Store Value of R0 into the address of R1
;End	
		BX LR							;Go Back
;--------------------------------------------------------- End of toggle_blue_led ----------------------------------------------------------

;------------------------------------------------------------- init_variables --------------------------------------------------------------
init_variables
		LDR R1, =LedOnTime				;R1 = &LedOnTime
		MOV R0, #20						;R0 = 1
		STRB R0, [R1]					;Store Value of R0 into the address of R1, B for 8 bit
		LDR R1, =LedOffTime				;R1 = &LedOffTime
		MOV R0, #80						;R0 = 1
		STRB R0, [R1]					;Store Value of R0 into the address of R1, B for 8 bit
		LDR R1, =DebugCounter			;R1 = &DebugCounter
		MOV R0, #0						;R0 = 1
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
		
;PortF2_DIR_setup						;Activate pin F2 as Output
		LDR R1, =GPIO_PORTF_DIR_R		;R1 = &GPIO_PORTF_DIR_R
		LDR R0, [R1]					;R0 = the value of the address pointed by GPIO_PORTF_DIR_R
		ORR	R0, R0, #0x04				;Set bit 2 (for F4), 0 means input 1 means output (this ensure only bit 2 changes)
		STR R0, [R1]					;Store Value of R0 into the address of R1
		
;PortF2_DEN_setup						;Enable F4
		LDR R1, =GPIO_PORTF_DEN_R		;R1 = &GPIO_PORTF_DEN_R
		LDR R0, [R1]					;R0 = the value of the address pointed by GPIO_PORTF_DEN_R
		ORR	R0, R0, #0x04				;set bit 2 (for F4),1 means enable 0 means disable (this ensure only bit 2 changes)
		STR R0, [R1]					;Store Value of R0 into the address of R1
		
;PortE0_and_E1_DIR_setup				;Activate pin E0 Output (LED) and E1 as Input
		LDR R1, =GPIO_PORTE_DIR_R		;R1 = &GPIO_PORTE_DIR_R
		LDR R0, [R1]					;R0 = the value of the address pointed by GPIO_PORTE_DIR_R
		AND	R0, R0, #~0x02				;Set bit 1 (for E1), 0 means input 1 means input (this ensure only bit 1 changes)
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

