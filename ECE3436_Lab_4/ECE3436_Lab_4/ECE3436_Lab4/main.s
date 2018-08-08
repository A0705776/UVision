;****************** main.s ***************
; Program written by: *** ... ***
; Date Created: ...
; Last Modified: ...
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

; Please feel free to add your favorite symbol here (:p)
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
			
;DataBuffer  	Storing the PE1 (the button) and the PE0 (LED) state for debugging 
;TimeBuffer 	Storing the time stamp of each state for debugging 
;DataPt 		DataBuffer pointer for the 8 bit array
;TimePt 		TimeBuffer pointer for the 32 bit array
;DebugCounter	Debug_Capture counter (7 times)
	
;LedOnTime		Store the LedOnTime (8 bit) in the RAM
;LedOffTime		Store the LedOffTime (8 bit) in the RAM

		AREA    |.text|, CODE, READONLY, ALIGN=2
			
		EXPORT  Start
			
;------------------------------------------------------------------ Start ------------------------------------------------------------------
Start
		BL Debug_Init					;Initialize Debug
		BL PLL_Init						;Setup the system clock
		BL SysTick_Init					;Setup the Delay Function***
		BL init_pins					;Initialize all the pins***
		BL init_variables				;Initialize global variables***
;------------------------------------------------------------------ Loop -------------------------------------------------------------------
loop  
		BL check_pin_E1					;Check to see if the button on PE1 was pressed
		BL blink_8Hz					;Blink LED at 8Hz
		BL toggle_blue_led				;Heartbeat debugging
		B  loop							;Infinite loop
;-------------------------------------------------------------- End of Loop ----------------------------------------------------------------

;------------------------------------------------------------- Debug_Capture ---------------------------------------------------------------
Debug_Capture 
	;Write your code here
		BX LR							;Go Back			
;--------------------------------------------------------- End of Debug_Capture ------------------------------------------------------------

;--------------------------------------------------------------- Debug_Init ----------------------------------------------------------------
Debug_Init 
	;Write your code here	
		BX LR							;Go Back			
;----------------------------------------------------------- End of Debug_Init -------------------------------------------------------------

;--------------------------------------------------------------- blink_8Hz -----------------------------------------------------------------
;Each paulse takes 125ms
blink_8Hz
	;Write your code here
		BX LR							;Go Back
;------------------------------------------------------------ End of blink_8Hz -------------------------------------------------------------

;-------------------------------------------------------------- check_pin_E1 ---------------------------------------------------------------
check_pin_E1
	;Write your code here
		BX LR							;Go Back
;---------------------------------------------------------- End of check_pin_E1 ------------------------------------------------------------

;------------------------------------------------------------ toggle_blue_led --------------------------------------------------------------
toggle_blue_led
	;Write your code here
		BX LR							;Go Back
;--------------------------------------------------------- End of toggle_blue_led ----------------------------------------------------------

;------------------------------------------------------------- init_variables --------------------------------------------------------------
init_variables
	;Write your code here
		BX LR							;Go Back
;--------------------------------------------------------- End of init_variables -----------------------------------------------------------

;----------------------------------------------------------------- init --------------------------------------------------------------------
init_pins
	;Write your code here
		BX LR							;Go Back
;------------------------------------------------------------------ End of init --------------------------------------------------------------

		ALIGN      ; make sure the end of this section is aligned
		END        ; end of file

