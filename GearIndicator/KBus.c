/*
 * KBus.c
 * Part of project: GearIndicator
 * Created: 25.05.2015 8:54:04
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
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include "uart.h"
#include "bikes.h"
#include "kbus.h"
#include "timers.h"
#include "eeproma.h"
#include <string.h>

//State of the K-Bus communication
tKBUS_state KBUS_state=KBUS_S_IDLE;		

////////////////////////////////////////////
//KBUS commands - partially :-)
////////////////////////////////////////////
//StartCommunication Request 
tKBUS_Msg KBUS_STC_req = {.form=ADDRESS_NO_LBYTE,					//Message with address without additional message length byte
						.header={
								.HForm_MsgLen=PHYSICAL_ADDR + 1,//Physical addressing + message  lenght (1 byte)
								.Tgt=0x11,						//Target address -> ECU address (0x11 - Kawa)
								.Src=IND_ADDR					//Source address = Indicator address
								},
						.data.Sld=STC,							//startCommunication Request Service Id
						.cs=0x04								//Check sum
						};


//Byte	Name				Value		Description
//-----------------------------------------------
//0		Fmt					0x81		Physical addressing PHYSICAL_ADDR<<6 + length of information (1 byte) = 0x81
//1		Tgt					<Ttg>		Address of target device (ECU)
//2		Src					IND_ADDR	Address of source device (GEAR INDICATOR)
//3		STC					0x81		startCommunication Request Service Id		
//4		CS					<CS>		Example: CHS = 04 = 81+11+F1+81 modulo 0x100 -> 0x81,0x11,0xF1,0x81,0x04
//			
////////////////////////////////////////////
//startCommunication Positive Response
unsigned char KeyBytes[2]={0xEA,0x8F};
	
tKBUS_Msg KBUS_STC_res = {.form=ADDRESS_LBYTE,					//Message with address and with extra length byte
						.header={
								.HForm_MsgLen=PHYSICAL_ADDR,	//Physical addressing
								.Tgt=IND_ADDR,					//Target address = Indicator address
								.Src=0x11,						//Source address = ECU
								.Len=3							//Data length - number of data bytes
								},
						.data.Sld=STCPR,						//startCommunication Positive Response Service Id
						.data.Data=KeyBytes,
						.cs=0xBF								//Check sum
						};

//Byte	Name				Value        Description
//------------------------------------------------------------------------------
//0		Fmt					0x80		Address mode: physical addressing + Len
//1		Tgt					IND_ADDR	Address of target devide (GEAR INDICATOR)
//2		Src					<ADDR>		Address of source device (ECU)
//3		Len					0x03		3 Bytes following...
//4		STCPR				0xC1		startCommunication Positive Response Service Id
//5		KB1					0xEA		Header with target and source address information
//6		KB2					0x8F		Additional length byte used
//7		CS					<CS>		Example: CHS BF = 80+F1+...+8F modulo 0x100 ->0x80,0x11,0x03,0xC1,0xEA,0x8F,0xBF
//
////////////////////////////////////////////	
//StartDiagnosticSession Request 
unsigned char KBUS_START_DIAG[7] =		{PHYSICAL_ADDR,0x11,IND_ADDR,0x02,SDS,0x80,0x14};	
	
unsigned char diagnosticMode=0x80;							//manufacterSpecific - 00-7F reserved, 80-FF for vehicle manufacturer specific use
tKBUS_Msg KBUS_SDS_req = {.form=ADDRESS_LBYTE,				//Message with address and with extra length byte
						.header={
							.HForm_MsgLen=PHYSICAL_ADDR,	//Physical addressing
							.Tgt=0x11,						//Target address -> ECU address (0x11 - Kawa)
							.Src=IND_ADDR,					//Source address = Indicator address
							.Len=2							//Data length - number of data bytes
							},
						.data.Sld=SDS,						//startDiagnosticSession Request Service Id
						.data.Data=&diagnosticMode,			//manufacterSpecific - 00-7F reserved, 80-FF for vehicle manufacturer specific use
						.cs=0x14							//Check sum
						};	
		
//Byte	Name				Value		 Description
//-------------------------------------------------------------------------------------
//0		Fmt					0x80		Address mode: physical addressing + Len
//1		Tgt					<ADDR>		Address of target device (ECU)
//2		Src					IND_ADDR	Address of source device (GEAR INDICATOR)
//3		Len					0x02		2 Bytes following...
//4		SDS					0x10		startDiagnosticSession Request Service Id
//5		diagnosticMode		0x80		manufacterSpecific - 00-7F reserved, 80-FF for vehicle manufacturer specific use
//6		CS					<CS>		Example: CHS = 14 = 80+11+F1+02+10+80 modulo 0x100 ->0x80,0x11,0xF1,0x02,0x10,0x80,0x14
//
////////////////////////////////////////////
//startDiagnosticSession Positive Response
unsigned char KBUS_START_DIAG_RPL[7] =	{PHYSICAL_ADDR,IND_ADDR,0x11,0x02,SDSPR,0x80,0x54};		
	
tKBUS_Msg KBUS_SDS_res = {.form=ADDRESS_LBYTE,				//Message with address and with extra length byte
						.header={
							.HForm_MsgLen=PHYSICAL_ADDR,	//Physical addressing
							.Tgt=IND_ADDR,					//Target address = indicator address
							.Src=0x11,						//Source address = ECU
							.Len=2							//Data length - number of data bytes
							},
						.data.Sld=SDSPR,					//startDiagnosticSession Request Service Id
						.data.Data=&diagnosticMode,			//manufacterSpecific - 00-7F reserved, 80-FF for vehicle manufacturer specific use
						.cs=0x54							//Check sum
						};	

//Byte	Name				Value		Description
//-----------------------------------------------------------------------------------
//0		Fmt					0x80		Address mode: physical addressing +Len
//1		Tgt					IND_ADDR	Address of target device (GEAR INDICATOR)
//2		Src					<ADDR>		Address of source device (ECU)
//3		Len					0x02		2 Bytes following...
//4		SDSPR				0x50		startDiagnosticSession Positive Response Service Id
//5		Diagnostic mode		0x80		manufacterSpecific
//6		Checksum			<CS>		Example: CS = 54 ->0x80,0xF1,0x11,0x02,0x50,0x80,0x54
//
////////////////////////////////////////////
//readDataByLocalIdentifier Request
unsigned char KBUS_READ_DATA[7]  =		{PHYSICAL_ADDR,0x11,IND_ADDR,0x02,RDBLID,0x0B,0xB0};

unsigned char RLOCID=0xB0;
tKBUS_Msg KBUS_RDBLID_req = {.form=ADDRESS_LBYTE,				//Message with address and with extra length byte
							.header={
								.HForm_MsgLen=PHYSICAL_ADDR,	//Physical addressing
								.Tgt=0x11,						//Target address = ECU
								.Src=IND_ADDR,					//Source address = Indicator address
								.Len=2							//Data length - number of data bytes
								},
							.data.Sld=RDBLID,					//readDataByLocalIdentifier Request Service Id
							.data.Data=&RLOCID,					//recordLocalIdentifier (to be read)
							.cs=0xB0							//Check sum	
							};
							
//Byte    Name				Value		Description
//-----------------------------------------------
//0		Fmt					0x80		Address mode: physical addressing + Len
//1		Tgt					<ADDR>		Address of target devide (ECU)
//2		Src					IND_ADDR	Address of source device (GEAR INDICATOR)
//3		Len					0x02		Two byte following...
//4		RDBLID				0x21		readDataByLocalIdentifier Request Service Id
//5		RLOCID				<RLOCID>	recordLocalIdentifier (to be read)
//6		CS					<CS>		Example: CHS = B0 = 80+11+F1+02+21+0B modulo 0x100 ->0x80,0x11,0xF1,0x02,0x21,0x0B,0xB0
//
//////////////////////////////////////////////
//READ REG - reply
//Differs according to the manufacturer/model
//This is defined in bikes.c & bikes.h
tKBUS_Msg KBUS_reply_msg;

////////////////////////////////////////
//KBus protocol check sum calculation
unsigned char KBUS_checksum1(volatile unsigned char *msg, volatile unsigned char cnt)
{
unsigned char checksum=0, i;
	
	for(i=0;i<cnt;i++)
		checksum+= *(msg+i);
		
	return(checksum);
}

unsigned char KBUS_checksum(tKBUS_Msg *msg)
{
unsigned char checksum=0;
int i, MsgLen;
	
	//Header byte always
	checksum+=msg->header.HForm_MsgLen;	
	
	//Addresses include only if necessary
	if(msg->form==ADDRESS_NO_LBYTE || msg->form==ADDRESS_LBYTE)
		{
			checksum+=msg->header.Src;
			checksum+=msg->header.Tgt;
		}
			
	//Always there
	checksum+=msg->data.Sld;
		
	//Length byte only if necessary
	if(msg->form==ADDRESS_LBYTE || msg->form==NO_ADDRESS_LBYTE)
		{
			checksum+=msg->header.Len;
			for(i=0;i<msg->header.Len-1;i++)
				checksum+=msg->data.Data[i];
		}
	else
		{
			MsgLen=msg->header.HForm_MsgLen & 0b00111111;
			for(i=0;i<MsgLen-1;i++)
				checksum+=msg->data.Data[i];
		}

	msg->cs=checksum;
	return(checksum);
}
	
////////////////////////////////////////
//Performs KBus Fast Init
//25ms pulse to KBUS - Initialization
void KBUS_fast_init(void)
{
	cli();
	wdt_disable();			//WDT would expire during fast init
	
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
	wdt_enable(WDTO_250MS); //Enable watch dog
}

///////////////////////////////////////////////////
//Store message to UART tx buffer

void KBUS_msg_to_uart(tKBUS_Msg *msg)
{
int MsgLen;
unsigned char msgout[10];
unsigned char *msgptr;
int i;
	
	msgptr=msgout;

	//Fmt
	*msgptr++=msg->header.HForm_MsgLen;
	
	//Address 
	if(msg->form==ADDRESS_LBYTE || msg->form==ADDRESS_NO_LBYTE)
		{
			*msgptr++=msg->header.Tgt;
			*msgptr++=msg->header.Src;
		}
		
	//Additional Length byte
	if(msg->form==ADDRESS_LBYTE || msg->form==NO_ADDRESS_LBYTE)
		{
			*msgptr++=msg->header.Len;
		}
		
	//Sld
	*msgptr++=msg->data.Sld;
	
	
	//Data - Sld is first, so number of data = Len-1
	if(msg->form==ADDRESS_LBYTE || msg->form==NO_ADDRESS_LBYTE)
		{
			if(msg->header.Len>1)
				for(i=0;i<msg->header.Len-1;i++)
					*msgptr++=msg->data.Data[i];
		}
	else
		{
			//Get MsgLen from the header byte
			MsgLen = msg->header.HForm_MsgLen & 0b00111111;	
			if(MsgLen>1)
				for(i=0;i<MsgLen-1;i++)
					*msgptr++=msg->data.Data[i];
		}
		
	//Check sum	
	*msgptr++=KBUS_checksum(msg);	
	
	//Send it to UART
	UARTsend(msgout,msgptr-msgout);
}

//////////////////////////////////////////////////
//Sends K-Bus message to ECU with particular ECUno
void KBUS_send_msg(volatile unsigned char ECUno)
{
	switch(KBUS_state)
		{
			case KBUS_S_IDLE:
				KBUS_STC_req.header.Tgt = BIKES_DATA[ECUno].ecu_addr;
				KBUS_fast_init();
				KBUS_msg_to_uart(&KBUS_STC_req);
				break;
			
			case KBUS_S_COMM_STARTED:
				KBUS_RDBLID_req.header.Tgt = BIKES_DATA[ECUno].ecu_addr;
				RLOCID=BIKES_DATA[ECUno].local_id_gear;
				KBUS_msg_to_uart(&KBUS_RDBLID_req);
				break;
				
			case KBUS_S_DATA_POOLING:
				//if GEAR localID is different from RPM localID, read that different localID and we need to read RPM (if RPM is programmed or we are in programming mode)
				if(BIKES_DATA[ECUno].local_id_gear!=BIKES_DATA[ECUno].local_id_rpm && (rpmValue.rpm>0 || rpmValue.prgmode==0))
					{
						if(RLOCID==BIKES_DATA[ECUno].local_id_gear)
							RLOCID=BIKES_DATA[ECUno].local_id_rpm;
						else
							RLOCID=BIKES_DATA[ECUno].local_id_gear;
					}
				KBUS_msg_to_uart(&KBUS_RDBLID_req);
				break;
				
			default:
				break;
		}
}	

/////////////////////////////////////////////////////////////
//Checks reply from remote ECU according to actual KBus state
unsigned char KBUS_check_reply(volatile unsigned char ECUno, volatile unsigned char *msg)
{
int replylen;

	// Reply length: <FMT> <IND_ADDR> <ECUaddr> <Bytes Follow> <Sdl> byte[0]...byte[Bytes Follow-1] <CHS> => replylen=<Bytes Follow>+5
	replylen = msg[3]+5;
	
	switch(KBUS_state)
		{
			case KBUS_S_IDLE:
				//KBUS_START_COMM_RPL
				//{<FMT> <IND_ADDR> <ECUaddr> <0x03> <STCPR> <0xEA> <0x8F>  <CHS>};
				if(	msg[1]==IND_ADDR && \
					msg[2]==BIKES_DATA[ECUno].ecu_addr && \
					msg[4]==STCPR && \
					msg[replylen-1]==KBUS_checksum1(msg,replylen-1)
				  )
					{
						KBUS_state = KBUS_S_COMM_STARTED;
						return(TRUE);
						break;
					}
				else
					return(FALSE);
				break;
				
			case KBUS_S_COMM_STARTED:
			case KBUS_S_DATA_POOLING:
				//{<B0>	<B1>		<B2>		<B3>			<B4>		<B5>	<B6>						<BN>							<CHS>};
				//{<FMT> <IND_ADDR> <ECUaddr> 	<Bytes Follows> 	<RDBLIDPR> 	<Loc_Id>	<B6>...	...	...	...	...	...	<BN>							<CHS>};
				//{<FMT> <IND_ADDR> <0x11>		<0x03>			<RDBLIDPR>	<0x0B>	<GEAR>														<CHS>}; KAWASAKI01 
				//{<FMT> <IND_ADDR> <0x11>		<0x04>			<RDBLIDPR>	<0x09>	<RPM1> <RPM2>												<CHS>}; KAWASAKI01 	
				//{<FMT>	<IND_ADDR>	<0x12>		<0x34>			<RDBLIDPR>	<0x08>	<B6>...<B16> <RPM1> <RPM2> ...<B25>,<GEAR>,<B27>... ...<Byte55>	<CHS>}; SUZUKI01
				if( msg[1]==IND_ADDR && \
					msg[2]==BIKES_DATA[ECUno].ecu_addr && \
					msg[4]==RDBLIDPR && \
					(msg[5]==BIKES_DATA[ECUno].local_id_gear || msg[5]==BIKES_DATA[ECUno].local_id_rpm) && \
					msg[replylen-1]==KBUS_checksum1(msg,replylen-1)
				  )
					{
						KBUS_state=KBUS_S_DATA_POOLING;
						return(TRUE);
						break;
					}
				else
					return(FALSE);
				break;
				
			default:
				return(FALSE);
				break;
		}
}