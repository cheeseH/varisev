/*
 * thread.h
 *
 *  Created on: 2015年11月19日
 *      Author: cheese
 */

#ifndef THREAD_H_
#define THREAD_H_

#include "varisheader.h"
#include "runnable.h"
#include <pthread.h>
#include <atomic>
class Thread:public Runnable{
public:
	Thread();
	Thread(Runnable* target);
	virtual ~Thread();
	virtual int run();
	int start();
	static void* Tdo(void* thread);
	int join();
private:
	pthread_t id_;
	Runnable* target_;
protected:
	std::atomic<bool> running_;
};

#endif
