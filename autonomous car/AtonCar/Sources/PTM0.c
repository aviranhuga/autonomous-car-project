/*
 * PTM0.c
 *
 *  Created on: Jun 17, 2017
 *      Author: dorliv
 */


#include "TFC.h"
#include "mcg.h"

//static unsigned int MaxDutyCycle = 0x493a;

static double DistanceSample[2];
static int MotorStatus = 0;

extern double LeftSonicSample=150;
extern double RightSonicSample=150;
extern double LeftSpeed=0;
extern double RightSpeed=0;

extern short EncoderCheck;
extern int RightWantedSpeed;
extern int LeftWantedSpeed;

extern short StraightDrive;

extern int TPM0SampleChoose=0;


uint16_t LPTPM = 0;
uint16_t freq = 0;

extern int CH2status=0;
extern int CH3status=0;
static int CH2TOF=0;
static int CH3TOF=0;
uint16_t firstCH2=0;
uint16_t CH2freq=0;
uint16_t firstCH3=0;
uint16_t CH3freq=0;

extern int SampleStatus;// GLOBAL STATUS
extern int LeftEncoderCounter=600; // Encoder Counter for Clicks
extern int RightEncoderCounter=600; // Encoder Counter for Clicks


extern int Ultrastatus1=3; // 3 becouse we dont take samples
static int UltraTOF1=0;
uint16_t firstUltra1=0;
uint16_t Ultrafreq1=0;

extern int Ultrastatus2=3;
static int UltraTOF2=0;
uint16_t firstUltra2=0;
uint16_t Ultrafreq2=0;
static short InitMotorMovement=10; // Encoder Counter for Clicks
static short GoToLine=0;
static int TimeOfSample = 0;


extern int NumberOfFocusOnBlackLine = 300;

//___________________________________	
//  FTM0_IRQHandler(){
//___________________________________
void FTM0_IRQHandler(){
	
	
	// ULTRA SONIC SENSORS
	switch(TPM0SampleChoose){
	    case 0:
	    	TPM0_STATUS |= 0xFFFF;
	    	break;
		case 1:
		  if(TPM0_STATUS & 0x0100){// TOF interput
			 if(Ultrastatus1==1)
				UltraTOF1++;
  			if(Ultrastatus2==1)
				UltraTOF2++;
			TPM0_STATUS |= 0x0100;
	     }

		  if(TPM0_STATUS & 0x0004){//CHANNEL 2
			if(Ultrastatus1==0){
				firstUltra1 = TPM0_C2V;
				Ultrastatus1=1;
				TPM0_STATUS |= 0x0004;
			}
			else if (Ultrastatus1==1) {
					double num =((65535 * UltraTOF1) + TPM0_C2V - firstUltra1);
					DistanceSample[0] = ((num)/480)*11 ;
					RightSonicSample = DistanceSample[0];
					TPM0_C2SC &= ~TPM_CnSC_CHIE_MASK;
					Ultrastatus1=3; // WE FINISH SAMPLE
					UltraTOF1=0;
					TPM0_STATUS |= 0x0004;
					Ultrastatus2=0;
					TPM0_C3SC |= TPM_CnSC_CHIE_MASK;// enable TPM0 ch3 interrupt
				}
			  }//end of channel 2
					
		  if(TPM0_STATUS & 0x0008){//CHANNEL 3
			if(Ultrastatus2==0){
				firstUltra2 = TPM0_C3V;
				Ultrastatus2=1;
				TPM0_STATUS |= 0x0008;
			}
			else if (Ultrastatus2==1) {
				double num =((65535 * UltraTOF2) + TPM0_C3V - firstUltra2);
				DistanceSample[1] = ((num)/480)*11 ;
				LeftSonicSample = DistanceSample[1];
				TPM0_C3SC &= ~TPM_CnSC_CHIE_MASK;
				Ultrastatus2=3;
				UltraTOF2=0;
				TPM0_STATUS |= 0x0008;
			}
		  }//end of channel 3
				     
			if (Ultrastatus2==3 & Ultrastatus1==3){
				//GPIO Configuration - Encoder - Input
				PORTD_PCR2 = PORT_PCR_MUX(0); //Input Capture PTD2
				//GPIO Configuration - Encoder - Input
				PORTD_PCR3 = PORT_PCR_MUX(0); //Input Capture PTD3
				TPM0_SC &= ~TPM_SC_TOIE_MASK;
				TPM0SampleChoose=0;
				}
					

						break;
			case 2:// ENCODERS
				TimeOfSample++;
				if(TPM0_STATUS & 0x0100){
							if(CH2status==1)
								CH2TOF++;
							if(CH3status==1)
								CH3TOF++;
							TPM0_STATUS |= 0x0100;
						}
						
						if(TPM0_STATUS & 0x0004){//CHANNEL 2
							if(CH2status==0){
								firstCH2 = TPM0_C2V;
								CH2status=1;
								TPM0_STATUS |= 0x0004;
							}
							else if (CH2status==1) {
								RightSpeed = (((65535 * CH2TOF) + TPM0_C2V) - firstCH2);
								RightSpeed = 31950/RightSpeed;
								RightSpeed = RightSpeed - 1;
								TPM0_C2SC &= ~TPM_CnSC_CHIE_MASK;
								CH2status=3;
								CH2TOF=0;
								TPM0_STATUS |= 0x0004;
								CH3status=0;
								TPM0_C3SC |= TPM_CnSC_CHIE_MASK;
							}
						}//end of channel 2
						
						if(TPM0_STATUS & 0x0008){//CHANNEL 3
							if(CH3status==0){
								firstCH3 = TPM0_C3V;
								CH3status=1;
								TPM0_STATUS |= 0x0008;
							}
							else if (CH3status==1) {
								LeftSpeed = (((65535 * CH3TOF) + TPM0_C3V) - firstCH3);
								LeftSpeed = 31950/LeftSpeed;
								TPM0_C3SC &= ~TPM_CnSC_CHIE_MASK; 
								CH3status=3;//we finish sample
								CH3TOF=0;
								TPM0_STATUS |= 0x0008;
							}
						}//end of channel 3
					 
					if (CH3status==3 & CH2status==3){
					 //GPIO Configuration - Encoder - Input
						if (LeftSpeed > 5 && RightSpeed > 5){
							if (LeftSpeed-RightSpeed > 1){
								TPM2_C1V = TPM2_C1V + 50; // change speed
								TPM1_C0V = TPM1_C0V - 50; // change speed
							}
							if (RightSpeed-LeftSpeed > 1){
								TPM2_C1V = TPM2_C1V - 50; // change speed
								TPM1_C0V = TPM1_C0V + 50; // change speed
							}
						}
						
						
					 CH2status=0; //take sample
					 TPM0_C2SC |= TPM_CnSC_CHIE_MASK;// enable TPM0 ch2 interrupt
					 
					 //Get Current Location And Time
					 currTime = LastTime + speed * TimeOfSample;
					 CurrX =(LastTime-CurrTime )* speed;
					 TimeOfSample = 0;
					 
					 
					 }
					break;	
			case 3:// turn left 

				if(TPM0_STATUS & 0x0004){//CHANNEL 3		
					LeftEncoderCounter--;				
			    	if (LeftEncoderCounter==0){
			    		TPM0_C2SC &= ~TPM_CnSC_CHIE_MASK; 
						PORTC_PCR3 = PORT_PCR_MUX(0);
						PORTC_PCR4 = PORT_PCR_MUX(0);
			    		TPM0SampleChoose=0;
			    		LeftEncoderCounter=600;
			    		motor_DirSpeed(0,0);
			    	}//end of if
			    	
			    	TPM0_STATUS |= 0x0004;
				}//end of channel 3
				else if(TPM0_STATUS & 0x0008){// channel 2
					RightEncoderCounter--;
				  	if (RightEncoderCounter==0){
						TPM0_C3SC &= ~TPM_CnSC_CHIE_MASK; 
						PORTC_PCR3 = PORT_PCR_MUX(0);
						PORTC_PCR4 = PORT_PCR_MUX(0);
						TPM0SampleChoose=0;
						RightEncoderCounter=600;
						motor_DirSpeed(0,0);
					}//end of if
								    	
				  	TPM0_STATUS |= 0x0008;
				}// end of channel 2
				
			    	break;
			case 4: // Left On black line

				if(TPM0_STATUS & 0x0004){//CHANNEL 3
					LeftEncoderCounter--;
					if ((GPIOE_PDIR & PORT_LOC(2)) | (GPIOE_PDIR & PORT_LOC(3)) | 
					   (GPIOE_PDIR & PORT_LOC(4)) | (GPIOE_PDIR & PORT_LOC(5)) |
					   (GPIOA_PDIR & PORT_LOC(16))){
						if (GoToLine==0){
							GoToLine=1;
							LeftEncoderCounter=NumberOfFocusOnBlackLine;
						}
					}
					
			    	if (LeftEncoderCounter==0){
			    		TPM0_C2SC &= ~TPM_CnSC_CHIE_MASK; 
						PORTC_PCR3 = PORT_PCR_MUX(0);
						PORTC_PCR4 = PORT_PCR_MUX(0);
						GoToLine=0;
			    		TPM0SampleChoose=0;
			    		LeftEncoderCounter=600;
			    		InitMotorMovement = 10;
			    		motor_DirSpeed(0,0);
			    	}//end of if
			    	TPM0_STATUS |= 0x0004;
				}
			    	
				
				 if(TPM0_STATUS & 0x0008){//CHANNEL 3
						LeftEncoderCounter--;
						if ((GPIOE_PDIR & PORT_LOC(2)) | (GPIOE_PDIR & PORT_LOC(3)) | 
						   (GPIOE_PDIR & PORT_LOC(4)) | (GPIOE_PDIR & PORT_LOC(5)) |
						   (GPIOA_PDIR & PORT_LOC(16))){
							if (GoToLine==0){
								GoToLine=1;
								LeftEncoderCounter=NumberOfFocusOnBlackLine;
							}
						}
						
				    	if (LeftEncoderCounter==0){
				    		TPM0_C3SC &= ~TPM_CnSC_CHIE_MASK; 
							PORTC_PCR3 = PORT_PCR_MUX(0);
							PORTC_PCR4 = PORT_PCR_MUX(0);
							GoToLine=0;
				    		TPM0SampleChoose=0;
				    		LeftEncoderCounter=600;
				    		InitMotorMovement = 10;
				    		motor_DirSpeed(0,0);
				    	}//end of if
				    	TPM0_STATUS |= 0x0008;	
					}
			
					break;

	}//end of encoder for motors
			
					
}

void StopEncodersSensing(){
   
	    PORTC_PCR3 = PORT_PCR_MUX(0); 
	    PORTC_PCR4 = PORT_PCR_MUX(0); 
	    TPM0_SC &= ~TPM_SC_TOIE_MASK;
	    TPM0_C3SC &= ~TPM_CnSC_CHIE_MASK;
		TPM0_C2SC &= ~TPM_CnSC_CHIE_MASK;
	    TPM0SampleChoose=0;
	
	 
}
