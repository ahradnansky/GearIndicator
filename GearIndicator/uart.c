/*
 * uart.c
 * Part of project: GearIndicator
 * Created: 13. 3. 2014 16:28:49
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
#include <avr/interrupt.h>
#include <avr/io.h>
#include "uart.h"
#include "buffer.h"
#include "timers.h"
#include "led.h"

//UART Initialization (328P)
//10400,n,8,1
void UARTinit(void)
{
	//Set the baud rate
	UBRR0L = LO(UART_BAUD_REGISTER);
	UBRR0H = HI(UART_BAUD_REGISTER);
	
	//Asynchronous, no-parity, 1-stop bit, 8 data bits
	UCSR0C |= (0 << UMSEL01) | (0 << UMSEL00) | (0 << UPM01) | (0<<UPM00) | (0<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00);
	
	//Enable RX interrupt
	STB(UCSR0B, RXC0);				
	
	//RX and TX buffers init
	BUFFinit(&rxbuf);
	BUFFinit(&txbuf);
}

void UARTsend(unsigned char *msg, unsigned char num)
{
unsigned char i;
	for(i=0;i<num;i++)
		{
			BUFFpush(&txbuf,msg++);
		}
		
	//Enable UDRE0	
	if(!(UCSR0B & (1<<UDRIE0)))		//Data register empty interrupt is disabled
		{
			CLB(UCSR0B, RXEN0);		//Disable receiver
			STB(UCSR0B, UDRIE0);	//Enable Data Register Empty Interrupt
		}
}

//RX Interrupt
ISR(USART_RX_vect)
{
unsigned char ch;
	cli();							//Disable all interrupts
	
	ch = UDR0;
	BUFFpush(&rxbuf,&ch);
	
	//Timer to recognize the end of message
	//if nothing is received after  - end of message
	TIMER[RXFINISHED]=RXFINISHEDTMR;
	
	//Diagnostic - keep DP segment on the display off if ECU replies
	IO_SET(DP_BIT);
	
	sei();							//Enable all interrupts
}

//TX interrupt
ISR(USART_TX_vect)
{
	cli();							//Disable all interrupts
	
	CLB(UCSR0B, TXC0);				//Disable TX Completed interrupt
	STB(UCSR0B, RXEN0);				//Enable receiver
	
	sei();							//Enable all interrupts
}

//USART_UDRE_vect
ISR(USART_UDRE_vect)
{
unsigned char ch;

	cli();								//Disable all interrupts
	
	//According to ISO14230 space between transmitted bytes should be 5-20ms
	//This is not fulfilled 100% here - we are faster !
	//If comm doesn't work properly uncomment next line (delay 6ms)
	//_delay_ms(6);
	
	if(BUFFpop(&txbuf,&ch))				//if buffer is not empty
		UDR0 = ch;						//keep sending
	else
		{
			CLB(UCSR0B, UDRIE0);		//if buffer is empty, disable interrupt from UDRE
			STB(UCSR0B, TXC0);			//enable interrupt from TX Completed
		}
		
	sei();								//Enable all interrupts
}