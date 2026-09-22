/*
 * kbus.h
 *
 * Created: 06.06.2014 14:33:21
 * Author: a.hradnansky
 */ 

#ifndef KBUS_H_
#define KBUS_H_

//KBUS STATE
#define KBUS_IDDLE			0
#define KBUS_COMM_STARTED	1
#define KBUS_DIAG_STARTED	2
#define KBUS_DATA_POOLING	3

extern unsigned char KBUS_START_COMM[5];	// START COMMUNICATION SESSION COMMAND
extern unsigned char KBUS_START_DIAG[7];	// START DIAGNOSTIC MODE COMMAND
extern unsigned char KBUS_READ_REG[7];		// READ DATA REGISTER

//Prototypes
unsigned char KBUS_checksum(unsigned char * msg, unsigned char cnt);
void KBUS_fast_init(unsigned char ECU_addr);

#endif /* KBUS_H_ */