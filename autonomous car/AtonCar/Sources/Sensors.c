/*
 * Sensors.c
 *
 *  Created on: May 27, 2017
 *      Author: Aviran and Tom 
 */

#include "TFC.h"
#include "mcg.h"
double IRsensor[150];
static int IRstatus = 0;
static int SampleNumber = 0;
double temp[15];
int kkk=0;
extern int status;
static int i=0 ;
static int sample=0;

void DistSensConfig(){
//___________________________________	
// IR distance measuring sensors 
//___________________________________
		// Enable clocks
		SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;	// ADC0 clock
		
		// Configure ADC
		ADC0_CFG1 = 0; // Reset register
		ADC0_CFG1 |= (ADC_CFG1_MODE(3)  |  	// 16 bits mode
					  ADC_CFG1_ADICLK(0)|	// Input Bus Clock (20-25 MHz out of reset (FEI mode))
					  ADC_CFG1_ADIV(1)) ;	// Clock divide by 2 (10-12.5 MHz)
		
		ADC0_CV1 = 0x0000;// 0 Volt 
		ADC0_CV2 = 0xFFFF;// 3.3 Volt 
		
		ADC0_SC3 =  ADC_SC3_AVGS(4);// 4 averaged sampales prior to "interrupting" the MCU
		
		ADC0_SC2 = 0;
		ADC0_SC2 |= ADC_SC2_ACREN_MASK + ADC_SC2_ACFE_MASK + ADC_SC2_ACFGT_MASK + ADC_SC2_REFSEL(1); // compare function (ADC_SC2_ACREN_MASK)     
		ADC0_SC1A = 0;
		ADC0_SC1A |= ADC_SC1_ADCH(31) + ADC_SC1_AIEN_MASK; // channel 4  module
		
		PORTD_PCR6 = PORT_PCR_MUX(0); // Assign PTD6 pin to ADC0_SE7b
		PORTD_PCR5 = PORT_PCR_MUX(0); // Assign PTD5 pin to ADC0_SE6b
		
		ADC0_CFG2  |= ADC_CFG2_MUXSEL_MASK;
		
		enable_irq(INT_ADC0-16); // Enable Interrupts 
		set_irq_priority (INT_ADC0-16,0);  // Interrupt priority = 1
}



double IRTakeSample(double smaple){
	
	 ADC0_SC1A |= ADC_SC1_ADCH(6) + ADC_SC1_AIEN_MASK; // channel 7  module
	 for( i = 1 ; i < 150 ; i++){
	 		if(sample > IRsensor[i]+20 && sample < IRsensor[i]-20) return sample;
	 	}
	 return 0;
}
//-----------------------------------------------------------------
// ADC0 - IRsensorInsert
//-----------------------------------------------------------------	
void IRsensorInsert(double sample){
	if (IRstatus==0){			
	   ADC0_SC1A = 0;
	   ADC0_SC1A |= ADC_SC1_ADCH(6) + ADC_SC1_AIEN_MASK; // channel 7  module
	   IRstatus=1;
	   return;
	 }
	int i;
	IRsensor[SampleNumber]=sample;
	if(SampleNumber == 0) SampleNumber = 9;
	else SampleNumber+=10; // next sample
		
		if (SampleNumber==159){// we finish sampling
			IRstatus=2;  
		    SampleNumber=9; //reset sample number
		    
		    FillMatrix(0,9,9); //first 5 values
		    for (i=0 ; i<14 ; i++){
		    	FillMatrix(9 + i*10,i*10+19,10); // rest of the values
		    }
      }//end of finish sample
}

//-----------------------------------------------------------------
// Fill Marix
//-----------------------------------------------------------------
void FillMatrix(int minIndex, int maxIndex, int div){
	int i;
	float gap = (IRsensor[maxIndex]-IRsensor[minIndex])/div;
	for( i = 1 ; i < div ; i++){
		IRsensor[minIndex+i] = IRsensor[minIndex+i-1] + gap;
	}
}

//-----------------------------------------------------------------
// DistanceMeasuring1
//-----------------------------------------------------------------
double DistanceMeasuring1(double DisSamp1){
	int i=2;
	for (; i<150 ; i++){
		if ((IRsensor[i-1]<DisSamp1 && DisSamp1<IRsensor[i+1]) || IRsensor[i]==DisSamp1){
			UARTprintf(UART0_BASE_PTR,"Distance is:\r\n");
			ADC0_SC1A = 0;
			printnumtochar((double)i);
		    return;}
	}
}
//-----------------------------------------------------------------
// Convert To Volt
//-----------------------------------------------------------------
double ConvertToVolt(uint32_t sample){
			sample = sample * 330; 
			sample = sample >> 16; // 0-330
			double ADCsample = sample;
			double volt = 0;
				while ( ADCsample >= 100 ){
					ADCsample=ADCsample-100;
					volt = volt + 1;
				}//0.x for some x
				while ( ADCsample>=10 ){
					ADCsample=ADCsample-10;
					volt = volt + 0.1;
					}
				while ( ADCsample>=1 ){
					ADCsample=ADCsample-1;
					volt = volt + 0.01;
					}				
				return volt;
	}

//-----------------------------------------------------------------
// printnumtochar
//-----------------------------------------------------------------
void printnumtochar(double num){
 char ahadot='0';
 char asirit='0';
 char miiiot='0';
	while ( num >= 1 ){
		num=num-1;
		ahadot=ahadot+1;
	}//0.x for some x
	while ( num>=0.1 ){
			num=num-0.1;
			asirit=asirit+1;
		}
	while ( num>=0.01 ){
				num=num-0.01;
				miiiot=miiiot+1;
			}
	UARTprintf(UART0_BASE_PTR,"\r\n");
	while(!(UART0_S1 & UART_S1_TDRE_MASK)); // TX buffer is empty and ready for sending
		UART0_D = ahadot;
    while(!(UART0_S1 & UART_S1_TDRE_MASK)); // TX buffer is empty and ready for sending
		UART0_D = '.';
	while(!(UART0_S1 & UART_S1_TDRE_MASK)); // TX buffer is empty and ready for sending
	    UART0_D = asirit;
	while(!(UART0_S1 & UART_S1_TDRE_MASK)); // TX buffer is empty and ready for sending
		    UART0_D = miiiot;	    
	UARTprintf(UART0_BASE_PTR,"[volt]\r\n");
	}



//-----------------------------------------------------------------
// ADC0 - ISR = Interrupt Service Routine
//-----------------------------------------------------------------
void ADC0_IRQHandler(){
	
	uint32_t sample=ADC0_RA;
	double volt = ConvertToVolt(sample);
	
//-----------------------------------------------------------------
// Status is 2 
//-----------------------------------------------------------------
	if(status == 2){
	//Motor servo 	
	UARTprintf(UART0_BASE_PTR,"\r\n");
	UARTprintf(UART0_BASE_PTR,"DANGER! volt over 1.28v\r\n");
	GREEN_LED_TOGGLE;
    ADC0_SC1A |= ADC_SC1_ADCH(31); // disable adc
    
    }
//-----------------------------------------------------------------
// Status is 3 
//-----------------------------------------------------------------
	else if(status == 3 || status == 4){
	
	if(IRstatus == 1)IRsensorInsert(volt); 
	else if(IRstatus == 2)DistanceMeasuring1(volt); 
	RED_LED_TOGGLE;
    temp[kkk] = volt;
    kkk++;
	}
//-----------------------------------------------------------------
// Status is 6 
//-----------------------------------------------------------------
	else if(status == 6){
		printnumtochar(ConvertToVolt(sample));
		BattLedState (ConvertToVolt(sample));
	}
}
	

