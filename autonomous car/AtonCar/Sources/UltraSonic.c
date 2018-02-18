/*
 * UltraSonic.c
 *
 *  Created on: Jun 8, 2017
 *      Author: tomhor
 */

#include "TFC.h"
#include "mcg.h"
static double SampleUltra=0;
extern int Ultrastatus1;
extern int Ultrastatus2;
extern int TPM0SampleChoose;

void UltraSonicConfig()
{
	//-----------------------------------------------------------------
	//  TPM0_CH1 TRIGGER FOR UltraSonic
	//-----------------------------------------------------------------
	TPM0_CONF = 0;
	TPM0_C1SC |= TPM_CnSC_MSB_MASK + TPM_CnSC_ELSB_MASK ;
	TPM0_C1V = 0x5; // stop motor DS=10us
	TPM0_SC |= TPM_SC_CMOD(1); //Start the TPM1 counter
	
	//PTD1 - TPM1_CH1
	PORTD_PCR1 = PORT_PCR_MUX(4); // / Assign PTE21 pin TPM0_CH1 - ALT4
	
	//-----------------------------------------------------------------
	//  PTD2 and PTD2 Input  ECHO
	//-----------------------------------------------------------------
	
	TPM0_C2SC = 0;
	TPM0_C2SC |= TPM_CnSC_ELSA_MASK +  TPM_CnSC_ELSB_MASK; // Rising edge + Falling edge
	TPM0_C3SC = 0;
	TPM0_C3SC |= TPM_CnSC_ELSA_MASK +  TPM_CnSC_ELSB_MASK; // Rising edge + Falling edge

	
	//GPIO Configuration - Encoder - Input
	PORTD_PCR2 = PORT_PCR_MUX(0); //Input Capture PTD2
	//GPIO Configuration - Encoder - Input
	PORTD_PCR3 = PORT_PCR_MUX(0); //Input Capture PTD3

	TPM0_STATUS |= 0x0006;	//clear interrupt flag
	enable_irq(INT_TPM0-16); // Enable Interrupts 
	set_irq_priority (INT_TPM0-16,1);  // Interrupt priority = 1

	}

void takesample(){

	//GPIO Configuration - Encoder - Input
	PORTD_PCR2 = PORT_PCR_MUX(4); //Input Capture PTD2
	//GPIO Configuration - Encoder - Input
	PORTD_PCR3 = PORT_PCR_MUX(4); //Input Capture PTD3
	TPM0SampleChoose=1;
	initTPM0forUltraSonic();
	Ultrastatus1=0;
	Ultrastatus2=3;
	TPM0_SC |= TPM_SC_CMOD(1) + TPM_SC_TOIE_MASK ; // Start counter and TOIE
	TPM0_STATUS |= 0xFFFF;
	TPM0_C2SC |= TPM_CnSC_CHIE_MASK;// enable TPM0 ch2 interrupt
	//TPM0_C3SC |= TPM_CnSC_CHIE_MASK;// enable TPM0 ch3 interrupt
}


