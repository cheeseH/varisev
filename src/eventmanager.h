/*
 * eventmanager.h
 *
 *  Created on: 2015年11月10日
 *      Author: cheese
 */

#ifndef EVENTMANAGER_H_
#define EVENTMANAGER_H_
#include <stddef.h>
#include <sys/epoll.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "varisevent.h"
//#include "looper.h"
//#include "varisheader.h"
//#include "timermanager.h"
typedef int EventOperation;

#define T_READ 1
#define T_WRITE 2
#define T_SIGNAL 4
#define T_TIME 8
#define T_CONTINUE 16
#define T_THREAD 32
#define T_COROUTINE 64

#define ACTIVE 1
#define SILENCE 2
#define OUT 4
#define HANDLING 8

struct InnerActiveEvent{
	VarisEvent* event_;
	InnerActiveEvent* next_;
	InnerActiveEvent* last_;
	InnerActiveEvent():event_(NULL),next_(NULL),last_(NULL){};
};

struct  ActiveHead
{
	InnerActiveEvent* first_;
	InnerActiveEvent* last_;
	ActiveHead():first_(NULL),last_(NULL){};
};

//class MultiEvent:public Event{
//	void callback(){
//		uint64_t count;
//		int ret = read(descriptor_,&count,sizeof(count));
//		if(ret<0)
//			assert(0);
//		else
//			cor_->work();
//	}
//};
class EventManager{
public:
	struct InnerEventOperation{
			VarisEvent* ev_;
			EventOperation operation_;
			int ltime_;

		};
private:
	int epfd_;
	size_t maxEvent_;
	//默认为0
	int minPority_;
	//默认为9
	int maxPority_;

	int multiThread_;
	TimerManager* timeEvents_;
	Looper* looper_;
	epoll_event* events_;
	WorkerPool* pool_;
public:
	EventManager();
	void Register(VarisEvent* event,long ltime = -1);
	void Release(VarisEvent* event);
	void Silence(VarisEvent* event);
	void Active(VarisEvent* event,long ltime = -1);
	int getMultiThread();
	void errorHandle(epoll_event* event);
	void Start();
	void setLooper(Looper* looper);
	void setPool(WorkerPool* pool);
};




#endif /* EVENTMANAGER_H_ */
