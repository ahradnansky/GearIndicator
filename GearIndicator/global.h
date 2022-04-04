/*
 * global.h
 * Part of project: GearIndicator
 * Created: 11.11.2013 23:20:11
 * Modified: 7/2014
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
 
#ifndef GLOBAL_H_
#define GLOBAL_H_

//General
#define TRUE						0x01
#define FALSE						0x00
#define ON							0x01
#define OFF							0x00
#define NULL						((void *)0)
#define NO							0x00
#define YES							0x01

//Communication 
//Number of re-tries to try to communicate to ECU before re-init
#define NOOFRETRIES					3

//Application CPU freq.
#define F_CPU						8000000UL			//8 MHz

//Global MACROS	
#define	STB(PORT, BIT)				PORT|=(1<<BIT)		//set bit
#define CLB(PORT, BIT)				PORT&=~(1<<BIT)		//clear bit
#define TGL(PORT, BIT)				PORT^=(1<<BIT)		//toggle bit
#define HI(x)						((x)>>8)			//upper byte of the word
#define LO(x)						((x) & 0xFF)		//lower byte of the word

//SET/CLEAR bit of the port, etc.
//Example:
//#define NAME_OF_THE_IO_BIT(f)		f(D,4)				//bit 4 of the D port
//
#define IO_PORT(f)					f(_IO_PORT)
#define _IO_PORT(port,bit)			(PORT ## port)
#define IO_DDR(f)					f(_IO_DDR)
#define _IO_DDR(port,bit)			(DDR ## port)
#define IO_BIT(f)					f(_IO_BIT)
#define _IO_BIT(port,bit)			(bit)
#define IO_SET(f)					f(_IO_SET)
#define _IO_SET(port,bit)			STB( _IO_PORT(port,bit), _IO_BIT(port,bit))
#define IO_CLR(f)					f(_IO_CLR)
#define _IO_CLR(port,bit)			CLB( _IO_PORT(port,bit), _IO_BIT(port,bit))

#endif /* GLOBAL_H_ */