/*
 * eeproma.h
 * Part of project: GearIndicator
 * Created: 29. 5. 2015
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

#ifndef EEPROMA_H
#define EEPROMA_H

//Functions prototypes

//Reads data from EEPROM a stores them to lastStatus
void EEPROMread(void);

//Writes data from lastStatus to EEPROM
void EEPROMwrite(void);

//Read/Writes data related to RPM to EEPROM
void EEPROM_rpm_read(void);
void EEPROM_rpm_write(void);

//Structure to be stored in EEPROM as a last successful connection to ECU
//Indicator starts with these parameters
typedef struct
{
	unsigned char lastECU;			//last successfully connected ECU index to BIKE_DATA field of structures bikedata (bikes.h)
	unsigned char initcheck;		//check or not replies to COMM_INIT and DIAG_INIT commands (0/1)
} indstatus;

//Structure to store RPM
typedef struct  
{
	unsigned int  rpm;
	unsigned char prgmode;
}rpmstatus;

extern rpmstatus rpmValue;			//minimal RPM when display starts blinking + prog mode flag

extern indstatus lastStatus;		//lastStatus read from EEPROM / writen to EEPROM

#endif /* EEPROMA_H_ */