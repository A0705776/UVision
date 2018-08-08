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

i EQU 0 ; loop iterator, also number of digits in number
digits EQU 4 ; loop iterator, also number of allowable digits in fixed-point
modder EQU 10 ; constant
zero EQU 0x30 ; ASCII 0
star EQU 0x2A ; ASCII *
dot EQU 0x2E ; ASCII .

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Modifies R0-R6
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
	PUSH {R0,R4-R11,LR}
	
	;Calculates number of digits in input
	PUSH {R0-R2,LR}
	BL numDigits
	MOV R4,R0 ; stores return of numDigits to temp reg to not get clobbered by POP
	POP {R0-R2,LR}
	MOV R1,R4 ; restores numDigits to R1

	;CURRENT REGS: 
	;R0 = input
	;R1 = numDigits
	;R2 = divider value, 10^(numDigits-1)
	
	;outputs number
outDigit
	MOV R2,#1
	MOV R3,R1 ; copy # digits to become iterator for exp loop
	MOV R4,#modder
	CMP R1,#1
	BEQ out
	
	SUB R3,#1
exp  ; decrements # digits
	SUB R3,#1
	MUL R2,R4 ; multiplies divider by 10 until correct divider reached (10^(n-1))
	CMP R3,#0
	BNE exp
	
out	UDIV R5,R0,R2 ; Masks for first digit
	ADD R5,#zero ; Converts to ASCII
	MOV R6,R0 ; Moves input to R6
	MOV R0,R5 ; Moves ASCII char to R0
	PUSH {R0-R11}
	BL ST7735_OutChar
	POP {R0-R11}
	SUB R5,#zero ; convert back to decimal
	MUL R5,R2
	SUB R0,R6,R5; Masks away most significant digit of input
	SUB R1,#1 ; updates numDigits for mask
	CMP R1,#0
	BNE outDigit
	
	
	POP {R0,R4-R11,LR}
	BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; Modifies: R0-R3
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
	MOV R1,#9999 ; if cannot be represented in format, print "*.*** "
	CMP R0,R1
	BHI outstar
	
	MOV R1,R0 ; To protect from clobbering input
	MOV R2,#digits
	MOV R3,#modder
	MOV R5,#1
	SUB R2,#1
	
exp2
	MUL R5,R3 ; divider
	SUBS R2,#1
	BNE exp2
 ;R5 = 1000
printNum
	UDIV R0,R5; Print first number
	PUSH{R0-R6,LR}
	BL LCD_OutDec
	POP {R0-R6,LR}
	MUL R0,R5 ; decimal mask
	SUB R1,R0 
	MOV R0,R1
	UDIV R5,R3 ; update divider
	
	PUSH {R0-R6,LR}
	MOV R0,#dot ; Print dot
	BL ST7735_OutChar
	POP {R0-R6,LR}

nextNum
	UDIV R0,R5 ; mask first digit
	PUSH{R0-R6,LR}
	BL LCD_OutDec ; print digit
	POP {R0-R6,LR}
	MUL R0,R5 ; remove first digit by subtracting it from prev
	SUB R1,R0
	MOV R0,R1
	UDIV R5,R3 ; update divider
	CMP R5, #0
	BHI nextNum
	BLS ret

outstar ; print "invalid" string
	PUSH {R0,LR}
	BL printString
	POP {R0,LR}
	
ret	BX LR
 
	ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

;-----------------------numDigits-----------------------
; Output number of digits of unsigned 32-bit number
; Input: R0 (call by value) 32-bit unsigned number
; Output: R0 number of digits
; Modifies: R0,R1,R2
; Invariables: This function must not permanently modify registers R4 to R11
numDigits
	MOV R1,#0 ;allocation
	PUSH {R1,LR}
	MOV R2, #modder
count
	LDR R1,[SP,#i] ;access - increment digit counter
	ADD R1,#1
	STR R1,[SP,#i]
	UDIV R0,R2 ; divide by 10 (masks away one digit by truncation)
	CMP R0,#0 ; check if there are more digits (i.e. integer part is not 0)
	BNE count ; if input is 0, return 0 as # digits
	
	ADD SP,#8 ;deallocation
	MOV R0,R1
	BX  LR
;* * * * * * * * End of numDigits * * * * * * * *

;-----------------------printString-----------------------
; Print string "*.***"
; Input: none
; Output: none
; Modifies: R0
; Invariables: This function must not permanently modify registers R4 to R11
printString
	PUSH {R0,LR}
	
	MOV R0, #star
	BL  ST7735_OutChar
	MOV R0, #dot
	BL  ST7735_OutChar
	MOV R0, #star
	BL  ST7735_OutChar
	MOV R0, #star
	BL  ST7735_OutChar
	MOV R0, #star
	BL  ST7735_OutChar
	
	POP {R0,LR}
	BX  LR
;* * * * * * * * End of printString * * * * * * * *



     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
