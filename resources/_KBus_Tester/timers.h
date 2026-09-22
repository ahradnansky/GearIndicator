/*
 * timers.h
 *
 * Created: 5. 6. 2012 21:46:06
 * Modified: 7/2014
 * Author: a.hradnansky
 */ 

#ifndef TIMERS_H_
#define TIMERS_H_

//define names of timers
//10MS TIMERS
enum{
	COMMON,
	REPLY,
	RXFINISHED,
	NEWREQUEST,
	NUMBER_OF_TIMERS
	};

//set timers values
#define REPLYTMR		50						//Wait for the reply 500ms
#define RXFINISHEDTMR	2						//There is nothing coming in 20ms - message being received has finished
#define NEWREQUESTTMR	100						//TIMER TO SEND new request 1s - time between queries should be 55-5000ms (ISO14230)
		
//Timers
extern int volatile TIMER[NUMBER_OF_TIMERS];

//Function prototypes
void TMRinit(void);								//initializes T1 a enables interrupt

#endif /* TIMERS_H_ */