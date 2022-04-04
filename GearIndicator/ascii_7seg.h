/*
 * ascii_7seg.h
 * Part of project: GearIndicator
 * Created: 5. 12. 2013 9:40:20
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
#ifndef ASCII_7SEG_H_
#define ASCII_7SEG_H_

#include <avr/pgmspace.h>

//LED Segments order inside the byte
#define A_SEG			0
#define B_SEG			1
#define C_SEG			2
#define D_SEG			3
#define E_SEG			4
#define F_SEG			5
#define G_SEG			6
#define DP_SEG			7

//global constant stored in PROGMEM
const unsigned char ASCII_TBL_7SEG[] PROGMEM = {					// ASCII to SEVEN-SEGMENT conversion table
	1<<A_SEG|1<<B_SEG|1<<C_SEG|1<<D_SEG|1<<E_SEG|1<<F_SEG,			// '0' 0
	1<<B_SEG|1<<C_SEG,												// '1' 1
	1<<A_SEG|1<<B_SEG|1<<D_SEG|1<<E_SEG|1<<G_SEG,					// '2' 2
	1<<A_SEG|1<<B_SEG|1<<C_SEG|1<<D_SEG|1<<G_SEG,					// '3' 3
	1<<B_SEG|1<<C_SEG|1<<F_SEG|1<<G_SEG,							// '4' 4
	1<<A_SEG|1<<C_SEG|1<<D_SEG|1<<F_SEG|1<<G_SEG,					// '5' 5
	1<<A_SEG|1<<C_SEG|1<<D_SEG|1<<E_SEG|1<<F_SEG|1<<G_SEG,			// '6' 6
	1<<A_SEG|1<<B_SEG|1<<C_SEG,										// '7' 7
	1<<A_SEG|1<<B_SEG|1<<C_SEG|1<<D_SEG|1<<E_SEG|1<<F_SEG|1<<G_SEG,	// '8' 8
	1<<A_SEG|1<<B_SEG|1<<C_SEG|1<<D_SEG|1<<F_SEG|1<<G_SEG,			// '9' 9
	1<<G_SEG,														// '-' 10 
	1<<A_SEG|1<<B_SEG|1<<C_SEG|1<<E_SEG|1<<F_SEG|1<<G_SEG,			// 'A' 11
	1<<B_SEG|1<<C_SEG|1<<E_SEG|1<<F_SEG|1<<G_SEG,					// 'H' 12
	1<<A_SEG|1<<B_SEG|1<<C_SEG|1<<D_SEG|1<<E_SEG|1<<F_SEG,			// 'O' 13
	1<<A_SEG|1<<B_SEG|1<<C_SEG|1<<D_SEG|1<<E_SEG,					// 'J' 14
	1<<B_SEG|1<<C_SEG|1<<E_SEG|1<<F_SEG|1<<G_SEG,					// 'H' 15
	1<<A_SEG|1<<D_SEG|1<<E_SEG|1<<F_SEG|1<<G_SEG,					// 'E' 16
	1<<D_SEG|1<<E_SEG|1<<F_SEG,										// 'L' 17
	1<<D_SEG|1<<E_SEG|1<<F_SEG,										// 'L' 18
	1<<A_SEG|1<<B_SEG|1<<C_SEG|1<<D_SEG|1<<E_SEG|1<<F_SEG,			// 'O' 19
	1<<A_SEG,														// SEG_A_only 20
	1<<B_SEG,														// SEG_B_only 21
	1<<C_SEG,														// SEG_C_only 22 
	1<<D_SEG,														// SEG_D_only 23
	1<<E_SEG,														// SEG_E_only 24
	1<<F_SEG,														// SEG_F_only 25
	1<<G_SEG,														// SEG_G_only 26
	1<<D_SEG|1<<E_SEG|1<<F_SEG,										// L		  27
	1<<A_SEG|1<<D_SEG|1<<E_SEG|1<<F_SEG|1<<G_SEG,					// E		  28
	1<<D_SEG|1<<E_SEG|1<<F_SEG|1<<G_SEG,							// t		  29
	1<<A_SEG|1<<C_SEG|1<<D_SEG|1<<F_SEG|1<<G_SEG,					// S		  30 
	0,																// (space)	  31
	1<<E_SEG|1<<G_SEG,												// r		  32
	1<<E_SEG,														// i		  33
	1<<B_SEG|1<<C_SEG|1<<D_SEG|1<<E_SEG|1<<G_SEG,					// d		  34	
	1<<A_SEG|1<<D_SEG|1<<E_SEG|1<<F_SEG|1<<G_SEG,					// E		  35	
	1<<A_SEG|1<<B_SEG|1<<E_SEG|1<<F_SEG|1<<G_SEG					// 'P'  	  36
};
#endif /* ASCII_7SEG_H_ */