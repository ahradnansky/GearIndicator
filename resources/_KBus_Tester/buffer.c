/*
 * buffer.c
 *
 * Created: 30.06.2014 14:39:41
 * Author: a.hradnansky
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
