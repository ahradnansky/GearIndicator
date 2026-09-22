/*
 * KBus_tester.c
 *
 * Created: 25.05.2015 8:47:48
 *  Author: Andrej
 */ 

#include "global.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include "uart.h"
#include "KBus.h"
#include "bikes.h"
#include "buffer.h"
#include "uart.h"
#include "timers.h"
#include "softuart.h"

unsigned char Num2ASCII(unsigned char ch)
{
	if(ch<10)
		return('0'+ ch);
	else
		return('A'+ ch - 10);
}

int main(void)
{
unsigned char i,j;
unsigned char replymsg[BUFF_LNG];
unsigned char checksum;
unsigned char retries=1;
unsigned char ECU_address=0x11;			//A11 -> Addr 0x11/ Addr ERROR;
unsigned char ECU_register=0x0B;		//R0B -> Reg 0x0B / Reg ERROR; 
unsigned char replylen=7;				//length of reply

	cli();
	////////////////////////////////////////////////
	//Watch dog disable - in case it was enabled before
	wdt_disable();
	
	///////////////////////////////////////////////
	//INITIALIZATION FUNCTIONS UART
	//UART init
	UARTinit();
	
	//Soft UART
	softuart_init();
	softuart_turn_rx_on(); /* redundant - on by default */
	sei();
	
	//Timers init
	TMRinit();

	softuart_puts_P("\r\nKBUS test v1.0, (c)2015\r\n");
	softuart_puts_P("\r\nECU Addr: ");
	softuart_putchar(Num2ASCII(HNIBB(ECU_address)));
	softuart_putchar(Num2ASCII(LNIBB(ECU_address)));
	softuart_puts_P( "h\r\nREG Addr: " );
	softuart_putchar(Num2ASCII(HNIBB(ECU_register)));
	softuart_putchar(Num2ASCII(LNIBB(ECU_register)));
	softuart_puts_P("h\r\nOK.\r\n");


	KBUS_READ_REG[1]=ECU_address;
	KBUS_READ_REG[5]=ECU_register;
	KBUS_READ_REG[6]=KBUS_checksum(KBUS_READ_REG,6);
	
	//KBUS init - Enables all interrupts at the end of the routiner
	KBUS_fast_init(ECU_address);
	
    while(TRUE)
	{
		while(!TIMER[NEWREQUEST])
		{				
			UARTsendmsg(KBUS_READ_REG,7);			//Send read gear value	
			TIMER[REPLY]=REPLYTMR;					//start timer - waiting for reply
												
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
							softuart_puts_P( "\r\nKBus comm error. Trying " );
							softuart_puts_P("ECU Addr: ");
							softuart_putchar(Num2ASCII(HNIBB(ECU_address)));
							softuart_putchar(Num2ASCII(LNIBB(ECU_address)));
							softuart_puts_P( "; REG Addr: " );
							softuart_putchar(Num2ASCII(HNIBB(ECU_register)));
							softuart_putchar(Num2ASCII(LNIBB(ECU_register)));
							
							KBUS_fast_init(ECU_address);		//Try re-init
							retries=1;
						}
						
					TIMER[NEWREQUEST]=NEWREQUESTTMR;			//time between queries should be 55-5000ms (ISO14230)
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
			for(i=j=0;i<BUFF_LNG;i++)
				{
					if(replymsg[i]==0x80) break;
				}
			j=i;
			
			//Calculate checksum
			//according to message length
			if(ECU_address==0x11) replylen=8;
			if(ECU_address==0x12) replylen=57;
			
			checksuam=0;
			for(i=j;i<j+replylen-1;i++)
				checksum+=replymsg[i];
				
			//Chack CHS and display reply after register reading
			if(replymsg[j+replylen] == checksum)
			//checksum OK - display the result
				{
					//We need to display ASCII codes
					for(i=j;i<j+replylen-1;i++)
						{
							softuart_puts_P( "\r\n\r\nRX: " );
							softuart_putchar(Num2ASCII(HNIBB(replymsg[i])));
							softuart_putchar(Num2ASCII(LNIBB(replymsg[i])));
							softuart_putchar(' ');
						}
				}
			//check sum not OK
			else
				{
					softuart_puts_P( "KBus CRC ERROR.\r\n" );
				}
			retries=1;	
			TIMER[NEWREQUEST]=NEWREQUESTTMR;
		} //End While NEWREQUEST timer
	
		unsigned char c, cind=0;
		unsigned char command[3];

		if(softuart_kbhit()) 
			{
				lab2:
				softuart_puts_P( "\r\nCmd: " );

				lab1:
					c = softuart_getchar();
					softuart_putchar(c);
					switch(c)
					{
						case 'g':
						case 'G':
									if(cind==0)
										{
											command[cind++]='G';
											goto lab1;
										}
									else
										{
											softuart_puts_P( "\r\nERR: Unknonw cmd.\r\n" );
											cind=0;
											goto lab2;
										}
									break;
						case 'a':
						case 'A':
									command[cind++]='A';
									if(cind>3)
										{
											softuart_puts_P( "\r\nERR: Unknonw cmd.\r\n" );
											cind=0;
											goto lab2;
										}	
									else goto lab1;
									break;
						case 'r':
						case 'R':
									if(cind==0)
										{
											command[cind++]='R';
											goto lab1;
										}
									else
										{
											softuart_puts_P("\r\nERR: Unknonw cmd.\r\n" );
											cind=0;
											goto lab2;
										}
									break;
						case 0x0D:
									switch(command[0])
										{
											case 'A':
														ECU_address=command[1]*16+command[2];
														KBUS_READ_REG[1]=ECU_address;
														KBUS_READ_REG[6]=KBUS_checksum(KBUS_READ_REG,6);
														softuart_puts_P( "\r\nECU Addr: " );
														softuart_putchar(Num2ASCII(HNIBB(ECU_address)));
														softuart_putchar(Num2ASCII(LNIBB(ECU_address)));
														softuart_puts_P( "\r\nOK.\r\n" );
														cind=0;
														goto lab2;
														break;
											case 'R':
														ECU_register=command[1]*16+command[2];
														KBUS_READ_REG[5]=ECU_register;
														KBUS_READ_REG[6]=KBUS_checksum(KBUS_READ_REG,6);
														softuart_puts_P( "\r\nREG Addr: " );
														softuart_putchar(Num2ASCII(HNIBB(ECU_register)));
														softuart_putchar(Num2ASCII(LNIBB(ECU_register)));
														softuart_puts_P( "\r\nOK.\r\n" );
														cind=0;
														goto lab2;
														break;
											case 'G':
													    break;
											default:	
														softuart_puts_P( "\r\nERR: Unknonw cmd.\r\n" );
														cind=0;
														goto lab2;
										}
									break;
						default:
									if(((c>='0' && c<='9') || (c>='A'&& c<='F') || (c>='a' && c<='f')) && cind>0)	
										{
											if(c>='a') c-=32;
											if(c>='A')
													   c-=55;
											else
													   c-=48;	
											command[cind++]=c;
											if(cind<=3)
												{ 
													goto lab1;
												}
											else
												{
													softuart_puts_P( "\r\nERR: Unknonw cmd.\r\n" );
													cind=0;
													goto lab2;
												}
										}				
									else			
										{
											softuart_puts_P( "\r\nERR: Unknonw cmd.\r\n" );	
											cind=0;
											goto lab2;
										}
									break;
					}
			}
	}
}

