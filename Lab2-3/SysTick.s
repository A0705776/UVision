;SysTick INIT (refer to Pg174 of the textbook)
;this file contains 2 functions, SysTick_INIT and Delay_10us and Delay_1250ns
;---------------------------------------------------------------- Definition ---------------------------------------------------------------
; NVIC REGISTERS
NVIC_ST_CTRL_R	   EQU 0xE000E010
NVIC_ST_RELOAD_R   EQU 0xE000E014
NVIC_ST_CURRENT_R  EQU 0xE000E018
Time_10us		   EQU 800
Time_1250ns		   EQU 100000
	
	;25ms 100ms
	;50ms 75ms
	;75ms 50ms
	;100ms 25ms
	;125ms 0ms
	;0ms 125ms
;------------------------------------------------------------ End of Definition -----------------------------------------------------------
	
		AREA	|.text|, CODE, READONLY, ALIGN = 2
		THUMB
		EXPORT SysTick_Init
		EXPORT Delay_10us
		EXPORT Delay_1250ns
;--------------------------------------------------------------- SysTick_INIT -------------------------------------------------------------
SysTick_Init
;Disable SysTick
		LDR R1, =NVIC_ST_CTRL_R		;R1 = &NVIC_ST_CTRL_R
		MOV R0, #0					;Disable SysTick during setup by clear it 
		STR R0, [R1]				;Store Value of R0 into the address of R1
;Setting Reload value	
		LDR R1, =NVIC_ST_RELOAD_R	;R1 = &NVIC_ST_RELOAD_R
		LDR R0, =0x00FFFFFF			;Max reload value
		STR R0, [R1]				;Store Value of R0 into the address of R1
;Clears counter	
		LDR R1, =NVIC_ST_CURRENT_R	;R1 = &NVIC_ST_CURRENT_R
		MOV R0, #0					;Any write to current clears it
		STR R0, [R1]				;Store Value of R0 into the address of R1
;Enable SysTick
		LDR R1, =NVIC_ST_CTRL_R		;R1 = &NVIC_ST_CTRL_R
		MOV R0, #0x05				;Enable SysTick by setting bit 2 and 1 with core clock
		STR R0, [R1]				;Store Value of R0 into the address of R1
;End
		BX LR						;Return
;----------------------------------------------------------- End of SysTick_INIT ----------------------------------------------------------

;------------------------------------------------------------------ Delay -----------------------------------------------------------------
; Time delay using busy wait. 
; Input: R0  delay parameter in units of the core clock (12.5 nsec) 
; Output: none     ; Modifies: R0, R1, R3
Delay
		LDR R1, =NVIC_ST_RELOAD_R	;R1 = &NVIC_ST_RELOAD_R
		SUB R0, #1					;Delay - 1
		STR R0, [R1]				;Store Value of R0 into the address of R1
		LDR R1, =NVIC_ST_CTRL_R		;R1 = &NVIC_ST_CTRL_R
Delay_LOOP
		LDR R3, [R1]				;R0 = the value of the address pointed by R1
		ANDS R3, R3, #0x00010000	;Check to see if bit 4 has been set
		BEQ Delay_LOOP				;Loop till bit 4 has been set (12.5ns * R0)
;End
		BX LR						;Return
;--------------------------------------------------------------- End of Delay -------------------------------------------------------------

;----------------------------------------------------------------- Delay_10us --------------------------------------------------------------
; 10us delay using Delay function 
; Input: R0  delay parameter in units of the 10us delay  
; Output: none     ; Modifies: R0, R1 
Delay_10us
		PUSH {R4, R3, LR}			;Save the register R4, R3 and the LR to stack
		MOVS R4, R0					;Move R0 into R4
		BEQ Delay_10us_Done			;If R0 is 0 then go to done
Delay_10us_Loop
		LDR R0, =Time_10us			;Counter that makes 10us delay (12.5ns * 80000 = 10us)
		BL Delay					;Branch to Delay for 10us
		SUBS R4, R4, #1				;R4 = R4 - 1
		BHI Delay_10us_Loop			;Branch if  R4 > 0
Delay_10us_Done
		POP {R4, R3, PC}			;Restore R4, R3 register and LR from stack into PC
;End
		BX LR						;Return
;------------------------------------------------------------- End of Delay_10us -----------------------------------------------------------

;--------------------------------------------------------------- Delay_1250ns -------------------------------------------------------------
; 1250ns delay using Delay function 
; Input: R0  delay parameter in units of the 12.5ms delay  
; Output: none     ; Modifies: R0, R1 
Delay_1250ns
		PUSH {R4, R3, LR}			;Save the register R4, R3 and the LR to stack
		MOVS R4, R0					;Move R0 into R4
		BEQ Delay_1250ns_Done		;If R0 is 0 then go to done
Delay_1250ns_Loop
		LDR R0, =Time_1250ns		;Counter that makes 1250ns delay (12.5ns * 100000 = 1250ns)
		BL Delay					;Branch to Delay for 1250ns
		SUBS R4, R4, #1				;R4 = R4 - 1
		BHI Delay_1250ns_Loop		;Branch if  R4 > 0
Delay_1250ns_Done
		POP {R4, R3, PC}			;Restore R4, R3 register and LR from stack into PC
;End
		BX LR						;Return
;------------------------------------------------------------ End of Delay_1250ns ---------------------------------------------------------


		ALIGN
		END