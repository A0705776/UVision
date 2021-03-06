// FiFo.c
// Runs on LM4F120/TM4C123
// Provide functions that implement the Software FiFo Buffer
// Last Modified: 4/10/2017 
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
// --UUU-- Declare state variables for FiFo
//        size, buffer, put and get indexes
#define size 6
char FIFO_BUFFER[size];
uint8_t Put;
uint8_t Get;
// *********** FiFo_Init**********
// Initializes a software FIFO of a
// fixed size and sets up indexes for
// put and get operations
void FiFo_Init() {
// --UUU-- Complete this
	Put=Get=0;
}

// *********** FiFo_Put**********
// Adds an element to the FIFO
// Input: Character to be inserted
// Output: 1 for success and 0 for failure
//         failure is when the buffer is full
uint32_t FiFo_Put(char data) {
	
	// --UUU-- Complete this routine
	if((Put+1)%size==Get){
		return 0;}
		FIFO_BUFFER[Put]=data;
		Put=(Put+1)%size;
   return(1); 
			
}

// *********** FiFo_Get**********
// Gets an element from the FIFO
// Input: Pointer to a character that will get the character read from the buffer
// Output: 1 for success and 0 for failure
//         failure is when the buffer is empty
uint32_t FiFo_Get(char *datapt)
{
	if(Get==Put){
		return 0;}
	  *datapt=FIFO_BUFFER[Get];
		Get=(Get+1)%size;
	//--UUU-- Complete this routine
  return(1); // Replace this
}



