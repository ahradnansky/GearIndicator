/*
 * buffer.h
 *
 * Created: 30.06.2014 14:40:58
 *  Author: a.hradnansky
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