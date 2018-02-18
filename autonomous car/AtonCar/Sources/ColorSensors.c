/*
 * ColorSensors.c
 *
 *  Created on: Jun 21, 2017
 *      Author: aviran
 */


#include "TFC.h"
#include "mcg.h"

extern int RightWantedSpeed;
extern int LeftWantedSpeed;
extern uint8_t BlackLineResult = 0;
extern short lastfix=3;
static short nextfix = 0;
static short thierdfix = 0;
static int countHowMuchOutOfBlack = 0;

void ColorSensConfig (){
//___________________________________	
//  5 Sensors CONFIG
//___________________________________
	
	PORTE_PCR2 = PORT_PCR_MUX(1); // assign PTE2 as GPIO
	PORTE_PCR3 = PORT_PCR_MUX(1); // assign PTE3 as GPIO
	PORTE_PCR4 = PORT_PCR_MUX(1); // assign PTE4 as GPIO
	PORTE_PCR5 = PORT_PCR_MUX(1); // assign PTE5 as GPIO
	PORTA_PCR16 = PORT_PCR_MUX(1); // assign PTE5 as GPIO
	
	GPIOE_PDDR &= ~PORT_LOC(2); // set as input
	GPIOE_PDDR &= ~PORT_LOC(3); // set as input
	GPIOE_PDDR &= ~PORT_LOC(4); // set as input
	GPIOE_PDDR &= ~PORT_LOC(5); // set as input
	GPIOA_PDDR &= ~PORT_LOC(16); // set as input
	
}

void BlackLineKeeping (){
	
	int i;
    int BlackLineTable[5];
	BlackLineResult = 0;
	BlackLineTable[0] =  (GPIOE_PDIR & PORT_LOC(2))/4;
	BlackLineTable[1] = (GPIOE_PDIR & PORT_LOC(3))/4;
	BlackLineTable[2] = (GPIOE_PDIR & PORT_LOC(4))/ 4 ;
	BlackLineTable[3] = (GPIOE_PDIR & PORT_LOC(5))/4;
	BlackLineTable[4] = (GPIOA_PDIR & PORT_LOC(16))/4096;
	
	BlackLineResult |= (GPIOE_PDIR & PORT_LOC(2))/4;
	BlackLineResult |= (GPIOE_PDIR & PORT_LOC(3))/4;
	BlackLineResult |= (GPIOE_PDIR & PORT_LOC(4))/4 ;
	BlackLineResult |= (GPIOE_PDIR & PORT_LOC(5))/4;
	BlackLineResult |= (GPIOA_PDIR & PORT_LOC(16))/4096;
	
	
	UARTprintf(UART0_BASE_PTR,"\r\n");
	for(i=0; i<5 ; i++){
		if(BlackLineTable[i] == 0)UARTprintf(UART0_BASE_PTR,"0");
		else UARTprintf(UART0_BASE_PTR,"1");
	}
	if (BlackLineResult & 0x01 ){
		lastfix=0;
		if(nextfix == 1){
			  TPM2_C1V = 0x493a; // change speed
			  TPM1_C0V = TPM1_C0V - (0x493a/4); // change speed
			  nextfix = 0;
			  thierdfix = 1;
		}
		else{
			TPM2_C1V = TPM2_C1V - (0x493a/7); // change speed	
			TPM1_C0V = TPM1_C0V + (0x493a/7); // change speed
			countHowMuchOutOfBlack = 0;
		}
	}
	else if ((BlackLineResult & 0x03) | (BlackLineResult & 0x02) | (BlackLineResult & 0x06)){
			if(thierdfix == 1){
				TPM2_C1V = 0x493a; // change speed
				TPM1_C0V = TPM1_C0V - (0x493a/4); // change speed
				thierdfix = 0;
			}
			lastfix=0;
			countHowMuchOutOfBlack = 0;
			TPM2_C1V = TPM2_C1V - (0x493a/10); // change speed
			TPM1_C0V = TPM1_C0V + (0x493a/10); // change speed
	}
	else if ((BlackLineResult & 0x18) | (BlackLineResult & 0x08) | (BlackLineResult & 0x0c)){
			if(thierdfix == 1){
				TPM2_C1V = TPM2_C1V - (0x493a/4); // change speed
				TPM1_C0V = 0x493a; // change speed
				thierdfix = 0;
			}
			lastfix=1;
			countHowMuchOutOfBlack = 0;
			TPM2_C1V = TPM2_C1V + (0x493a/10); // change speed
			TPM1_C0V = TPM1_C0V - (0x493a/10); // change speed
		}
	else if (BlackLineResult & 0x10 ){
		    lastfix=1;
			if(nextfix == 1){
				TPM2_C1V = TPM2_C1V - (0x493a/4); // change speed	
				TPM1_C0V = 0x493a; // change speed
				  nextfix = 0;
				  thierdfix = 1;
			}else{
				countHowMuchOutOfBlack = 0;
				TPM2_C1V = TPM2_C1V + (0x493a/5); // change speed
				TPM1_C0V = TPM1_C0V - (0x493a/5); // change speed
			}
		}
	else {
		countHowMuchOutOfBlack++;
		if(countHowMuchOutOfBlack == 10){
		nextfix = 1;
		countHowMuchOutOfBlack = 0;
		}
		if (lastfix==1){//1 is left
				  TPM2_C1V = TPM2_C1V + (0x493a/5); // change speed
				  TPM1_C0V = TPM1_C0V - (0x493a/5); // change speed
		}//end of lastfix==1
		
		else if (lastfix==0){
			  TPM2_C1V = TPM2_C1V - (0x493a/5); // change speed	
			  TPM1_C0V = TPM1_C0V + (0x493a/5); // change speed
		}//end of lastfix==0
		
	}
	
}

