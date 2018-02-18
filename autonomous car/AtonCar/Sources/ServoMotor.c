/*
 * ServoMotor.c
 *
 *  Created on: May 27, 2017
 *      Author: tomhor
 */


#include "TFC.h"
#include "mcg.h"

extern int LeftServocurr = 0;
extern int RightServocurr = 0;
//For sample 0 = move right servo motor to specific degree
extern int RightServoWantedDegree = 0;
extern int LeftServoWantedDegree = 0;
static unsigned int PWMfreq = 0x493E; // 40Hz
static int Left90degree = 1155; // 180 degree
static int Left0degree = 495; // 0 degree
static int Right90degree = 775; // 180 degree
static int Right0degree = 1455; // 0 degree
static double RightMove5degree = (1455 - 775)/18; // 5 degree
static double LeftMove5degree = (1155 - 495)/18; // 5 degree
extern short sample1EN;




void ServosConfig(){
//___________________________________	
//SERVO MOTOR 1 CONFIG
//___________________________________
	//init TPM1_CH1 Servo1_En
	TPM1_C1SC |= TPM_CnSC_MSB_MASK + TPM_CnSC_ELSB_MASK ;
	TPM1_C1V = 0; // stop motor DS=0

	//PTE21 - TPM1_CH1
	PORTE_PCR21 = PORT_PCR_MUX(3); // Assign PTE21 pin TPM1_CH1 - ALT3
//___________________________________	
//SERVO MOTOR 2 CONFIG
//___________________________________
//init TPM0_CH4 Servo2_En

	TPM0_C4SC |= TPM_CnSC_MSB_MASK + TPM_CnSC_ELSB_MASK ;
	TPM0_C4V = 0; // stop motor DS=0
	TPM0_CONF = 0;
	TPM0_SC |= TPM_SC_CMOD(1); //Start the TPM1 counter
	
	//PTE21 - TPM1_CH1
	PORTE_PCR31 = PORT_PCR_MUX(0); //  DISABLE PIN FOR SERVO MOTOR
}
void ADCservoConfig(){
	
//___________________________________	
//ADC Servos MOTORS CONFIG
//___________________________________
	
	// Enable clocks
	SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;	// ADC0 clock
	
	// Configure ADC
	ADC0_CFG1 = 0; // Reset register
	ADC0_CFG1 |= (ADC_CFG1_MODE(3)  |  	// 16 bits mode
				  ADC_CFG1_ADICLK(0)|	// Input Bus Clock (20-25 MHz out of reset (FEI mode))
				  ADC_CFG1_ADIV(1)) ;	// Clock divide by 2 (10-12.5 MHz)
	
	ADC0_CV1 = 0x6b6e;// 1.3 Volt 
	ADC0_CV2 = 0xFFFF;// 3.3 Volt 
	
	ADC0_SC2 = 0;
	ADC0_SC2 |= ADC_SC2_ACREN_MASK + ADC_SC2_ACFE_MASK + ADC_SC2_ACFGT_MASK + ADC_SC2_REFSEL(1); // compare function (ADC_SC2_ACREN_MASK)     
	ADC0_SC1A = 0;
	 ADC0_SC1A |= ADC_SC1_ADCH(31) + ADC_SC1_AIEN_MASK; // channel 4  module
	
	PORTE_PCR29 = PORT_PCR_MUX(0); // Assign PTE29 pin to ADC0_SE4b
	PORTE_PCR30 = PORT_PCR_MUX(0); // Assign PTE30 pin to ADC0_SE23
	
	ADC0_CFG2  |= ADC_CFG2_MUXSEL_MASK;
	
	enable_irq(INT_ADC0-16); // Enable Interrupts 
	set_irq_priority (INT_ADC0-16,2);  // Interrupt priority = 1

}

//___________________________________	
//Left Servo start zero degree
//___________________________________

void LeftStartzeroDeg(){
	TPM1_C1V = Left0degree;
	LeftServocurr = 0;
}

//___________________________________	
//right Servo start zero degree
//___________________________________
void RightStartzeroDeg(){
	initTPM0forServo();
	TPM0_C4V = Right0degree;
	RightServocurr = 0;

}
//___________________________________	
//Left Servo start 90 degree
//___________________________________

void Left90Deg(){
	TPM1_C1V = Left90degree;
	LeftServocurr = 90;

}

//___________________________________	
//right Servo start 90 degree
//___________________________________
void Right90Deg(){
	initTPM0forServo();
	TPM0_C4V = Right90degree;
	RightServocurr = 90;

}

//___________________________________	
//LEFT servo 5 degree move
//___________________________________
// direction 0 = go to 90 degree
// direction 1 = go to 0 degree
void LeftMove5Degree(int direction){
	if(direction == 0 & LeftServocurr < 90){
		TPM1_C1V = TPM1_C1V + LeftMove5degree;
		LeftServocurr = LeftServocurr + 5;
		printnumtochar(LeftServocurr);
	} 
	if(direction == 1 & LeftServocurr > 0){
			TPM1_C1V = TPM1_C1V - LeftMove5degree;
			LeftServocurr = LeftServocurr - 5;
			printnumtochar(LeftServocurr);
	}
}	

//___________________________________	
//Right servo 5 degree move 
//___________________________________
// direction 0 = go to 90 degree
// direction 1 = go to 0 degree
void RightMove5Degree(int direction){
	initTPM0forServo(); 
	if(direction == 0 & RightServocurr < 90){ 
		TPM0_C4V = TPM0_C4V - RightMove5degree;
		RightServocurr = RightServocurr + 5;
		printnumtochar(RightServocurr);
	} 
	if(direction == 1 & RightServocurr > 0){
			TPM0_C4V = TPM0_C4V + RightMove5degree;
			RightServocurr = RightServocurr - 5;
			printnumtochar(RightServocurr);
	} 

}	

//___________________________________	
//Right move to degree
//___________________________________

void RightDegree(int degree){
	RightServoWantedDegree = degree;
	sample1EN = 1;
}
//___________________________________	
//Left move to degree
//___________________________________

void LeftDegree(int degree){
	LeftServoWantedDegree = degree;
	sample1EN = 1;
}
//___________________________________	
//ServoScan
//___________________________________

void ServoScan(){ // always start when servo motors is in 0 degree
	RightServoWantedDegree = 65;
	LeftServoWantedDegree = 65;
	sample2EN = 1;
}


//___________________________________	
//RIGHT scan
//___________________________________

//void RightScan(int startDeg,){




