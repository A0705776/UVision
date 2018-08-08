;DEBUG.s
;DEBUG_INIT: INITIALIZE SYSTICK, CREATE 2 ARRAY PORT_E_MEA, PORT_E_TIME
;0XFF OR 0XFFFFFFFF INTO THE ARRAY TO SIGNIFY NO DATA SAVED.
;DEBUG_CAPTURE: SAVE A DATA-POINT(PE1 INPUT, PE0 OUTPUT) IN DATABUFFER, AND NVIC_ST_CURRENT_R TO TIMEBUFFER

;VARIABLES
NVIC_ST_RELOAD_R   EQU 0xE000E014
NVIC_ST_CURRENT_R  EQU 0xE000E018
GPIO_PORTE_DATA_R   EQU 0x400243FC
;ARRAYS
	   AREA		DATA, ALIGN=2
PORTE_DATA SPACE 1
CURRENT_TIME SPACE 4
DATABUFFER SPACE 50
TIMEBUFFER SPACE 4*50
DATAPT	SPACE 4
TIMEPT	SPACE 4
NENTRIES SPACE 1
;VARIABLES


	   IMPORT SYSTICK_INIT  ;change the name if you use different name
	   IMPORT SYSTICK_CURRENT_TIME ;change the name if you use different name
       AREA    |.text|, CODE, READONLY, ALIGN=2
       THUMB
	   EXPORT DEBUG_INIT ;change the name if you use different name
	   EXPORT DEBUG_CAPTURE ;change the name if you use different name

DEBUG_INIT
		;Write Your code here
	BX LR							;Go Back	

DEBUG_CAPTURE
		;Write Your code here
	BX LR							;Go Back

	ALIGN
	END