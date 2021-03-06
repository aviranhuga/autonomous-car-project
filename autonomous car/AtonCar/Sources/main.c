/*
 * main implementation: use this 'C' sample to create your own application
 *
 */
#include "TFC.h"
#include "derivative.h" /* include peripheral declarations */

extern int SampleStatus= 0;

extern short sample4EN;

int main(void){
	
	ClockSetup();
	initTPM1();
	InitLeds();
	InitUARTs();
	motorconfig();
	PrintMenu();
	ServosConfig();
	UltraSonicConfig();
	DistSensConfig();
	initTPM1();
	InitPIT();
	ColorSensConfig();
	LeftStartzeroDeg();
	RightStartzeroDeg();
	EmptyMap();

	
    
	return 0;
}


//-----------------------------------------------------------------
//  UART0 - ISR
//-----------------------------------------------------------------
void UART0_IRQHandler(){
	
	if(UART0_S1 & UART_S1_RDRF_MASK){ // RX buffer is full and ready for reading
		
	uint8_t Temp = UART0_D;
	
		switch(Temp){
		case '1':
			motor_DirSpeed(3,3);

					break;
		case '2':
			motor_DirSpeed(0,0);
					break;
		case '3':
			EncodersSensing();
					break;
		case '4':
			motor_DirSpeed(4,4);
					break;
		case '5':
			StopEncodersSensing();
			
					break;
		case '6':
			ServoScan();

					break;
		case '7':
			LeftStartzeroDeg();
			RightStartzeroDeg();
			
					break;
		case '8':
			TPM1_C0V = TPM1_C0V - 50; // change speed

							break;
		case '9':
			SearchToTheLeft(550,550,500,500);

							break;
		case '0':
			LeftFocusBlackLine();
			 // TPM2_C1V = TPM2_C1V - (0x493a/100); // change speed
			 // TPM1_C0V = TPM1_C0V + (0x493a/100); // change speed

							break;
		}//end of switch
  }//end of status 0
	
}

