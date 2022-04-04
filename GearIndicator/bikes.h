/*
 * bikes.h
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

#ifndef BIKES_H_
#define BIKES_H_

//Bikes database record
typedef struct
{
	unsigned char ecu_addr;			//address of ECU
	unsigned char local_id_gear;	//local identifier where gear no is stored
	unsigned char byte_no_gear;		//byte number in local identifier data field (from ECU) which indicates the gear
	unsigned char data_bytes_gear;	//number of bytes of gear info
	unsigned char local_id_rpm;		//local identifier where RPM is stored
	unsigned char byte_no_rpm;		//byte number in local identifier data field (from ECU) which indicates RPM
	unsigned char data_bytes_rpm;	//number of bytes of RPM info
} bikedata;

//Bikes database size
enum
{
	KAWASAKI01,					//Z750, Versys 650, Z1000, Z800
	SUZUKI01,					//V-Strom, VL800
	NUM_OF_ECUS
};

//Bikes database data
extern bikedata volatile BIKES_DATA[NUM_OF_ECUS];

#endif /* BIKES_H_ */