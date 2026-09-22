/*
 * KBus.c
 *
 * Created: 25.05.2015 8:54:04
 *  Author: Andrej
 */ 


#include "global.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include "uart.h"
#include "KBus.h"


////////////////////////////////////////////
//KBUS commands - partially :-)
//
//START COMMUNICATION SESSION COMMAND
unsigned char KBUS_START_COMM[5] =		{0x81,0x11,0xF1,0x81,0x04};			
//Byte	Name				Value		Description
//-----------------------------------------------
//0		Format Byte			0x81        Address mode: with address information, physical addressing (message is 1 Byte long)
//1		Target Byte			<ADDR>       Address of target device (ECU)
//2		Source Byte			0xF1        Address of source device (GEAR INDICATOR)
//3		Request service ID	0x81        START COMMUNICATION REQUEST
//4		Checksum			<CHS>		Example: CHS = 04 = 81+11+F1+81 modulo 0x100
//			
//START COMM - reply: Start comm request accepted 0xC1
//unsigned char KBUS_START_COMM_RPL[8] = 	{0x80,0xF1,0x11,0x03,0xC1,0xEA,0x8F,0xBF};
//Byte	Name				Value        Description
//-----------------------------------------------
//0		Format Byte			0x80        Address mode: with address information, physical addressing
//1		Target Byte			0xF1        Address of target devide (GEAR INDICATOR)
//2		Source Byte			<ADDR>        Address of source device (ECU)
//3		Length Byte			0x03        3 Bytes following...
//4		Response			0xC1        START COMMUNICATION REQUEST ACCEPTED
//5		Key Byte 1			0xEA        Header with target and source address information
//6		Key Byte 2			0x8F        Additional length byte used
//7		Checksum			<CHS>       Example: CHS BF = 80+F1+...+8F modulo 0x100
	
//START DIAGNOSTIC SESSION COMMAND	
unsigned char KBUS_START_DIAG[7] =		{0x80,0x11,0xF1,0x02,0x10,0x80,0x14};		
//Byte	Name                Value       Description
//-----------------------------------------------
//0		Format Byte         0x80        Address mode: with address information, physical addressing
//1		Target Byte         <ADDR>      Address of target device (ECU)
//2		Source Byte         0xF1        Address of source device (GEAR INDICATOR)
//3		Length Byte         0x02        2 Bytes following...
//4		Request service ID  0x10        START DIAGNOSTIC REQUEST
//5		Diagnostic mode     0x80        manufacterSpecific
//6		Checksum			<CHS>       Example: CHS = 14 = 80+11+F1+02+10+80 modulo 0x100
//
//START DIAG SESSION - reply: Diagnostic request accepted 0x50
//unsigned char KBUS_START_DIAG_RPL[7] =	{0x80,0xF1,0x11,0x02,0x50,0x80,0x54};		
//Byte	Name                Value       Description
//-----------------------------------------------
//0		Format Byte         0x80        Address mode: with address information, physical addressing
//1		Target Byte         0xF1        Address of target device (GEAR INDICATOR)
//2		Source Byte         <ADDR>      Address of source device (ECU)
//3		Length Byte         0x02        2 Bytes following...
//4		Request service ID  0x50        START DIAGNOSTIC REQUEST ACCEPTED
//5		Diagnostic mode     0x80        manufacterSpecific
//6		Checksum			0x54        Checksum
	
//READ REGISTER COMMAND
//Register 0x0B
unsigned char KBUS_READ_REG[7]  =		{0x80,0x11,0xF1,0x02,0x21,0x0B,0xB0};
//Byte    Name					Value		Description
//-----------------------------------------------
//0    Format Byte				0x80		Address mode: with address information, physical addressing
//1    Target Byte				<ADDR>      Address of target devide (ECU)
//2    Source Byte				0xF1        Address of source device (GEAR INDICATOR)
//3    Length Byte				0x02        Two byte following...
//4    Service ID               0x21        ReadDataByLocallIdentifier
//5    Parameter name			<REG>       Register number (to be read)
//6    Checksum					<CHS>       Example: CHS = B0 = 80+11+F1+02+21+0B modulo 0x100
//
//
//READ REG - reply
//Differs according to the manufacturer/model
//This is defined in particular files - kawasaki.c, suzuki.c, etc. and apropriate .h files


////////////////////////////////////////
//Check sum calculation
unsigned char KBUS_checksum(unsigned char * msg, unsigned char cnt)
{
unsigned char checksum=0, i;
	
	for(i=0;i<cnt;i++)
		checksum+= *(msg+i);
		
	return(checksum);
}
	
////////////////////////////////////////
//25ms pulse to KBUS - Initialization
void KBUS_fast_init(unsigned char ECU_addr)
{
	cli();
	wdt_disable();			//WDT would expire during fast init
	
	//Set corect address & checksums
	KBUS_START_COMM[1]=ECU_addr;
	KBUS_START_COMM[4]=KBUS_checksum(KBUS_START_COMM,4);
	KBUS_START_DIAG[1]=ECU_addr;
	KBUS_START_DIAG[6]=KBUS_checksum(KBUS_START_DIAG,6);
		
	//RX Disable
	CLB(UCSR0B, RXEN0);
	//TX Disable
	CLB(UCSR0B, TXEN0);

	
	STB(UARTDR,TX);			//TXD is output
	STB(UARTPORT,TX);		//TX high
	_delay_ms(400);			//Just be sure we fulfill TIdle time - min.300ms first transmission after power up)
	
	//KBUS FAST INIT
	//(--FIRST LOW--)
	CLB(UARTPORT,TX);		//Keep TX Lo for min. 25ms +/- 1ms (TiniL according to ISO14230)
	_delay_ms(25);
	STB(UARTPORT,TX);		//and TX Hi for another 25ms +/- 1ms (TWuP according to ISO14230)
	_delay_ms(25);	
	
	//TX Enable
	STB(UCSR0B, TXEN0);
	sei();					//Enable all interrupts
	
	//Start communication
	//First falling edge must be issued 49-51ms after first CLB(UARTPORT, TX) (--FIRST LOW--)
	//It looks in AVR studio simulator that it will happen in 50.069ms - OK :-)
	UARTsendmsg(KBUS_START_COMM,5);
	//we don't care for response here
	_delay_ms(500);
	UARTsendmsg(KBUS_START_DIAG,7);
	//we don't care for response here
	_delay_ms(500);
	
	//wdt_enable(WDTO_250MS); //Enable watch dog
}

