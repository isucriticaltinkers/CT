/*
 * Piezo.h
 *
 * Created: 10/30/2016 4:17:15 PM
 *  Author: Administrator
 */ 


#ifndef PIEZO_H_
#define PIEZO_H_

#include <stdint.h>

typedef void(*PiezoHandler)();


void Piezo_init(uint16_t, PiezoHandler);



#endif /* PIEZO_H_ */