/*
 * led.h
 * Part of project: GearIndicator
 * Created: 6/2014
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

#ifndef LED_H_
#define LED_H_

#include "global.h"

//************************************************************
//Begin
//Display HW connections 
//Physical connection of the segments to ports & bits
//f(PORT,BIT)

#define A_BIT(f)			f(D,4)
#define B_BIT(f)			f(D,2)
#define C_BIT(f)			f(C,1)
#define D_BIT(f)			f(B,2)
#define E_BIT(f)			f(D,7)
#define F_BIT(f)			f(D,5)

//*normalne osadena 7 segmentovka
#define G_BIT(f)			f(D,6)
#define DP_BIT(f)			f(C,2)

/*opacne osadena 7 segmentovka (otocena o 180°)
#define G_BIT(f)			f(C,2)
#define DP_BIT(f)			f(D,6)
*/

//End of
//Display HW connection
//************************************************************

extern char volatile displayedchar;
extern char volatile f_dispblink;

//functions prototypes	
void LEDinit(void);										//Initializes display
void LEDclear(void);									//Clears display
void LEDoutch(unsigned char ch);						//Outputs char to display

#endif /* LED_H_ */