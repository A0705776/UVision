;ECE3436 Fall 2017
;Lab 4
;Name: put your name here or this look silly
;Copy the code from Lab 2-3 here
;Whenever you press the button to change the duty cycle, call the DEBUG_CAPTURE subroutine
;to save data and time into the RAM
;Also impliment the heartbeat LED indicator, so that whenever you change the duty cycle
;the heartbeat LED will flash

SYSCTL_RCGCGPIO_R  EQU 0x400FE608
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

; CONSTANT

	   
       AREA    |.text|, CODE, READONLY, ALIGN=2
       THUMB
       EXPORT  Start
	  

Start
	


    ALIGN      ; make sure the end of this section is aligned
    END        ; end of file

