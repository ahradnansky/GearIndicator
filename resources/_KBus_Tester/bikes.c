/*
 * kbus_Z750.c
 *
 * Created: 06.06.2014 14:33:06
 * Author: a.hradnansky
 */ 

#include "global.h"
#include <avr/io.h>
#include "bikes.h"


////////////////////////////////////////////
//BIKES ECUS and registers
//
//ECUs

//BIKE_DATA [bike no][ECU addr, data reg no, reply data byte no 0..X, reply lng]
unsigned char BIKE_DATA[NUM_OF_ECUS][4]=
{
	{0x11,0x0B, 6, 8},	//Kawasaki Z750, Versys 650, Z1000, Z800
	{0x12,0x08,26,57}	//Suzuki V-Storm
};
	
//READ REGISTER REPLIES DESCRIPTION
//
//****************************************
//0 - Kawasaki - BIKE_DATA[0][...]
//
//unsigned char KBUS_READ_REG_RPL[7] =	{0x80,0xF1,0x11,0x03,0x61,0x0B,<GEAR>,<CRC>};	
//0x80 0xF1 0x11 0x03 0x61 0x0B 0x00 0xF1	  IDLE
//0x80 0xF1 0x11 0x03 0x61 0x0B 0x01 0xF2     (GEAR 1)
//0x80 0xF1 0x11 0x03 0x61 0x0B 0x02 0xF3     (GEAR 2)
//0x80 0xF1 0x11 0x03 0x61 0x0B 0x03 0xF4     (GEAR 3)
//0x80 0xF1 0x11 0x03 0x61 0x0B 0x04 0xF5     (GEAR 4)
//0x80 0xF1 0x11 0x03 0x61 0x0B 0x05 0xF6     (GEAR 5)
//0x80 0xF1 0x11 0x03 0x61 0x0B 0x06 0xF7     (GEAR 6)
//
//Byte    Name					Value		Description
//-----------------------------------------------
//0    Format Byte				0x80		Address mode: with address information, physical addressing
//1    Target Byte				0xF1        Address of target devide (GEAR INDICATOR)
//2    Source Byte				0x11        Address of source device (ECU)
//3    Length Byte				0x03        Three byte following...
//4    Service ID				0x61        Positive reply to Request ReadDataByLocallIdentifier
//5    Parameter name			0x0B        "Gear position" Parameter
//6    Parameter value			n			n = gear position value
//7    Checksum					0xF1+n		Checksum = 80+F1+11+03+61+0B+n modulo 0x100 (OK)
//
//
//*****************************************
//1 - Suzuki - BIKE_DATA[1][...]
//
//unsigned char KBUS_READ_REG_RPL[7] =	{0x80,0xF1,0x12,0x34,0x61,0x08,<Byte6>...<Byte25>,<GEAR>,<Byte27>..<Byte55>,<CRC>};	
//0x80 0xF1 0x12 0x34 0x61 0x08 <Byte6>...<Byte25> <0x00> <Byte27>..<Byte55> <CRC>		IDLE
//0x80 0xF1 0x12 0x34 0x61 0x08 <Byte6>...<Byte25> <0x01> <Byte27>..<Byte55> <CRC>		(GEAR1)
//0x80 0xF1 0x12 0x34 0x61 0x08 <Byte6>...<Byte25> <0x02> <Byte27>..<Byte55> <CRC>		(GEAR2)
//0x80 0xF1 0x12 0x34 0x61 0x08 <Byte6>...<Byte25> <0x03> <Byte27>..<Byte55> <CRC>		(GEAR3)
//0x80 0xF1 0x12 0x34 0x61 0x08 <Byte6>...<Byte25> <0x04> <Byte27>..<Byte55> <CRC>		(GEAR4)
//0x80 0xF1 0x12 0x34 0x61 0x08 <Byte6>...<Byte25> <0x05> <Byte27>..<Byte55> <CRC>		(GEAR5)
//0x80 0xF1 0x12 0x34 0x61 0x08 <Byte6>...<Byte25> <0x06> <Byte27>..<Byte55> <CRC>		(GEAR6)
//
//Byte	Name					Value		Description
//-----------------------------------------------
//0		Format Byte				0x80		Address mode: with address information, physical addressing
//1		Target Byte				0xF1		Address of target devide (GEAR INDICATOR)
//2		Source Byte				0x12		Address of source device (ECU)
//3		Length Byte				0x34		52 bytes follow (0x61...0xFF without checksum 0x07)
//4		Service ID				0x61		Positive reply to Request ReadDataByLocallIdentifier
//5		Parameter name			0x08		Request was to read register 0x08, and data continue:
//6	 02
//7	 05
//8	 05
//9	 A0
//10 17
//11 69
//12 A2
//13 FF
//14 FF
//15 FF
//16							0x00		Speed = byte 16 * 2 in km/h
//17							0x00		RPM byte1
//18							0x00		RPM byte2: RPM = 10 * byte1 + byte2 / 10
//19							0x37		Throttle position: 0x37 = 0%, 0xDD = 100%
//20 B8
//21							0x6B		Engine Temperature = (byte21 - 48) / 1.6 in Celsius degrees
//22							0x61		Intake Air Temperature, same formula as above
//23 B9
//24 00
//25 FF
//26							0x00		Gear indicator 0-neutral; 1-1st gear...
//27 FF
//28 5E
//29 1F
//30 FF
//31 00
//32 00
//33 00
//34 00
//35 00
//36 00
//37 00
//38 00
//39 FF
//40 FF
//41 40
//42 40
//43 40
//44 40
//45 FF
//46 1A
//47 00
//48 CB
//49 1A
//50 30
//51 00
//52							0x04		Clutch sensor (bit 8?): 0x04 = clutch released, 0x14 clutch "pressed"
//53							0x00		Neutral sensor (bit 1?): 0x00 = neutral, 0x02 = in gear
//54 FF
//55 FF
//56							0x07		Checksum


