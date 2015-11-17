/*
 * varisevent.cpp
 *
 *  Created on: 2015年11月10日
 *      Author: cheese
 */
#include "varisevent.h"
#include "eventmanager.h"
VarisEvent::VarisEvent(int fd,int listenEventType):descriptor_(fd),listenEventType_(listenEventType),eventManager_(NULL),/*pool_(NULL),*/onEventType_(0),state_(OUT),pority_(0),cor_(NULL){};

EventManager* VarisEvent::getEventManager(){
		return eventManager_;
	}
	void  VarisEvent::setOnEventType(int type){
		onEventType_ = type;
	}
	int VarisEvent::getListenEventType(){
		return listenEventType_;
	}
	int VarisEvent::getDescriptor(){
		return descriptor_;
	}
	void VarisEvent::setState(int state){
		switch(state){
			case ACTIVE:
			case SILENCE:
			case OUT:
				state_ = state;
				break;
			default:
				assert(0);
		}
	}
	void VarisEvent::setPority(int pority){
		pority_ = pority;
	}
	int VarisEvent::getPority(){
		return pority_;
	}

	void VarisEvent::setCoroutine(Coroutine* cor){
		cor_ = cor;
	}
	Coroutine*  VarisEvent::getCoroutine(){
		return cor_;
	}

	int VarisEvent::getState(){
		return state_;
	}

	int VarisEvent::getOnEventType(){
		return onEventType_;
	}

	void VarisEvent::setListenEventType(int listenEventType){
		listenEventType_ = listenEventType;
	}

//	void VarisEvent::runThreadWork(MultiThreadWork* work){
//		if(eventManager_->getMultiThread() == 0)
//			assert(0);
//		int evfd = eventfd(0,EFD_NONBLOCK);
//		Event* multi = new CorEvent(evfd,T_READ);
//		multi->setCoroutine(this->cor_);
//		eventManager_->Register(multi);
//		pool_->add(work);
//		cor_->yield();
//
//	}
	void VarisEvent::handle(VarisEvent* th){
		th->callback();
	}
	void VarisEvent::setManager(EventManager* manager){
		this->eventManager_ = manager;
	}

