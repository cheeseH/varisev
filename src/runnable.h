/*
 * runnable.h
 *
 *  Created on: 2015年11月19日
 *      Author: cheese
 */
#ifndef RUNNABLE_H_
#define RUNNABLE_H_
#include "varisheader.h"
class Runnable{
public:
	virtual int run()=0;
	virtual ~Runnable(){};
};

#endif

