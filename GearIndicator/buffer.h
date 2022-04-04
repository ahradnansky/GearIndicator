/*
 * buffer.h
 * Part of project: GearIndicator
 * Created: 30.06.2014 14:40:58
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

#ifndef BUFFER_H_
#define BUFFER_H_

//Communication Buffer length
//RX and TX same size
#define BUFF_LNG				100						//Communication buffer length
#define BUFFFILLCHAR			0xFF					//Value to fill buffer after reading the value

//_buffer typedef
typedef struct _buffer
{
	unsigned char	buffer[BUFF_LNG];			// data buffer
	unsigned char	*buffer_end;				// end of data buffer
	int				count;						// number of items in the buffer
	unsigned char 	*head;						// pointer to head
	unsigned char 	*tail;						// pointer to tail
} t_buffer;

//Global variables
extern t_buffer rxbuf;
extern t_buffer txbuf;

//prototypes
void BUFFinit(t_buffer *b);
void BUFFflush(t_buffer *b);
unsigned char BUFFpush(t_buffer *b, unsigned char *ch);
unsigned char BUFFpop(t_buffer *b, unsigned char *ch);

#endif /* BUFFER_H_ */