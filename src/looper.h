/*
 * looper.h
 *
 *  Created on: 2015年11月10日
 *      Author: cheese
 */

#ifndef LOOPER_H_
#define LOOPER_H_
#include "varisheader.h"
//#include "varisevent.h"
//#include "eventmanager.h"
//#include "coroutinemanager.h"
#include <stdlib.h>
//#include <stddef.h>
class Looper{

public:

	Looper();
	virtual ~Looper();
	void loop();
	void addToActiviList(VarisEvent* event,int onEventType);
protected:
	ActiveHead* activeList_;
private:
	//默认为9
	int maxPority_;
	virtual void forEachPority(int pority);
};

class CorLooper:public Looper{
private:
	CoroutineManager* corManager_;
public:
	CorLooper(CoroutineManager* corManager);
private:
	void forEachPority(int pority);
};

#endif /* LOOPER_H_ */
