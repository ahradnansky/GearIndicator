/*
 * uart.h
 * Part of project: GearIndicator
 * Created: 13. 3. 2014 16:39:18
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

#ifndef UART_H_
#define UART_H_

//UART port
#define UARTPORT	PORTD
#define UARTDR		DDRD
#define UARTPIN		PIND

#define RX			0			//AtMega328P - PORTD bit0
#define TX			1			//AtMega328P - PORTD bit1


//UART
#define UART_BAUD_RATE			10400
#define UART_BAUD_REGISTER		F_CPU / (16UL * UART_BAUD_RATE) - 1

//Functions
void UARTinit(void);
void UARTsend(unsigned char *msg, unsigned char num);

#endif /* UART_H_ */