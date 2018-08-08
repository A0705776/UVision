; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

	PRESERVE8 
    AREA    |.text|, CODE, READONLY, ALIGN=2
	THUMB
		
Powers DCD 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
	

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
; R0 = the number
; R1 = temp 
; R2 = counter
; R3 = 2nd temp
; R4 = Powers (10^n)
; R5 = 10

LCD_OutDec
	PUSH {R1-R5,LR}					;saving the registers
	MOV R1,R0						;moving the number into R1 
	MOV R2,#-1						;initlize R2 for counting 
	MOV R5,#10						;initlize the divider
DivBy10
	ADD R2,#1						;increment R2 by 1
	UDIV R1,R5						;unsigned divide R1 = R1/R5
	CMP R1, #0						;check to see if riched the limit
	BNE DivBy10						;loop till finished counting 
	
	LDR R4, =Powers					;R4 = &Powers
	LSL R1,R2,#2					;R1 = R2*4
	LDR R4,[R1,R4]					;R4 = Max power	

	CMP R2,#9
	BEQ MaxNum
OutputChar
	UDIV R1,R0,R4					;R1 = number / power
	MUL R3,R4,R5					;R3 = power * 10
	UDIV R3,R0,R3					;R3 = number / (power*10)
	MUL R3,R5						;R3 = R3 * 10
	SUB R1,R1,R3					;R1 = R1 - R3
	
	PUSH {R0,R2}					;save R0 and R2
	ADD R0,R1,#0x30					;R0 = R1 + 0x30 (for number chars, from ASCII table)
	BL ST7735_OutChar				;output char to lcd
	POP {R0,R2}						;recover R0 and R2
	
	UDIV R4,R5						;R4 = R4 / 10
	CMP R4,#0						;compare if it is end of the sequence
	BNE OutputChar					;if not then loop and ourput chars

	POP {R1-R5,PC}					;recover and exit
	
MaxNum
	UDIV R1,R0,R4					;R1 = number / power
	PUSH {R0,R2}					;save R0 and R2
	ADD R0,R1,#0x30					;R0 = R1 + 0x30 (for number chars, from ASCII table)
	BL ST7735_OutChar				;output char to lcd
	POP {R0,R2}						;recover R0 and R2
	UDIV R4,R5						;R4 = R4 / 10
	B OutputChar					;go back
	
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
; R0 = the number
; R1 = temp 
; R2 = counter
; R3 = 2nd temp
; R4 = Powers (10^n)
; R5 = 10
LCD_OutFix
	PUSH {R1-R5,LR}					;saving the registers
	MOV R1,R0						;moving the number into R1 
	MOV R2,#-1						;initlize R2 for counting 
	MOV R5,#10						;initlize the divider
DivBy10_F
	ADD R2,#1						;increment R2 by 1
	UDIV R1,R5						;unsigned divide R1 = R1/R5
	CMP R1, #0						;check to see if riched the limit
	BNE DivBy10_F					;loop till finished counting 
	
	LDR R4, =Powers					;R4 = &Powers
	LSL R1,R2,#2					;R1 = R2*4
	LDR R4,[R1,R4]					;R4 = Max power	
	
	CMP R2,#3						;compare to see if the number > 4 digit
	BHI NumLargerThan4Digit			;if higher then fill in with '*'
	BLO FillInWithZeros				;if lower then add '0' till reach the limit

	UDIV R1,R0,R4					;R1 = number / power
	MUL R3,R4,R5					;R3 = power * 10
	UDIV R3,R0,R3					;R3 = number / (power*10)
	MUL R3,R5						;R3 = R3 * 10
	SUB R1,R1,R3					;R1 = R1 - R3
	
	PUSH {R0,R2}					;save R0 and R2
	ADD R0,R1,#0x30					;R0 = R1 + 0x30 (for number chars, from ASCII table)
	BL ST7735_OutChar				;output char to lcd
	MOV R0,#0x2E					;R0 = '.'
	BL ST7735_OutChar				;output char to lcd
	POP {R0,R2}						;recover R0 and R2
	
	UDIV R4,R5						;R4 = R4 / 10	
						
OutputChar_F
	UDIV R1,R0,R4					;R1 = number / power
	MUL R3,R4,R5					;R3 = power * 10
	UDIV R3,R0,R3					;R3 = number / (power*10)
	MUL R3,R5						;R3 = R3 * 10
	SUB R1,R1,R3					;R1 = R1 - R3
	
	PUSH {R0,R2}					;save R0 and R2
	ADD R0,R1,#0x30					;R0 = R1 + 0x30 (for number chars, from ASCII table)
	BL ST7735_OutChar				;output char to lcd
	POP {R0,R2}						;recover R0 and R2
	
	UDIV R4,R5						;R4 = R4 / 10
	CMP R4,#0						;compare if it is end of the sequence
	BNE OutputChar_F				;if not then loop and ourput chars

	B Exit							;exit
	
FillInWithZeros
	PUSH {R0,R2}					;save R0 and R2
	MOV R0,#0x30					;R0 = '0'
	BL ST7735_OutChar				;output char to lcd
	MOV R0,#0x2E					;R0 = '.'
	BL ST7735_OutChar				;output char to lcd
	POP {R0,R2}						;recover R0 and R2
	MOV R1,R2						;R1 = R2
LoopWriteZeros
	ADD R1,#1						;R1++
	CMP R1,#3						;compare to see if reach the limit
	BEQ OutputChar_F				;if true then go to output the number
	PUSH {R0-R2}					;save R0 and R2
	MOV R0,#0x30					;R0 = '0'
	BL ST7735_OutChar				;output char to lcd
	POP {R0-R2}						;recover R0 and R2
	B LoopWriteZeros				;loop till it reach the limit
	
NumLargerThan4Digit
	MOV R4,#3						;R4 = 3
	MOV R0,#0x2A					;R0 = '*'
	BL ST7735_OutChar				;output char to lcd
	MOV R0,#0x2E					;R0 = '.'
	BL ST7735_OutChar				;output char to lcd
LoopAsterisk
	SUB R4,#1						;R4--
	MOV R0,#0x2A					;R0 = '*'
	BL ST7735_OutChar				;output char to lcd
	CMP R4,#0						;compare with 0
	BNE LoopAsterisk				;if not true then loop
Exit
	POP {R1-R5,PC}					;recover and exit
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
