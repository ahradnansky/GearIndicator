/*
 * buffer.c
 * Part of project: GearIndicator
 * Created: 30.06.2014 14:39:41
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
#include "buffer.h"

//rx and tx circular buffers
t_buffer txbuf, rxbuf;

void BUFFinit(t_buffer *b)
{
	b->buffer_end = b->buffer + BUFF_LNG;
	BUFFflush(b);
}

void BUFFflush(t_buffer *b)
{
unsigned char i;

	b->count = 0;
	b->head = b->buffer;
	b->tail = b->buffer;
	for(i=0;i<BUFF_LNG;i++) b->buffer[i]=BUFFFILLCHAR;
}

unsigned char BUFFpush(t_buffer *b, unsigned char *ch)
{
	if(b->count == BUFF_LNG)
		return(0);			//buffer is full
		
	*b->head=*ch;
	b->head = b->head + 1;
	if(b->head == b->buffer_end)
		b->head = b->buffer;
	b->count++;
	return(1);				//OK
}

unsigned char BUFFpop(t_buffer *b, unsigned char *ch)
{
	if(b->count == 0)
		return(0);			//buffer is empty

	*ch = *b->tail;			//get the value from the buffer
	*b->tail=BUFFFILLCHAR;	//clear the place with default value
	b->tail = b->tail + 1;	
	if(b->tail == b->buffer_end)
		b->tail = b->buffer;
	b->count--;
	return(1);				//OK
}
