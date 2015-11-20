/*
 * epollmanager.cpp
 *
 *  Created on: 2015年11月10日
 *      Author: cheese
 */
#include "eventmanager.h"
#include "timermanager.h"
#include "looper.h"
#include <iostream>
EventManager::EventManager(){
	//give it an constant, maybe change in the follow version
	maxEvent_ = 1000;
	epfd_ = epoll_create(maxEvent_);
	minPority_ = 0;
	maxPority_ = 9;
	multiThread_ = 0;
	timeEvents_ = new TimerManager();
	looper_ =NULL;
	events_ = new epoll_event[maxEvent_];

}

/*
	notice : 传入的event需要在堆上创建,调用release释放内存;
*/
void EventManager::setLooper(Looper* looper){
	looper_ = looper;
}
void EventManager::Register(VarisEvent* event,long ltime){
	int eventnum = event->getDescriptor();
	int eventType = event->getListenEventType();
	event->setState(ACTIVE);
	event->setManager(this);
	event->setPool(pool_);
	if(eventnum < 0 && !(eventType&T_TIME)){
		//错误处理
		assert(0);
	}
	if(eventType&T_TIME){
		if(ltime == -1){
			assert(0);
		}
		timeEvents_->push(event,ltime);
		return;
	}
	if(eventType & T_SIGNAL){
		//以后再说
	}
	epoll_event ev;
	ev.data.ptr = event;
	if(eventType & T_READ){
		ev.events|=EPOLLIN;
		ev.events^=EPOLLWRNORM;
	}
	if(eventType & T_WRITE){
		ev.events|=EPOLLOUT;
		ev.events^=EPOLLRDNORM;
	}
	if(!(eventType & T_CONTINUE)){
		ev.events|=EPOLLONESHOT;
	}
	if(epoll_ctl(epfd_,EPOLL_CTL_ADD,eventnum,&ev)<0){
		perror(strerror(errno));
	}

}


void EventManager::Release(VarisEvent* event){
	int eventnum = event->getDescriptor();
	int eventType = event->getListenEventType();
	int state = event->getState();
	if(state == HANDLING){
		event->setState(OUT);
	}
	if(state != ACTIVE && state!=SILENCE){
		assert(0);
	}
	if(state == SILENCE){
		if(eventType & T_SIGNAL){

		}
		if( (eventType  & T_READ)||(eventType & T_WRITE) ){
			close(eventnum);
		}
	}
	if(state == ACTIVE){
		if(eventType & T_SIGNAL){

		}
		if( (eventType  & T_READ)||(eventType & T_WRITE) ){
			epoll_event ev;
			epoll_ctl(epfd_,EPOLL_CTL_DEL,eventnum,&ev);

		}

	}
	delete event;
}

int EventManager::getMultiThread(){
	return multiThread_;
}

void EventManager::Silence(VarisEvent* event){
	//simple
	int eventnum = event->getDescriptor();
	int eventType = event->getListenEventType();
	int state = event->getState();
	if(state != ACTIVE){
		//错误处理
		assert(0);
	}
//	if(eventType&T_TIME){
//		timeEvents_->remove(event);
//	}
	if(eventType & T_SIGNAL){
		//以后再说
		return;
	}
	if(eventType& T_TIME){
		event->setState(SILENCE);
		return;
	}
	epoll_event ev;
	epoll_ctl(epfd_,EPOLL_CTL_DEL,eventnum,&ev);
	event->setState(SILENCE);

}


void EventManager::Active(VarisEvent* event,long ltime){
	int eventnum = event->getDescriptor();
	int eventType = event->getListenEventType();
	int state = event->getState();
	if(state != SILENCE){
		assert(0);
	}
	Register(event,ltime);
}


void EventManager::errorHandle(epoll_event* event){

}




void EventManager::Start(){

	while(true){
		//处理定时器和超时事件
		VarisEvent* event;
		while((event = timeEvents_->pop())!=NULL){
			int state = event->getState();
			switch(state){
				case ACTIVE:
					looper_->addToActiviList(event,T_TIME);
					break;
				case SILENCE:
					break;
				case OUT:
					delete event;
					break;

			}
		}
		int fds = epoll_wait(epfd_,events_,maxEvent_,0);
		if(fds == -1 && errno == EINTR)
			continue;

		else if(fds == -1){
			assert(0);
		}
		for(int i = 0;i<fds;i++){
			epoll_event epollev = events_[i];
			VarisEvent* vevent = (VarisEvent*)(epollev.data.ptr);
			uint32_t epollevent = epollev.events;
			if(epollevent&(EPOLLRDHUP | EPOLLERR | EPOLLHUP)){
				errorHandle(&epollev);
			}
			else{
				if(epollevent & EPOLLIN){
					if((vevent->getListenEventType()&T_COROUTINE)&&(vevent->getListenEventType()&T_CONTINUE)){

					}
					looper_->addToActiviList(vevent,T_READ);
				}
				else if(epollevent & EPOLLOUT){
					looper_->addToActiviList(vevent,T_WRITE);
				}
			}
		}
		looper_->loop();
		timeEvents_->pushWaiting();
	}
}

void EventManager::setPool(WorkerPool* pool){
	pool_ = pool;
}


