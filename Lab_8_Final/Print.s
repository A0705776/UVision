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
Power DCD 1, 10, 100, 1000, 10000
	  DCD 100000, 1000000, 10000000, 100000000
	  DCD 1000000000
  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
	 PUSH {R4-R11, LR}
	 MOV R1, R0 ;n = input
	 MOV R9, R0 ;n = input
	 MOV R6, #10
	 MOV R4, #0	;c = 0
	 ;determine how many digits the number have in R0, store it in R4
count	 
	 ADD R4, #1 ;c++
	 UDIV R1, R1, R6 ;n = n/10
	 CMP R1, #0
	 BNE count ;keep looping back until 0
	 ;digit count finishes
dec_out ;start print out the number
	 SUB R4, #1 ;c--
	 LSL R8, R4, #2 ;R8 = c*4
	 LDR R7, =Power ;R7 = &Power
	 LDR R7, [R7 ,R8] ;R7 = power[c]
	 UDIV R5, R9, R7 ; R5 = n/R7
	 ADD R0, R5, #0x30 ;convert R5 number to ASCII and store in R0 
	 BL ST7735_OutChar ;print out char R0
	 MUL R5, R7 ;R5 = R5*m
	 SUB R9, R5 ;R9 = n-R5 ;eliminating the digit
	 CMP R4, #0
	 BNE dec_out ;loop until c=0
	 POP {R4-R11, PC}
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
	 PUSH {R4-R11, LR}
	 MOV R1, R0 ;n = input
	 MOV R9, R0 ;n = input
	 MOV R6, #10
	 MOV R4, #0	;c = 0
	 ;determine how many digits the number have in R0, store it in R4
count1	 
	 ADD R4, #1 ;c++
	 UDIV R1, R1, R6 ;n = n/10
	 CMP R1, #0
	 BNE count1 ;keep looping back until 0
	 ;digit count finishes
	 CMP R4, #4
	 BHI higher4
	 MOV R4, #4
out_fix
	 SUB R4, #1 ;c--
	 LSL R8, R4, #2 ;R8 = c*4
	 LDR R7, =Power ;R7 = &Power
	 LDR R7, [R7 ,R8] ;R7 = power[c]
	 UDIV R5, R9, R7 ; R5 = n/R7
	 ADD R0, R5, #0x30 ;convert R5 number to ASCII and store in R0 
	 BL ST7735_OutChar ;print out char R0
	 MUL R5, R7 ;R5 = R5*m
	 SUB R9, R5 ;R9 = n-R5 ;eliminating the digit
	 CMP R4, #3
	 BEQ insert
	 CMP R4, #0
	 BNE out_fix ;loop until c=0
	 B done
insert
	 MOV R0, #46
	 BL ST7735_OutChar
	 B out_fix
higher4
	 MOV R0, #42
	 BL ST7735_OutChar
	 MOV R0, #46
	 BL ST7735_OutChar
	 MOV R0, #42
	 BL ST7735_OutChar
	 MOV R0, #42
	 BL ST7735_OutChar
	 MOV R0, #42
	 BL ST7735_OutChar
done
	 POP {R4-R11, PC}
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
