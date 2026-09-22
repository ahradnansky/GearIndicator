/*
 * bikes.h
 *
 * Created: 06.06.2014 14:33:21
 * Author: a.hradnansky
 */ 

#ifndef BIKES_H_
#define BIKES_H_

//Number of ECUs known (will be checked on startup)
#define NUM_OF_ECUS							2

extern unsigned char BIKE_DATA[NUM_OF_ECUS][4];

#endif /* BIKES_H_ */