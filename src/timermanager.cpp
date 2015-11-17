/*
 * TimerManager.cpp
 *
 *  Created on: 2015年11月13日
 *      Author: cheese
 */

#ifndef TimerManager_CPP_
#define TimerManager_CPP_

#include "timermanager.h"
#include <stddef.h>
#include "eventmanager.h"




TimerManager::InnerTimeEvent::InnerTimeEvent(unsigned long deadline,unsigned long ltime,VarisEvent* event):deadline_(deadline),ltime_(ltime),event_(event){}

TimerManager::InnerTimeEvent::~InnerTimeEvent(){};

VarisEvent* TimerManager::InnerTimeEvent::getEvent(){
	return event_;
}

unsigned long TimerManager::InnerTimeEvent::getDeadline(){
	return deadline_;
}
unsigned long TimerManager::InnerTimeEvent::getltime(){
	return ltime_;
}


TimerManager::TimerManager(){
	latest_ = NULL;
	events = std::vector<InnerTimeEvent*>();
	std::make_heap(events.begin(),events.end());
}

void TimerManager::push(VarisEvent* event,unsigned long msec){
	timeval curTime;
	gettimeofday(&curTime,NULL);
	unsigned long start = curTime.tv_sec*1000+curTime.tv_usec/1000;
	unsigned long deadline = start+msec;
	InnerTimeEvent* timeevent = new InnerTimeEvent(deadline,msec,event);
	events.push_back(timeevent);
	push_heap(events.begin(),events.end());
}

void TimerManager::waiting_push(VarisEvent* event,unsigned long ltime){
	InnerTimeEvent* timeevent = new InnerTimeEvent(0,ltime,event);
	wait_events.push_back(timeevent);
}

void TimerManager::pushWaiting(){
	while(wait_events.size() != 0){
		InnerTimeEvent* event = wait_events.back();
		push(event->getEvent(),event->getltime());
		wait_events.pop_back();
		delete event;
	}
}

VarisEvent* TimerManager::pop(){
	timeval curTime;
	VarisEvent* resev = NULL;
	InnerTimeEvent* resinev = NULL;
	gettimeofday(&curTime,NULL);
	unsigned long now = curTime.tv_sec*1000+curTime.tv_usec/1000;
	//compare the lastes_
	if(latest_ !=NULL){
		if(now>=latest_->getDeadline()){
			resinev = latest_;
			latest_ = NULL;
		}
		else
			resinev = NULL;
	}
	else{
		if(events.size() == 0)
			return NULL;
		pop_heap(events.begin(), events.end());
		InnerTimeEvent* event = events.back();
    	events.pop_back();
    	if(now>=event->getDeadline())
    		resinev = event;
    	else{
    		latest_ = event;
    		return NULL;
    	}
	}
	if(resinev == NULL)
		return NULL;
	resev = resinev->getEvent();
//	if(resinev->getEvent()->getListenEventType()&T_CONTINUE){
//		this->push(resinev->getEvent(),resinev->getltime());
//	}
	if(resev->getState()!=OUT && resev->getState()!=SILENCE && (resev->getListenEventType()&T_CONTINUE)){
		waiting_push(resev,resinev->getltime());
	}

	delete resinev;
	return resev;
}




#endif /* TimerManager_CPP_ */
