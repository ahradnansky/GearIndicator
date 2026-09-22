/*
 * timers.c
 *
 * Created: 5. 6. 2012 21:23:25
 * Modified: 7/2014
 * Author: a.hradnansky
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "global.h"
#include "timers.h"

int volatile TIMER[NUMBER_OF_TIMERS];

//T1 init
void TMRinit(void)
{
uint8_t i;

	for(i=0;i<NUMBER_OF_TIMERS;i++) TIMER[i]=0;

	/*
	Timers are controlled by TIMER1
	16-bit Timer/Counter with PWM
	*/

	TCCR1B|=1<<WGM12;										//Timer 1 - CTC mode
	TCCR1B|=1 << CS12|1 << CS10;							//Pre-scaler fCPU/1024
	OCR1A = (F_CPU/102400UL) - 1;					        //Set CTC compare to period of 10ms-> gives f=100Hz:(FCPU/(Prescaler * Required_f))-1
	STB(TIMSK1, OCIE1A);									//Enable interrupt from CTC
}

//ISR - interrupt routine from T1 (TIMING SERVICES FOR EVERYTHING)
ISR(TIMER1_COMPA_vect)
{
int i;
	/////////////////////////////////////
	//Reset Watch Dog - every 10ms
	//wdt_reset();
	
	/////////////////////////////////////
	//TIMERS decrement
	for(i=0;i<NUMBER_OF_TIMERS;i++)
			if(TIMER[i]>0) TIMER[i]--;
	//TIMERS decrement:END
	/////////////////////////////////////
} //END OF: ISR(TIMER1_COMPA_vect)