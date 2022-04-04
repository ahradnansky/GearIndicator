/*
 * led.c
 * Part of project: GearIndicator
 * Created: 9. 6. 2014 15:05
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
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "ascii_7seg.h"
#include "led.h"

//Global - displayed character
char volatile displayedchar;
char volatile f_dispblink=FALSE;

////////////////////////////////////////////////
//LEDInit
//Initializes LED display

void LEDinit(void)
{
	//Set LED segments pins as outputs
	STB(IO_DDR(A_BIT),IO_BIT(A_BIT));
	STB(IO_DDR(B_BIT),IO_BIT(B_BIT));
	STB(IO_DDR(C_BIT),IO_BIT(C_BIT));
	STB(IO_DDR(D_BIT),IO_BIT(D_BIT));
	STB(IO_DDR(E_BIT),IO_BIT(E_BIT));
	STB(IO_DDR(F_BIT),IO_BIT(F_BIT));
	STB(IO_DDR(G_BIT),IO_BIT(G_BIT));
	STB(IO_DDR(DP_BIT),IO_BIT(DP_BIT));

	//Display off
	LEDclear();
	IO_SET(DP_BIT);
}

////////////////////////////////////////////////
//LEDOutch
//Outputs char to display
void LEDoutch(unsigned char ch)
{		
unsigned char tmp;
	
	displayedchar = ch;
	tmp = ~pgm_read_byte(&ASCII_TBL_7SEG[(unsigned char)ch]);
	
	asm volatile (
	    "sbrs %0,%1""\n\t"
	    "cbi  %2,%3""\n\t"
		"sbrc %0,%1""\n\t"
		"sbi  %2,%3""\n\t"
	    ::"r"(tmp),"M"(A_SEG),"I"(_SFR_IO_ADDR(IO_PORT(A_BIT))),"M"(IO_BIT(A_BIT)));
	asm volatile (
		"sbrs %0,%1""\n\t"
		"cbi  %2,%3""\n\t"
		"sbrc %0,%1""\n\t"
		"sbi  %2,%3""\n\t"
		::"r"(tmp),"M"(B_SEG),"I"(_SFR_IO_ADDR(IO_PORT(B_BIT))),"M"(IO_BIT(B_BIT)));
	asm volatile (
		"sbrs %0,%1""\n\t"
		"cbi  %2,%3""\n\t"
		"sbrc %0,%1""\n\t"
		"sbi  %2,%3""\n\t"
		::"r"(tmp),"M"(C_SEG),"I"(_SFR_IO_ADDR(IO_PORT(C_BIT))),"M"(IO_BIT(C_BIT)));
	asm volatile (
		"sbrs %0,%1""\n\t"
		"cbi  %2,%3""\n\t"
		"sbrc %0,%1""\n\t"
		"sbi  %2,%3""\n\t"
		::"r"(tmp),"M"(D_SEG),"I"(_SFR_IO_ADDR(IO_PORT(D_BIT))),"M"(IO_BIT(D_BIT)));	
	asm volatile (
		"sbrs %0,%1""\n\t"
		"cbi  %2,%3""\n\t"
		"sbrc %0,%1""\n\t"
		"sbi  %2,%3""\n\t"
		::"r"(tmp),"M"(E_SEG),"I"(_SFR_IO_ADDR(IO_PORT(E_BIT))),"M"(IO_BIT(E_BIT)));
	asm volatile (
		"sbrs %0,%1""\n\t"
		"cbi  %2,%3""\n\t"
		"sbrc %0,%1""\n\t"
		"sbi  %2,%3""\n\t"
		::"r"(tmp),"M"(F_SEG),"I"(_SFR_IO_ADDR(IO_PORT(F_BIT))),"M"(IO_BIT(F_BIT)));
	asm volatile (
		"sbrs %0,%1""\n\t"
		"cbi  %2,%3""\n\t"
		"sbrc %0,%1""\n\t"
		"sbi  %2,%3""\n\t"
		::"r"(tmp),"M"(G_SEG),"I"(_SFR_IO_ADDR(IO_PORT(G_BIT))),"M"(IO_BIT(G_BIT)));
	//asm volatile (
	//	"sbrs %0,%1""\n\t"
	//	"cbi  %2,%3""\n\t"
	//	"sbrc %0,%1""\n\t"
	//	"sbi  %2,%3""\n\t"
	//	::"r"(tmp),"M"(DP_SEG),"I"(_SFR_IO_ADDR(IO_PORT(DP_BIT))),"M"(IO_BIT(DP_BIT)));
}

////////////////////////////////////////////////
//LEDClear
//Clear the LED display
void LEDclear(void)
{
	//Display off
	IO_SET(A_BIT);
	IO_SET(B_BIT);
	IO_SET(C_BIT);
	IO_SET(D_BIT);
	IO_SET(E_BIT);
	IO_SET(F_BIT);
	IO_SET(G_BIT);
	//	IO_SET(DP_BIT);
}
