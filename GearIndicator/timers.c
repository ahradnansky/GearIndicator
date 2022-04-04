/*
 * timers.c
 * Part of project: GearIndicator
 * Created: 5. 6. 2012 21:23:25
 * Modified: 7/2014
 * Author: Andrej Hradnansky, 2022
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU
 * (General Public License as published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "global.h"
#include "led.h"
#include "timers.h"

int volatile TIMER[NUMBER_OF_TIMERS];
unsigned char volatile f_dispcleared=0;

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
	wdt_reset();
	
	/////////////////////////////////////
	//TIMERS decrement
	for(i=0;i<NUMBER_OF_TIMERS;i++)
			if(TIMER[i]>0) TIMER[i]--;
	//TIMERS decrement:END
	/////////////////////////////////////
	
	//Display refresh + blink
	if(f_dispblink)
		{
			if(!TIMER[DISPBLINK])
				{
					TIMER[DISPBLINK]=DISPBLINKTMR;
					if(f_dispcleared)
						LEDoutch(displayedchar);
					else
						LEDclear();
						
					f_dispcleared=~f_dispcleared;
				}
		}
	else
		LEDoutch(displayedchar);
	
} //END OF: ISR(TIMER1_COMPA_vect)