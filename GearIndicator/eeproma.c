/*
 * eeproma.c
 * Part of project: GearIndicator
 * Created: 29. 5. 2015 12:06:53
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
#include <avr/eeprom.h>
#include "eeproma.h"

//last used address
indstatus lastStatus;
rpmstatus rpmValue;
indstatus EEMEM e_lastStatus = {.lastECU=0, .initcheck=YES};	//EEPROM init values for status: start from ECUno 0, initcheck yes
rpmstatus EEMEM e_rpmValue = {.prgmode=0xFF, .rpm=0};			//EEPROM init values for rpm: prg mode off, rpm not set

//Reads data from EEPROM
void EEPROMread(void)
{
	eeprom_read_block((void*)&lastStatus, (const void*)&e_lastStatus, sizeof(lastStatus));
}

//Writes data to EEPROM
void EEPROMwrite(void)
{
	eeprom_update_block((const void*)&lastStatus,(void *)&e_lastStatus, sizeof(lastStatus));
}

//Reads RPM from EEPROM
void EEPROM_rpm_read(void)
{
	eeprom_read_block((void*)&rpmValue, (const void*)&e_rpmValue, sizeof(rpmValue));
}

//Writes RPM to EEPROM
void EEPROM_rpm_write(void)
{
	eeprom_update_block((const void*)&rpmValue,(void *)&e_rpmValue, sizeof(rpmValue));
}
