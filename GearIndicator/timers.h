/*
 * timers.h
 * Part of project: GearIndicator
 * Created: 5. 6. 2012 21:46:06
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

#ifndef TIMERS_H_
#define TIMERS_H_

//define names of timers
//10MS TIMERS
enum{
	COMMON,
	REPLY,
	RXFINISHED,
	DISPBLINK,
	PRGMODE,
	VALACCEPT,
	PGMEXIT,
	NUMBER_OF_TIMERS
	};

//set timers values
#define REPLYTMR		20						//Wait for the reply 200ms
#define RXFINISHEDTMR	2						//There is nothing coming in 20ms - message being received has finished
#define DISPBLINKTMR	10						//Display blinks with period 100ms (ON) + 100ms (OFF)
#define PRGMODETMR		200						//Enter to program mode timer (2s)
#define VALACCEPTTMR	300						//Value RPM is accepted once it is constant and this timer expires (3)
#define PGMEXITTMR		1000					//Program mode exits when this timer expires (10s)
		
//Timers
extern int volatile TIMER[NUMBER_OF_TIMERS];

//Function prototypes
void TMRinit(void);								//initializes T1 a enables interrupt

#endif /* TIMERS_H_ */