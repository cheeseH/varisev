/*
 * looper.cpp
 *
 *  Created on: 2015年11月10日
 *      Author: cheese
 */

#include "looper.h"
#include "coroutinemanager.h"
#include "eventmanager.h"
Looper::Looper(){
	maxPority_ = 9;
	activeList_ = new ActiveHead[maxPority_+1];
}

Looper::~Looper(){

}
void Looper::forEachPority(int pority){
	ActiveHead* head = &activeList_[pority];
	InnerActiveEvent* cur = head->first_;
	while(cur != NULL){
		VarisEvent* curev = cur->event_;
		curev->callback();
		if(curev->getState() == OUT || (curev->getListenEventType()&T_CONTINUE))
			delete curev;

		InnerActiveEvent* tmp = cur;
		cur = cur->next_;
		head->first_ = cur;
		free(tmp);
	}
	head->first_ = NULL;
	head->last_ = NULL;
}

void Looper::addToActiviList(VarisEvent* event,int onEventType){
//	event->setOnEventType(onEventType);
//	int pority = event->getPority();
//	InnerActiveEvent* innEvent = NULL;
//	for(innEvent = activeList_[pority].first_;innEvent->next_ != NULL;innEvent = innEvent->next_){
//	}
//	InnerActiveEvent* newEvent = new InnerActiveEvent();
//	newEvent->event_ = event;
//	newEvent->next_ = NULL;
//	newEvent->last_ = innEvent;
//	innEvent->next_ = newEvent;
	event->setOnEventType(onEventType);
		int pority = event->getPority();
		InnerActiveEvent* oldLast = activeList_[pority].last_;
		InnerActiveEvent* inner = (InnerActiveEvent*)malloc(sizeof(InnerActiveEvent));
		inner->event_ = event;
		inner->last_ = oldLast;
		inner->next_ = NULL;
		if(oldLast == NULL){
			ActiveHead* head = &activeList_[pority];
			head->first_ = inner;
			head->last_ = inner;
		}
		else{
			oldLast->next_ = inner;
			activeList_[pority].last_ = inner;
		}
}

void Looper::loop(){
	for(int i = maxPority_;i>=0;i--){
		forEachPority(i);
	}
}


void CorLooper::forEachPority(int pority){
		ActiveHead* head = &activeList_[pority];
		InnerActiveEvent* cur = head->first_;
		while(cur != NULL){
			VarisEvent* curev = cur->event_;
			corManager_->doWork(curev);
			if((curev->getState() == OUT||!(curev->getListenEventType()&T_CONTINUE)) && curev->getCoroutine()->getStatus() == FREE)
				delete curev;

			InnerActiveEvent* tmp = cur;
			cur = cur->next_;
			head->first_ = cur;
			free(tmp);
		}
		head->first_ = NULL;
		head->last_ = NULL;
}


CorLooper::CorLooper(CoroutineManager* corManager):corManager_(corManager){};


