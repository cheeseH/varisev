#ifndef MULTITHREADWORK_H_
#define MULTITHREADWORK_H_

#include <sys/eventfd.h>
#include "varisheader.h"
class MultiThreadWork{
private:
	int eventfd_;
public:
	MultiThreadWork(){
//		eventfd_ = eventfd(0,EFD_NONBLOCK);
	}
	int getFd(){
		return eventfd_;
	}
	void setFd(int eventfd){
		eventfd_ = eventfd;
	}
	void virtual work()=0;
	virtual ~MultiThreadWork(){};
};

#endif
