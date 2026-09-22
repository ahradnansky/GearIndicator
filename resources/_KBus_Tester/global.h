/*
 * global.h
 *
 * Created: 11.11.2013 23:20:11
 * Modified: 7/2014
 * Author: a.hradnansky
 */ 
#ifndef GLOBAL_H_
#define GLOBAL_H_

//General
#define TRUE						1
#define FALSE						0
#define ON							1
#define OFF							0
#define NULL						((void *)0)
#define NO							0
#define YES							1

//HW version - compilation of the software
//1 - the first version Z750 (single side PCB,low cost)
//2 - the second version, 20x30x15mm (2x double sided PCB)
#define HW_VERSION					1
#define PWM							NO

//TEST Software compile
#define TEST						NO

//Communication 
//Number of re-tries to try to communicate to ECU before re-init
#define NOOFRETRIES					3

//Application CPU freq.
#define F_CPU						8000000UL			//8 MHz

//Animation times]
#define TANIM						15					//animation change 15ms

//Global MACROS	
#define	STB(PORT, BIT)				PORT|=(1<<BIT)		//set bit
#define CLB(PORT, BIT)				PORT&=~(1<<BIT)		//clear bit
#define TGL(PORT, BIT)				PORT^=(1<<BIT)		//toggle bit
#define HI(x)						((x)>>8)			//upper byte of the word
#define LO(x)						((x) & 0xFF)		//lower byte of the word
#define HNIBB(x)					((x)>>4)			//upper nibble of the byte
#define LNIBB(x)					((x) & 0x0F)		//lowed nibble of the byte
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