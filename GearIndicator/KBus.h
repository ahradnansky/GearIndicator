/*
 * kbus.h
 * Part of project: GearIndicator
 * Created: 06.06.2014 14:33:21
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

#ifndef KBUS_H_
#define KBUS_H_

//Indicator address on the K-Bus
//Diagnostic equipment should have this address 
#define		IND_ADDR			0xF1	

//////////////////////////////////////////////////////////////////////
//Format byte definition
//A1 A0 L5 L4 L3 L2 L1 L0

//Header form
//A1,A0: Define the form of the header which will be used by the message.
#define		NO_ADDRESS			0x00			//No address information
#define		CARB				0x40			//Exception mode (CARB)
#define		PHYSICAL_ADDR		0x80			//With address information, physical addressing
#define		FUNCTIONAL_ADDR		0xC0			//With address information, functional addressing
//
//END OF: Format byte definition
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//Key Bytes definition
//
//Key Bytes - these are returned on STC - startCommunication Request Service Id - command as part of reply
//Example: 
//Reply is: 0x80,0x11,0x03,0xC1,0xEA,0x8F,0xBF
//KB1=0xEA, KB2=0x8F
//KB1=<KB1_EPAR> <1> <TP1> <TP0> <HB1> <HB0> <AL1> <AL0>
//KB1=<1> <1> <1> <0> <1> <0> <1> <0> 
//KB1=0b11101010=0xEA
//AL0=0 -  length inf. in format byte not supp.
//AL1=1 -  add. length byte supported
//HB0=0 -  1 byte header not supported
//HB1=1 -  Tgt/Src address in header supported
//TP0=0  -  normal timing parameter set
//TP1=1  -  normal timing parameter set
	
//KB1									//0(FALSE)							1(TRUE)
typedef struct  
{
	unsigned char	AL0:1;				//length inf. in format byte not supp.		length inf. in format byte supported
	unsigned char	AL1:1;				//add. length byte not supported			add. length byte supported
	unsigned char	HB0:1;				//1 byte header not supported			1 byte header supported
	unsigned char	HB1:1;				//Tgt/Src addr. in header not supported	Tgt/Src address in header supported
	unsigned char	TP0:1;				//normal timing parameter set			extended timing parameter set
	unsigned char	TP1:1;				//extended timing parameter set			normal timing parameter set
	unsigned char	:1;					//Always 1 (TRUE)
	unsigned char	KB1_EPAR:1;			//Odd parity
}tKeyByte1;

//KB2
#define		KB2		0x8F				//Always 8Fh
//
//END OF:Key Bytes definition
///////////////////////////////////////////////////////////////////////
//
//L5 … L0: Define the length of a message from the beginning of the data field
//(Service Identification byte included) to checksum byte (not included). 
//A message length of 1 to 63 bytes is possible. 
//If L0 to L5 = 0 then the additional length byte is included.

///////////////////////////////////////////////////////////////////////
//Message Structure Definition
//

//Message forms
typedef enum
{
	NO_ADDRESS_NO_LBYTE,					//Message without address and without extra length byte
	ADDRESS_NO_LBYTE,						//Message with address and without extra length byte
	NO_ADDRESS_LBYTE,						//Message without address and with extra length byte
	ADDRESS_LBYTE							//Message with address and with extra length byte
}tKBUS_Msg_form_t;

//Message Header type definition
typedef struct
{
	unsigned char			HForm_MsgLen;	//Message length 1-63, Define the length of a message from the beginning
											//of the data field (Service Identification byte included) to checksum byte (not included).
											//A message length of 1 to 63 bytes is possible.
											//If L0 to L5 = 0 then the additional length byte is included.
											//Header form - 0..3 (A1, A0)
	unsigned char			Tgt;			//Target address
	unsigned char			Src;			//Source address
	unsigned char			Len;			//Data length
}tKBUS_Msg_header;

//Message data type
typedef struct
{
	unsigned char			Sld;			//Service identification byte
	unsigned char*			Data;			//Data
}tKBUS_Msg_data_bytes;

///////////////////////////////////////////////////////////////////////
// K-bus Message Type definition
typedef struct
{
	tKBUS_Msg_form_t		form;			//Message form (4 forms)
	tKBUS_Msg_header		header;			//Message header (1-4 bytes according to Message form)
	tKBUS_Msg_data_bytes	data;			//Data 
	unsigned char			cs	;			//Check sum
}tKBUS_Msg;
//
//END OF: Message Type definition
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//K-Bus Services Id's (Necessary only)
#define STC			0x81	//startCommunication Request Service Id
#define STCPR		0xC1	//startCommunication Positive Response Service Id
#define SPC			0x82	//stopCommunication Request Service Id
#define SPCPR		0xC2	//stopCommunication Positive Response Service Id
#define SPCNR		0x7F	//stopCommunication Negative Response Service Id

#define SDS			0x10	//startDiagnosticSession Request Service Id
#define SDSPR		0x50	//startDiagnosticSession Positive Response Service Id
#define SPDS		0x20	//stopDiagnosticSession Request Service Id
#define SPDSPR		0x60	//stopDiagnosticSession Positive Response Service Id

#define RDBLID		0x21	//readDataByLocalIdentifier Request Service Id
#define	RDBLIDPR	0x61	//readDataByLocalIdentifier Positive Response Service Id

#define NACK		0x7F	//negativeResponse Service Id

///////////////////////////////////////////////////////////////////////
//K-Bus states definition (matter of implementation)
typedef enum
{
		KBUS_S_IDLE,			//No comm, K-Bus in idle mode
		KBUS_S_COMM_STARTED,	//Communication to ECU started		
		KBUS_S_DATA_POOLING,	//Data pooling active
		NUM_OF_KBUS_STATES
}tKBUS_state;
	
extern tKBUS_state KBUS_state;
//
//END OF:K-Bus states definition 
///////////////////////////////////////////////////////////////////////

//Functions prototypes
//
//Calculates KBus check sum
unsigned char KBUS_checksum1(volatile unsigned char *msg, volatile unsigned char cnt);
unsigned char KBUS_checksum(tKBUS_Msg *msg);

//Performs KBus Fast Init - at the start disables all interrupts and watchdog, enables all at the end
void KBUS_fast_init(void);

//Send message to KBus according to actual KBus state, ECUno - ECU from bikedata struct
void KBUS_send_msg(volatile unsigned char ECUno);

//Checks reply from remote ECU according to actual KBus state, ECUno - ECU from bikedata struct
unsigned char KBUS_check_reply(volatile unsigned char ECUno, volatile unsigned char *msg);

#endif /* KBUS_H_ */
