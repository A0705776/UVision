;SysTick INIT (refer to Pg174 of the textbook)
;this file contains 2 functions, SysTick_INIT and Delay_10us and Delay_1250ns
;---------------------------------------------------------------- Definition ---------------------------------------------------------------
; NVIC REGISTERS
NVIC_ST_CTRL_R	   EQU 0xE000E010
NVIC_ST_RELOAD_R   EQU 0xE000E014
NVIC_ST_CURRENT_R  EQU 0xE000E018
Time_10us		   EQU 800
Time_1250ns		   EQU 100000
;------------------------------------------------------------ End of Definition -----------------------------------------------------------
	
		AREA	|.text|, CODE, READONLY, ALIGN = 2
		THUMB
		EXPORT SysTick_Init
		EXPORT Delay_10us
		EXPORT Delay_1250ns
;--------------------------------------------------------------- SysTick_INIT -------------------------------------------------------------
SysTick_Init
	;Write your code here
		BX LR						;Return
;----------------------------------------------------------- End of SysTick_INIT ----------------------------------------------------------

;------------------------------------------------------------------ Delay -----------------------------------------------------------------
; Time delay using busy wait. 
; Input: R0  delay parameter in units of the core clock (12.5 nsec) 
; Output: none     ; Modifies: R0, R1, R3
Delay
	;Write your code here
		BX LR						;Return
;--------------------------------------------------------------- End of Delay -------------------------------------------------------------

;----------------------------------------------------------------- Delay_10us --------------------------------------------------------------
; 10us delay using Delay function 
; Input: R0  delay parameter in units of the 10us delay  
; Output: none     ; Modifies: R0, R1 
Delay_10us
	;Write your code here
		BX LR						;Return
;------------------------------------------------------------- End of Delay_10us -----------------------------------------------------------

;--------------------------------------------------------------- Delay_1250ns -------------------------------------------------------------
; 1250ns delay using Delay function 
; Input: R0  delay parameter in units of the 12.5ms delay  
; Output: none     ; Modifies: R0, R1 
Delay_1250ns
	;Write your code here
		BX LR						;Return
;------------------------------------------------------------ End of Delay_1250ns ---------------------------------------------------------


		ALIGN
		END