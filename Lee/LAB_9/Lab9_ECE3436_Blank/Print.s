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
		

    AREA    |.text|, CODE, READONLY, ALIGN=2
			
    THUMB
		PRESERVE8 {TRUE}
  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
	PUSH {R4, LR}
	MOV R2, #10 ; R2 = 10 = divisor
	MOV R4, #0 ; R4 = 0 = cnt
	
	
ODloop
	UDIV R3, R0, R2 ; R3 = N/10
	MUL R1, R3, R2 ; R1 = N/10*10
	SUB R1, R0, R1 ; R1 = N%10
	PUSH {R1} ; save value
	ADD R4, R4, #1 ; cnt = cnt + 1
	MOVS R0, R3 ; R0 N = N/10
	CMP R0, #0 ; is N == 0?
	BNE ODloop ; if not continue
	
	
ODout
	POP {R0} ; restore into R0
	ADD R0, R0, #'0'; convert ASCII
	BL ST7735_OutChar ; print character
	SUBS R4, R4, #1 ; cnt = cnt – 1
	CMP R4, #0 ; is cnt == 0?
	BNE ODout ; if not continue
	POP {R4, LR} ; restore
	BX LR ; return
	
    BX  LR


      BX  LR
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
LCD_OutFix
	PUSH {R1-R6,R12,LR}
	SUB SP,#0x10
	
	MOV R5,R0
	MOV R1,#3
	MOV R3,SP
	MOV R2,#10
	MOV R12,#0
	
	
First	
	STR R1,[R3,#8]
	MOV R4,#1
	CMP R1,#2
	BNE Second
	PUSH {R0-R5}
	MOV R0,#0x2E
	BL ST7735_OutChar
	POP {R0-R5}
	
	
Second	
	ORRS R1,#0
	BEQ Fourth
	
	
Third	
	MUL R4,R2
	SUBS R1,#1
	BGT Third
	
	
Fourth	
	UDIV R0,R5,R4
	LDR R1,[R3,#8]
	CMP R0,#0x0A
	BPL Fifth
	ORRS R12,#0
	BNE Fifth
	B Sixth
	
	
Fifth	
	MOV R0,#0x2A
	MOV R12,#1
	B Seventh
	
	
Sixth	
	ADD R0,#0x30
	
	
Seventh	
	PUSH {R0-R5}
	BL ST7735_OutChar
	POP {R0-R5}
	SUB R0,#0x30
	MUL R0,R4
	SUB R5,R0
	SUBS R1,#1
	BPL First
	ADD SP,#0x10
	POP {R1-R6,R12,LR}
	BX LR 
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file