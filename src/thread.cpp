/*
 * thread.cpp
 *
 *  Created on: 2015年11月19日
 *      Author: cheese
 */

#include "thread.h"
#include <stddef.h>
Thread::Thread(){

}

Thread::Thread(Runnable* target):target_(target){
}

Thread::~Thread(){

}

int Thread::run(){
	int ret;
	running_ = true;
	if(target_ != NULL)
		ret = target_->run();
	else
		ret = 0;
	running_ = false;
	return ret;
}
int Thread::start(){
	int ret = pthread_create(&id_,NULL,Thread::Tdo,this);
	if(ret != 0)
		return -1;
	else
		return 1;
}

void* Thread::Tdo(void* thread){
	Thread* thd = (Thread*)thread;
	int ret = thd->run();
	return (void*)ret;
}

int Thread::join(){
	void* rs;
	return pthread_join(id_,&rs) == 0?1:-1;
}



