/*****************************************************************************/
/**
 *	@file smallprintf.h
 *
 *	@created 25. 7. 2015
 *	@author Mica (michalhrouda@gmail.com)
 *
 *	@licence CC
 *
 *****************************************************************************/

#ifndef SMALLPRINTF_H_
#define SMALLPRINTF_H_

#include <stdint.h>

uint16_t smallprintf(void (*small_putchar)(char), char * str, ...);


#endif /* SMALLPRINTF_H_ */
