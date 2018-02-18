#include "TFC.h"
#include "mcg.h"


//-----------------------------------------------------------------
// InitLeds
//-----------------------------------------------------------------
void InitLeds()
{
	//GPIO Configuration - LEDs - Output
	PORTD_PCR1 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;  //Blue
	GPIOD_PDDR |= BLUE_LED_LOC; //Setup as output pin	
	PORTB_PCR18 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK; //Red  
	PORTB_PCR19 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK; //Green
	GPIOB_PDDR |= RED_LED_LOC + GREEN_LED_LOC; //Setup as output pins
	RGB_LED_OFF;
}

//-----------------------------------------------------------------
// TPMx - Clock Setup
//-----------------------------------------------------------------
void ClockSetup(){

  //enable Clocks to all ports - page 206, enable clock to Ports
   SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
   
   pll_init(8000000, LOW_POWER, CRYSTAL,4,24,MCGOUT); //Core Clock is now at 48MHz using the 8MHZ Crystal
		
    //Clock Setup for the TPM requires a couple steps.
    //1st,  set the clock mux
    //See Page 124 of f the KL25 Sub-Family Reference Manual
    SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;// We Want MCGPLLCLK/2=24MHz (See Page 196 of the KL25 Sub-Family Reference Manual
    SIM_SOPT2 &= ~(SIM_SOPT2_TPMSRC_MASK);
    SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1); //We want the MCGPLLCLK/2 (See Page 196 of the KL25 Sub-Family Reference Manual
	//Enable the Clock to the TPM0 and PIT Modules
	//See Page 207 of f the KL25 Sub-Family Reference Manual
	 	    
	SIM_SCGC6 |= SIM_SCGC6_TPM1_MASK; // tpm1 clock(motor 2)
	SIM_SCGC6 |= SIM_SCGC6_TPM2_MASK;// tpm1 clock(motor 1)
	SIM_SCGC6 |= SIM_SCGC6_TPM0_MASK; // TPM0 timer
	SIM_SCGC6 |= SIM_SCGC6_PIT_MASK; //Enable the Clock to the PIT Modules
}




void initTPM0forUltraSonic(){
	PORTE_PCR31 = PORT_PCR_MUX(0); // DISABLE PIN FOR SERVO MOTOR
	TPM0_SC = 0; // to ensure that the counter is not running
	TPM0_SC |= TPM_SC_PS(5); //Prescaler =6, up-mode, counter-disable
	TPM0_MOD = 0xAFBE; // PWM T=60ms
	TPM0_C2SC = 0;
	TPM0_C2SC |= TPM_CnSC_ELSA_MASK +  TPM_CnSC_ELSB_MASK; // Rising edge + Falling edge
	TPM0_C3SC = 0;
	TPM0_C3SC |= TPM_CnSC_ELSA_MASK +  TPM_CnSC_ELSB_MASK; // Rising edge + Falling edge

}

void initTPM0forEncoders(){
	PORTE_PCR31 = PORT_PCR_MUX(0); // DISABLE PIN FOR SERVO MOTOR
	TPM1_SC = 0; // to ensure that the counter is not running
	TPM1_SC |= TPM_SC_PS(5); //Prescaler =5, up-mode, counter-disable
	TPM1_MOD = 0x493E; // PWM frequency of 40Hz
	TPM0_C2SC = 0;
	TPM0_C2SC |= TPM_CnSC_ELSA_MASK; // Rising edge 
	TPM0_C3SC = 0;
	TPM0_C3SC |= TPM_CnSC_ELSA_MASK; // Rising edge
}

void initTPM0forServo(){
	TPM0_SC = 0; // to ensure that the counter is not running
	TPM0_SC |= TPM_SC_PS(5); //Prescaler =5, up-mode, counter-disable
	TPM0_MOD = 0x493E; // PWM frequency of 40Hz
	PORTE_PCR31 = PORT_PCR_MUX(3); // Assign PTE31 pin TPM0_CH4 - ALT3
}

void initTPM1(){
	//initTPM1_CH0
	TPM1_SC = 0; // to ensure that the counter is not running
	TPM1_SC |= TPM_SC_PS(5); //Prescaler =5, up-mode, counter-disable
	TPM1_MOD = 0x493E; // PWM frequency of 40Hz
	TPM1_C0SC |= TPM_CnSC_MSB_MASK + TPM_CnSC_ELSB_MASK ;
	TPM1_C0V = 0; // stop motor DS=0
	TPM1_CONF = 0;
	
}


//-----------------------------------------------------------------
// PIT - Initialisation
//-----------------------------------------------------------------
void InitPIT(){
	// Timer 0
	PIT_LDVAL0 = 0x001CFC80; // setup timer 0 for 100msec counting period
	PIT_TCTRL0 = PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK; //enable PIT0 and its interrupt
	PIT_LDVAL1 = 0x0121DD00; // setup timer 0 for 100msec counting period
	PIT_TCTRL1 = PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK; //enable PIT0 and its interrupt
	PIT_MCR |= PIT_MCR_FRZ_MASK; // stop the pit when in debug mode
	enable_irq(INT_PIT-16); //  //Enable PIT IRQ on the NVIC
	set_irq_priority(INT_PIT-16,2);  // Interrupt priority = 0 = max
	PIT_MCR &= ~PIT_MCR_MDIS_MASK; //Enable the PIT module
}

 
