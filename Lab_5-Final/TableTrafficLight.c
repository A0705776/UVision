// TableTrafficLight.c solution to edX lab 10, EE319KLab 5
// Runs on LM4F120 or TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// November 7, 2013

/*

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "SysTick.h"
#include "Lab5_init.h"
#define S_G (*((volatile unsigned long *)0x40005004)) //PB0 =1
#define S_Y (*((volatile unsigned long *)0x40005008)) //PB1 =2
#define S_R (*((volatile unsigned long *)0x40005010)) //4
#define W_G (*((volatile unsigned long *)0x40005020)) //8
#define W_Y (*((volatile unsigned long *)0x40005040)) //16
#define W_R (*((volatile unsigned long *)0x40005080)) //PB5 =32
#define W_B (*((volatile unsigned long *)0x40024004)) //PE0 =1
#define S_B (*((volatile unsigned long *)0x40024008)) //PE1 =2
#define P_B (*((volatile unsigned long *)0x40024010)) //PE2 =4
#define P_R (*((volatile unsigned long *)0x40025008)) //PF1 =2
#define P_G (*((volatile unsigned long *)0x40025020)) //PF3 =8
int Button_before;
int Button_after;
// Declare your FSM linked structure here
void SouthToWest(void);
void WestToSouth(void);
void PedestrianOn(void);
void PedestrianOff(void);
void EnableInterrupts(void);

int main(void)
{
		volatile unsigned long delay;
		HW_init();
		SYSTICK_INIT();     
		//EnableInterrupts();
		S_G = 1;
		S_Y = 0;
		S_R = 0;
		W_G = 0;
		W_Y = 0;
		W_R = 32;
		P_R = 2;
		P_G = 0;
		
		//FSM Engine
		while(1)
		{
			Button_before = GPIO_PORTE_DATA_R & 0x07;
			SYSTICK_10MS(100); //delay 1s
			Button_after = GPIO_PORTE_DATA_R & 0x07;
			if(Button_after == Button_before) //buttons are pressed for at least 1s
			{
			switch(Button_after) //checking 3 switches
				{
				//-----------------------case 0--------------------------
				case 0: //no button is pressed
					//do nothing
				break;
				//-------------------------------------------------------
				
				//------------------------case 1-------------------------
				case 1: //West button is pressed
					switch(GPIO_PORTB_DATA_R&0x09) //checking which lane is running
					{
						case 1: //S is green
							switch(GPIO_PORTF_DATA_R&0x08) //check pedestrian light
							{
								case 0: //pedestrian light is red
								SouthToWest();
								break;
								case 8: //pedestrian light is green
								PedestrianOff();
								SouthToWest();
								break;
							}
						break;
						
						case 8: //W is green
							//do nothing
						break;
					}				
				break;
				//-------------------------------------------------------
				
				//------------------------case 2-------------------------
				case 2: //South button is pressed
					switch(GPIO_PORTB_DATA_R&0x09) //checking which lane is running
					{
						case 1: //S is green
							//do nothing
						break;
						
						case 8: //W is green
							switch(GPIO_PORTF_DATA_R&0x08) //check pedestrian light
							{
								case 0: //pedestrian light is red
								WestToSouth();
								break;
								case 8: //pedestrian light is green
								PedestrianOff();
								WestToSouth();
								break;
							}
						break;
					}	
				break;
				//-------------------------------------------------------
				
				//------------------------case 3-------------------------
				case 3: //South and West buttons are pressed
					switch(GPIO_PORTB_DATA_R&0x09) //checking which lane is running
					{
						case 1: //S is green
							switch(GPIO_PORTF_DATA_R&0x08) //check pedestrian light
							{
								case 0: //pedestrian light is red
								SouthToWest();
								SYSTICK_10MS(300);
								WestToSouth();
								break;
								case 8: //pedestrian light is green
								PedestrianOff();
								SouthToWest();
								SYSTICK_10MS(300);
								WestToSouth();
								break;
							}
						break;
						
						case 8: //W is green
							switch(GPIO_PORTF_DATA_R&0x08) //check pedestrian light
							{
								case 0: //pedestrian light is red
								WestToSouth();
								SYSTICK_10MS(300);
								SouthToWest();
								break;
								case 8: //pedestrian light is green
								PedestrianOff();
								WestToSouth();
								SYSTICK_10MS(300);
								SouthToWest();
								break;
							}
						break;
					}	
				break;
				//-------------------------------------------------------
				
				//------------------------case 4-------------------------
				case 4: //Pedestrian button is pressed
					switch(GPIO_PORTF_DATA_R&0x08) //check pedestrian light
							{
								case 0: //pedestrian light is red
								PedestrianOn();
								break;
								case 8: //pedestrian light is green
								PedestrianOff();
								SYSTICK_10MS(100);
								PedestrianOn();
								break;
							}
				break;
				//-------------------------------------------------------
				
				//------------------------case 5-------------------------
				case 5: //Pedestrian and West buttons are pressed
					switch(GPIO_PORTB_DATA_R&0x09) //checking which lane is running
					{
						case 1: //S is green
							switch(GPIO_PORTF_DATA_R&0x08) //check pedestrian light
							{
								case 0: //pedestrian light is red							
								SouthToWest();
								PedestrianOn();
								break;
								case 8: //pedestrian light is green
								PedestrianOff();
								SouthToWest();
								PedestrianOn();
								break;
							}
						break;
						
						case 8: //W is green
							switch(GPIO_PORTF_DATA_R&0x08) //check pedestrian light
							{
								case 0: //pedestrian light is red
								PedestrianOn();
								break;
								case 8: //pedestrian light is green
								PedestrianOff();
								SYSTICK_10MS(100);
								PedestrianOn();
								break;
							}
						break;
					}				
				break;
				//-------------------------------------------------------
				
				//------------------------case 6-------------------------
				case 6: //Pedestrian and South buttons are pressed
					switch(GPIO_PORTB_DATA_R&0x09) //checking which lane is running
					{
						case 1: //S is green
							switch(GPIO_PORTF_DATA_R&0x08) //check pedestrian light
							{
								case 0: //pedestrian light is red
								PedestrianOn();
								break;
								case 8: //pedestrian light is green
								PedestrianOff();
								SYSTICK_10MS(100);
								PedestrianOn();
								break;
							}
						break;
						
						case 8: //W is green
							switch(GPIO_PORTF_DATA_R&0x08) //check pedestrian light
							{
								case 0: //pedestrian light is red
								WestToSouth();
								PedestrianOn();
								break;
								case 8: //pedestrian light is green
								PedestrianOff();
								WestToSouth();
								PedestrianOn();
								break;
							}
						break;
					}	
				break;
				//-------------------------------------------------------
				
				//------------------------case 7-------------------------
				case 7: //All three buttons are pressed
					switch(GPIO_PORTB_DATA_R&0x09) //checking which lane is running
					{
						case 1: //S is green
							switch(GPIO_PORTF_DATA_R&0x08) //check pedestrian light
							{
								case 0: //pedestrian light is red
								SouthToWest();
								PedestrianOn();
								SYSTICK_10MS(300);
								PedestrianOff();
								WestToSouth();
								PedestrianOn();
								break;
								case 8: //pedestrian light is green
								PedestrianOff();
								SouthToWest();
								PedestrianOn();
								SYSTICK_10MS(300);
								PedestrianOff();
								WestToSouth();
								PedestrianOn();
								break;
							}
						break;
						
						case 8: //W is green
							switch(GPIO_PORTF_DATA_R&0x08) //check pedestrian light
							{
								case 0: //pedestrian light is red
								WestToSouth();
								PedestrianOn();
								SYSTICK_10MS(300);
								PedestrianOff();
								SouthToWest();
								PedestrianOn();
								break;
								case 8: //pedestrian light is green
								PedestrianOff();
								WestToSouth();
								PedestrianOn();
								SYSTICK_10MS(300);
								PedestrianOff();
								SouthToWest();
								PedestrianOn();
								break;
							}
						break;
					}	
				break;
				//-------------------------------------------------------
				}
			}
		}
}
//----------------------------South to West light----------------
void SouthToWest(void) //South turn red, West turn green
{
	S_G = 0;
	S_Y = 2; //yellow on
	SYSTICK_10MS(100); //delay 1s
	S_Y = 0;
	S_R = 4;
	SYSTICK_10MS(50);
	W_R = 0;
	W_G = 8;
	//SYSTICK_10MS(300); //light stay for at least 3s
}
//----------------------------West to South Light-------------------
void WestToSouth(void) //West turns red, South turn green
{
	W_G = 0;
	W_Y = 16; //yellow on
	SYSTICK_10MS(100); //delay 1s
	W_Y = 0;
	W_R = 32;
	SYSTICK_10MS(50);
	S_R = 0;
	S_G = 1;
	//SYSTICK_10MS(300); //light stay for at least 3s
}
//--------------------------Turn on Pedestrian light----------------
void PedestrianOn(void)
{
	P_R = 0;
	P_G = 8;
	
}
//--------------------------Turn off Pedestrian Light--------------
void PedestrianOff(void)
{
	P_R = 2;
	P_G = 0;
	SYSTICK_10MS(20);
	P_R = 0;
	SYSTICK_10MS(20);
	P_R = 2;
	SYSTICK_10MS(20);
	P_R = 0;
	SYSTICK_10MS(20);
	P_R = 2;
	SYSTICK_10MS(20);
	P_R = 0;
	SYSTICK_10MS(20);
	P_R = 2;
}
//-----------------------------------------------------------------
