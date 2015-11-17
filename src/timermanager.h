/*
 * timermanager.h
 *
 *  Created on: 2015年11月13日
 *      Author: cheese
 */

#ifndef TIMERMANAGER_H_
#define TIMERMANAGER_H_

#include <vector>
#include <stddef.h>
#include <algorithm>
#include <sys/time.h>
#include "varisheader.h"
//#include "varisevent.h"

class TimerManager{
public:
	class InnerTimeEvent{
	private:
		unsigned long deadline_;
		unsigned long ltime_;
		VarisEvent* event_;
	public:
		bool operator<(const InnerTimeEvent& eventB);
		InnerTimeEvent(unsigned long deadline,unsigned long ltime,VarisEvent* event );
		~InnerTimeEvent();
		VarisEvent* getEvent();
		unsigned long getltime();
		unsigned long getDeadline();
	};
	TimerManager();
	~TimerManager();
	void push(VarisEvent* event,unsigned long ltime);
	VarisEvent* pop();
	void pushWaiting();
private:
	std::vector<InnerTimeEvent*> events;
	InnerTimeEvent* latest_;
	std::vector<InnerTimeEvent*> wait_events;

	void waiting_push(VarisEvent* event,unsigned long ltime);



};



#endif /* TIMERMANAGER_H_ */
