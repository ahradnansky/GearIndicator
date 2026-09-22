/*
 * uart.h
 *
 * Created: 13. 3. 2014 16:39:18
 * Author:	a.hradnansky
 */ 

#ifndef UART_H_
#define UART_H_

//UART port
#define UARTPORT	PORTD
#define UARTDR		DDRD

#define RX			0			//AtMega328P - PORTD bit0
#define TX			1			//AtMega328P - PORTD bit1


//UART
#define UART_BAUD_RATE			10400
#define UART_BAUD_REGISTER		F_CPU / (16UL * UART_BAUD_RATE) - 1

//Functions
void UARTinit(void);
void UARTsendmsg(unsigned char *msg, unsigned char num);

#endif /* UART_H_ */