/*
 * varisevent.h
 *
 *  Created on: 2015年11月10日
 *      Author: cheese
 */

#ifndef VARISEVENT_H_
#define VARISEVENT_H_
#include "varisheader.h"
//#include "eventmanager.h"
//#include "coroutinemanager.h"
#include <vector>
class VarisEvent{
protected:
	int descriptor_;
	int listenEventType_;
	EventManager* eventManager_;
	WorkerPool* pool_;
	//表示事件发生时触发他的事件类型
	int onEventType_;

	int state_;
	int pority_;
	Coroutine* cor_ ;
public:
	VarisEvent(int fd,int listenEventType);
	virtual ~VarisEvent(){}
	virtual void callback() = 0;
	EventManager* getEventManager();
	void setOnEventType(int type);
	int getListenEventType();
	int getDescriptor();
	void setState(int state);
	void setPority(int pority);
	int getPority();
	void setManager(EventManager*);
	void setListenEventType(int listenEventType);
	void setCoroutine(Coroutine* cor);
	Coroutine* getCoroutine();
	void setPool(WorkerPool* pool);


	int getState();

protected:
	int getOnEventType();

	void runThreadWork(MultiThreadWork* work);
public:
	static void handle(VarisEvent* th);



};

//class VarisCorCtnEvent:public VarisEvent{
//public:
//	VarisCorCtnEvent(int fd,int listenEventType);
//	virtual ~VarisCorCtnEvent();
//	void setCoroutine(Coroutine* coroutine);
//	Coroutine* getCoroutine();
//private:
//	std::vector<Coroutine*> cors_;
//	size_t curCor_;
//};



#endif /* VARISEVENT_H_ */
