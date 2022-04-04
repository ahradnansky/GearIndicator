/*
 * main.c
 * Part of project: GearIndicator
 *
 * Gear indicator for Kawasaki (ECU address 11h) - tested with Z750, Versys, Z800,
 * and for Suzuki (ECU 12h) - tested with V-Strom, VL800,...
 * using KBUS communication to ECU via KDS (ISO-14230) protocol
 * Other bikes can be added, see bikes.c / bikes.h
 * Extra function: RPM over limit indication (display blinks fast if RMP are above set point value)
 *
 * Software developed in Atmel Studio (now Microchip Studio)
 * Controller:		ATmega328P
 * Hardware design: CadSoft Eagle (now Autodesk Eagle) - 2x 2 layers PCB, SMD design
 * 3D Modelling:		SketchUp
 *
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


#include "global.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "uart.h"
#include "led.h"
#include "bikes.h"
#include "kbus.h"
#include "buffer.h"
#include "uart.h"
#include "timers.h"
#include "eeproma.h"

//RPM hysteresis for programming mode
//if actual RPM differs +/- this value, it is considered the same RPM
#define	 RPMHYSTERESYS	200							

int main(void)
{

//unsigned char *replymsgptr;
volatile unsigned char* replymsgptr;
volatile unsigned char actgear=10, newgear=10;
volatile unsigned int  actrpm=0, newrpm=2000;		//RPM value is 2 bytes long, so type is unsigned int
volatile unsigned char retries=1;
volatile unsigned char actualECU;					// actual ECU we currently communicate with
volatile unsigned char initcheck=YES;				// if COMM_REQ and DIAG_REQ reply from ECU is checked for its correctness
unsigned char replymsg[BUFF_LNG];
unsigned char f_prgmode=FALSE;
unsigned char f_exittmr_started=FALSE;

int i=0;											// temp variable i

	////////////////////////////////////////////////
	//Watch dog disable - in case it was enabled before
	wdt_disable();
	
	////////////////////////////////////////////////
	//LED display init
	LEDinit();					//display init

	////////////////////////////////////////////////
	//WELCOME MESSAGES
	//Uses direct LEDoutch() because timers don't run yet
	for(i=27;i<=35;i++)			//LEtS ridE
		{
			LEDoutch(i);
			_delay_ms(200);
			LEDclear();
			_delay_ms(200);
		}
		
	_delay_ms(400);
	
	//show display numbers - display test
	//and show '-' at the end
	for(i=0;i<=6;i++)
		{
			LEDoutch(i);
			_delay_ms(100);
		}
	
	for(i=6;i>0;i--)
		{
			LEDoutch(i);
			_delay_ms(100);
		}
	LEDoutch(0);
	_delay_ms(100);
	LEDoutch(10);			//display "-"
	
	//END OF WELCOME MESSAGES
	///////////////////////////////////////////////

	///////////////////////////////////////////////
	//Timers init
	TMRinit();
	
	//Read last used ECU from EEPROM
	EEPROMread();							//lastECU from EEPROM
	
	if(lastStatus.lastECU>NUM_OF_ECUS-1) 
		{
			lastStatus.lastECU=0;			//if lastECU is wrong number
			EEPROMwrite();					//write 0 to EEPROM
		}
	actualECU=lastStatus.lastECU;			//and set actualECU
	
	if(lastStatus.initcheck>1)
		{
			lastStatus.initcheck=YES;
			EEPROMwrite();
		}
	
	initcheck=lastStatus.initcheck;
	
	//RPM PROGRAMM MODE
	CLB(UARTDR,RX);			//RX is input
	STB(UARTPORT,RX);		//Pull up
	
	//if RX pin is LOW
	//Entering programming mode
	TIMER[PRGMODE]=PRGMODETMR;			//set the timer
	while(!(UARTPIN & (1<<RX)))			//if RX is LOW
		{
			sei();						//enable INT		
			if(!TIMER[PRGMODE])			//and timer expired	
				{
					displayedchar=36;	//display P - programming mode
					rpmValue.prgmode=0;	//store prgmode to EEPROM - prgmode active = 0
					rpmValue.rpm=0;		//store rpm
					EEPROM_rpm_write();	//to EEPROM		
					f_dispblink=TRUE;
					while(TRUE);		//wait until indicator is turned off/reset
				}
		}
	
	cli();								//disable INT
	
	///////////////////////////////////////////////
	//UART init
	UARTinit();
	
	///////////////////////////////////////////////
	//PROGRAMMING MODE
	EEPROM_rpm_read();					//get data from EEPROM
	
	//if EEPROM is empty 0xFFFF
	if(rpmValue.rpm==0xFFFF)			
		{
			rpmValue.rpm=0;				//Make it 0
			rpmValue.prgmode=0xFF;		//Programm mode off
			EEPROM_rpm_write();			//Store rpm to EEPROM
		}
		
	//if we are entering  prog mode
	if(rpmValue.prgmode==0)				
		{
			f_prgmode=TRUE;				//program mode active
			displayedchar=36;			//display P
		}
	
	///////////////////////////////////////////////
	//START ENDLESS LOOP
    while(TRUE)
		{	
			////////////////////////////////////////////
			//PROGRAMMING MODE EXIT - IN CASE NO RPM IS SET
			//start programing mode exit timer in the beginning
			if(f_prgmode && !f_exittmr_started)
				{
					f_exittmr_started=TRUE;
					TIMER[PGMEXIT]=PGMEXITTMR;
				}
			
			//if 	exit timer expired SET DEFAULT STATUS (without RPM indication set)
			if(!TIMER[PGMEXIT] && f_exittmr_started)
				{
					f_exittmr_started=FALSE;	
					rpmValue.rpm=0;
					rpmValue.prgmode=0xFF;
					EEPROM_rpm_write();
					f_prgmode=FALSE;
				}
			//PROGRAMMING MODE EXIT (END)
			///////////////////////////////////////////
			
			KBUS_send_msg(actualECU);			//Send K-Bus message
			
			if(	(initcheck==YES)||\
				(initcheck==NO && KBUS_state==KBUS_S_COMM_STARTED)||\
				(initcheck==NO && KBUS_state==KBUS_S_DATA_POOLING) )
				{
							
					TIMER[REPLY]=REPLYTMR;				//start timer - waiting for reply
												
					while(!(UCSR0B & (1<<RXEN0)));				//Wait until Receiver is enabled
			
					while(TIMER[REPLY] && !TIMER[RXFINISHED]);	//Wait until character is received or TIMER expires
			
					//If timer expired, try again
					if(!TIMER[REPLY]) 
						{
							if(retries<NOOFRETRIES)
								{
									retries++;
								}
							else
								{	
									//ECU does not reply 
									f_dispblink=FALSE;
									
									if(f_prgmode)
										displayedchar=36;	//in PROGRAMMING MODE display "P"
									else 
										displayedchar=10;	//else display "-"
										
									IO_CLR(DP_BIT);		    //Turn DP on the display as flag that ECU does not reply
							
									//lets see if we have another ECU connected
									//if we check reply for init commands, we will try not to
									if(initcheck==YES)
										{
											initcheck=NO;
										}
									else
									//if we don't check reply, try another ECU and check replies from init commands at first
									//try another known ECU
										{
											actualECU=(actualECU+1) % NUM_OF_ECUS;
											initcheck=YES;
										}
										
									//K-Bus state is idle - no reply until now
									KBUS_state = KBUS_S_IDLE;
									retries=1;
								}
						
							_delay_ms(200);					//time between queries should be 55-5000ms (ISO14230)
							continue;
						}

					while(TIMER[RXFINISHED]);				//wait while whole reply message has arrived
			
					//Clear the buffer
					for(i=0;i<BUFF_LNG;i++) replymsg[i]=BUFFFILLCHAR;
				
					i=0;
				
					while(BUFFpop(&rxbuf,&replymsg[i]))		//read all data from the RX buffer to replymsg
						{
							i=(i+1) % BUFF_LNG;
						}
				
					/////////////////////////////////////////////////
					//Check the message	
					//Find the beginning of the message in the buffer (if necessary) 
					for(i=0;i<BUFF_LNG-2;i++)
						{
							//reply starts 0x00 or 0x40	
							if((replymsg[i]&0xF0)==NO_ADDRESS||(replymsg[i]&0xF0)==CARB) break;
							
							//reply starts 0x80 or 0xC0
							if(((replymsg[i]&0xF0)==PHYSICAL_ADDR ||(replymsg[i]&0xF0)==FUNCTIONAL_ADDR) &&\
								replymsg[i+1]==IND_ADDR &&\
								replymsg[i+2]==BIKES_DATA[actualECU].ecu_addr)
									break;
						}
						
					//Start of the message was found	
					if(i!=BUFF_LNG-2)
						{
							replymsgptr=&replymsg[i];
						}
					//Start of the message was not found
					else
						{
							//?? Something wrong;
							//LEDoutch(16);			//display "E"rror
							displayedchar=16;
							continue;
						}
							
				
					//if reply is OK
					if(KBUS_check_reply(actualECU,replymsgptr))
						{
							//Store ECUno which is active to EEPROM, if necessary
							if(actualECU!=lastStatus.lastECU)
								{
									lastStatus.lastECU=actualECU;
									EEPROMwrite();
								}
								
							if(initcheck!=lastStatus.initcheck)
						
								{
									lastStatus.initcheck=initcheck;
									EEPROMwrite();
								}
							//If we are already getting data - display gear on the LED display and/or get RPM
							if(KBUS_state==KBUS_S_DATA_POOLING)
								{
									//GEAR
									if(*(replymsgptr+5)==BIKES_DATA[actualECU].local_id_gear)
									{
										newgear = *(replymsgptr+BIKES_DATA[actualECU].byte_no_gear);	//byte of the message where is the gear value stored
										if(newgear > 9) newgear = 10;									//Number >9 is displayed as "-"
										
										if(f_prgmode)
											displayedchar=36;											//always display P in program mode
										else
											{							
												if(newgear!=actgear)
												{
													//LEDoutch(newgear);		
													displayedchar=newgear;								//display gear
												
													actgear=newgear;
												}
											}
									}
									
									//RPM
									if(*(replymsgptr+5)==BIKES_DATA[actualECU].local_id_rpm)
									{
										switch(actualECU)
										{
											//KAWASAKI (Z750: rpm byte * 100;...)
											case 0:													
												actrpm=(unsigned int)*(replymsgptr+BIKES_DATA[actualECU].byte_no_rpm)*100;
												break;
										
											//SUZUKI (DL650: rpm byte * 62.5;...)
											case 1:
												actrpm=(unsigned int)*(replymsgptr+BIKES_DATA[actualECU].byte_no_rpm)*62.5;
												break;
											
											default:
												break;
										}
												
										
										//////////////////////////
										//PROGRAMM MODE - STORE RPM TO EEPROM
										
										
										if(f_prgmode) 
											{
												//if actrpm > 3000
												if(actrpm>3000)
													{	
														//restart PROGRAMING MODE EXIT timer
														TIMER[PGMEXIT]=PGMEXITTMR;
														
														//if  actrpm is out of hysteresis interval
														if(!(actrpm>newrpm-RPMHYSTERESYS && actrpm<newrpm+RPMHYSTERESYS))
															{
																//we have new RPM value
																newrpm=actrpm;
																//start the timer
																TIMER[VALACCEPT]=VALACCEPTTMR;
																f_dispblink=FALSE;
																
															}
														//actrpm is in hysteresis interval	
														else
															{	
																//timer has expired
																if(!TIMER[VALACCEPT])
																	{	
																		//store new RPM value to EEPROM
																		rpmValue.rpm=newrpm;
																		rpmValue.prgmode=0xFF;
																		EEPROM_rpm_write();
																		f_dispblink=TRUE;
																		//f_prgmode=FALSE;
																	}
															}
													}
													
												//actrpm<=3000
												else
													{
														//nothing to do here ??
													}
											}
										else
											//f_prgmode=FALSE
											{	
												if(actrpm>=rpmValue.rpm && rpmValue.rpm>0)
													f_dispblink=TRUE;
												else
													f_dispblink=FALSE;
											}
											
										//END OF RPM PROGRAMMING AND INDICATION
										///////////////////////////
									}
								}
						}
					//If reply is not ok
					else
						{
							//?? Reply is not OK
							//LEDoutch(16);		//display "E"rror
							displayedchar=16;
							continue;
						}
			
					retries=1;
					_delay_ms(100);	//time between queries should be 55-5000ms (ISO14230)
				}
			else
				{
					_delay_ms(300);
					if(KBUS_state==KBUS_S_IDLE)
						{
							KBUS_state=KBUS_S_COMM_STARTED;
							continue;
						}
				} 
		}
}