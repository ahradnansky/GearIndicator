/*
 * bikes.c
 * Part of project: GearIndicator
 * Created: 06.06.2014 14:33:06
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
#include <avr/io.h>
#include "bikes.h"


////////////////////////////////////////////
//BIKES ECUS and registers
//
//ECUs

//BIKES_DATA[ECU_no]:{<ecu_addr>,<local_Id>,<byte_no>]
bikedata volatile BIKES_DATA[NUM_OF_ECUS]=
{
	// KAWASAKI01: Z750, Versys 650, Z1000, Z800 -> ECU 11h, Register 0Bh, Gear is 6th byte in the reply msg, reply length is 8 bytes	
	//actualECU=0
	{
		.ecu_addr=0x11,
		.local_id_gear=0x0B,
		.byte_no_gear=6,
		.local_id_rpm=0x09,
		.byte_no_rpm=6				//value in byte no 6 and 7 (2 bytes)
	},
	// SUZUKI01: VL800, V-Strom -> ECU 12h, Register 08h, Gear is 26th byte in the reply msg, reply lng. is 57
	//actualECU=1
	{
		.ecu_addr=0x12,
		.local_id_gear=0x08,
		.byte_no_gear=26,
		.local_id_rpm=0x08,
		.byte_no_rpm=17				//value in byte no 17 and 18 (2 bytes)
	}
};
	
//READ REGISTER REPLIES DESCRIPTION
///////////////////////////////////////////////////////
//KAWASAKI01 
//
///////////////////////////////////////////////////////
/*     ..../'''\....
//  /             \
// | B/Y  G/B  |
// |               |
// | R/Y W/Y |
// |              |
// \.............../
*/
//Connection to KDS - Kawasaki diagnostic system - black (grey) 4 pole connector
//	Signal/Pwr		SPEED Indicator			Bike's KDS
//	+12V			RED						WHITE/YELLOW W/Y
//	GND			BLUE					BLACK/YELLOW B/Y	
//	K-BUS			GREEN					RED/YELLOW   R/Y
//
//BIKE_DATA[KAWASAKI01][...]
//
//GEAR NUMBER
//unsigned char KBUS_READ_REG_RPL[8] =	{0x80,0xF1,0x11,0x03,0x61,0x0B,<GEAR>,<CRC>};	
//0x80 0xF1 0x11 0x03 0x61 0x0B 0x00 0xF1	 NEUTRAL
//0x80 0xF1 0x11 0x03 0x61 0x0B 0x01 0xF2     (GEAR 1)
//....
//
//Byte    Name					Value		Description
//-----------------------------------------------
//0    Format Byte				0x80		Address mode: with address information, physical addressing
//1    Target Byte				0xF1		Address of target devide (GEAR INDICATOR)
//2    Source Byte				0x11		Address of source device (ECU)
//3    Length Byte				0x03		Three byte following...
//4    Service ID				0x61		Positive reply to Request ReadDataByLocallIdentifier
//5    Parameter name			0x0B		"Gear position" Parameter
//6    Parameter value			n			n = gear position value
//7    Checksum				0xF1+n		Checksum = 80+F1+11+03+61+0B+n modulo 0x100 (OK)
//
//
//RPM
//unsigned char KBUS_READ_REG_RPL[9] =	{0x80,0xF1,0x11,0x04,0x61,0x09,<RPM1>,<RPM2>,<CRC>};
//0x80 0xF1 0x11 0x03 0x61 0x09 0x32 0x00 0x21     (RPM=5000)
//....
//
//Byte    Name					Value		Description
//-----------------------------------------------
//0    Format Byte				0x80		Address mode: with address information, physical addressing
//1    Target Byte				0xF1		Address of target devide (GEAR INDICATOR)
//2    Source Byte				0x11		Address of source device (ECU)
//3    Length Byte				0x04		Three byte following...
//4    Service ID				0x61		Positive reply to Request ReadDataByLocallIdentifier
//5    Parameter name			0x09		"RPM" Parameter
//6    Parameter value			RPM1		RPM1 value
//7	  Parameter value			RPM2		RPM2 value (RPM = RPM1*100+RPM2)
//8    Checksum							Checksum = 80+F1+11+03+61+09+RPM1+RPM2 modulo 0x100
//
//
//////////////////////////////////////////////////////
//SUZUKI01
//
//////////////////////////////////////////////////////
//VL800/2010
//
/*     .........../'''\...........
//  /   1     2    3     \
// | O/GN  --   W/R  |
// |    4    5    6       |
// | --   R/GY BK/W  |
// \............................../
//
//OTHERS (from WEB)
//
//     ............./'''\...........
//  /   1     2    3	   \
// | O/GN  W   W/R   |
// |    4     5    6	    |
// |   BN   GY  BK/W  |
// \................................/
*/
// 1	ORANGE/GREEN	+12V
// 2	WHITE			Serial Out
// 3	WHITE/RED		Dealed Mode
// 4	BROWN			Serial In
// 5	GREY			K-Line (K-BUS)
// 6	BLACK/WHITE		GND
//
//Connection to SDS (Suzuki diagnostic system) - white 6 pole connector
//	Signal/Pwr		SPEED Indicator			Bike's SDS
//	+12V			RED						ORANGE/GREEN O/GN		(1)
//	GND				BLUE						BLACK/WHITE  BK/W		(6)
//	K-BUS			GREEN					GREY		 GY or R/GY	(5)
//
//
//BIKE_DATA[SUZUKI01][...]
//
//unsigned char KBUS_READ_REG_RPL[7] =	{0x80,0xF1,0x12,0x34,0x61,0x08,<Byte6>...<Byte25>,<GEAR>,<Byte27>..<Byte55>,<CRC>};	
//0x80 0xF1 0x12 0x34 0x61 0x08 <Byte6>...<Byte25> <0x00> <Byte27>..<Byte55> <CRC>		NEUTRAL
//0x80 0xF1 0x12 0x34 0x61 0x08 <Byte6>...<Byte25> <0x01> <Byte27>..<Byte55> <CRC>		(GEAR1)
//...
//
//Byte	Name					Value		Description
//-----------------------------------------------
//0		Format Byte				0x80		Address mode: with address information, physical addressing
//1		Target Byte				0xF1		Address of target devide (GEAR INDICATOR)
//2		Source Byte				0x12		Address of source device (ECU)
//3		Length Byte				0x34		52 bytes follow (0x61...0xFF without checksum 0x07)
//4		Service ID				0x61		Positive reply to Request ReadDataByLocallIdentifier
//5		Parameter name			0x08		Request was to read register 0x08, and data continue:
//...
//...
//16								0x00		Speed = byte16 * 2 in km/h
//17								0x00		RPM Byte1
//18								0xFF		RPM byte2 (THIS WAS WRONG INFO RPM=10*byte1+byte2/10 - DL650 this byte is always 0xFF): RPM=byte1*62.5
//19								0x37		Throttle position: 0x37 = 0%, 0xDD = 100%
//20 B8
//21								0x6B		Engine Temperature = (byte21 - 48) / 1.6 in Celsius degrees
//22								0x61		Intake Air Temperature, same formula as above
//23 B9
//24 00
//25 FF
//26								0x00		Gear indicator 0-neutral; 1-1st...6-6th gear
//...
//...
//52								0x04		Clutch sensor (bit 8?): 0x04 = clutch released, 0x14 clutch "pressed"
//53								0x00		Neutral sensor (bit 1?): 0x00 = neutral, 0x02 = in gear
//54 FF
//55 FF
//56								0x07		Checksum
