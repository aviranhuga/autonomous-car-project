/*
 * GeneralFunctions.c
 *
 *  Created on: Jun 11, 2017
 *      Author: Aviranh
 */

#include "TFC.h"
#include "mcg.h"


//-----------------------------------------------------------------
// printnumtochar
//-----------------------------------------------------------------


//___________________________________	
// freqcal
//___________________________________
double freqcal (uint32_t num){
	double sec = 24000000;
	double count = 0;
	while (10*num <= sec){
		sec -= 10*num;
		count += 10;
	}
	while (5*num <= sec){
			sec -= 5*num;
			count += 5;
		}
	while (num <= sec){
				sec -= num;
				count ++;
			}
	if(num < 2*sec)count++;
	return count;
	
}

//-----------------------------------------------------------------
//  PrintManu
//-----------------------------------------------------------------
void PrintMenu(){
	UARTprintf(UART0_BASE_PTR,"\r\n");
	UARTprintf(UART0_BASE_PTR,"            Vehicle checking menu\r\n");
	UARTprintf(UART0_BASE_PTR,"\r\n");
	UARTprintf(UART0_BASE_PTR,"1. move motors\r\n");
	UARTprintf(UART0_BASE_PTR,"2. \r\n");
	UARTprintf(UART0_BASE_PTR,"3. \r\n");
	UARTprintf(UART0_BASE_PTR,"4. \r\n");
	UARTprintf(UART0_BASE_PTR,"5. r\r\n");
	UARTprintf(UART0_BASE_PTR,"6. \r\n");
	UARTprintf(UART0_BASE_PTR,"7. \r\n");

}
