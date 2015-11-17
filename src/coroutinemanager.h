#ifndef COROUTINEMANAGER_H_
#define COROUTINEMANAGER_H_

#define ROOT 0
#define MAIN 1
#include <ucontext.h>
#include <stack>
#include <stddef.h>
#include "varisheader.h"
#include "varisevent.h"
#define FREE 1
#define BUSY 2
#define READY 3
class Coroutine{
private:
	size_t id_;
	size_t stackSize_;
	char* stack_;
	ucontext_t* context_;
	CoroutineManager* cManager_;
	int status_;
public:
	void yield();
	void work();
	Coroutine(size_t id,size_t stackSize);
	Coroutine();
	void init(size_t id,size_t stackSize,CoroutineManager* cManager);
	void setContext(ucontext_t*);
	size_t getId();
	ucontext_t* getContext();
	int getStatus();
	void setStatus(int status);

};

class CoroutineManager{
	friend class Coroutine;
private:
	const int EPOLL_COROUTINE = 1;
	const int ROOT_COROUTINE = 0;
	std::stack<size_t> freeCoroutine_;
	size_t lastCoroutine_;
	size_t curCoroutine_;
	size_t realSize_;
	Coroutine* coroutines_;
	size_t stackSize_;



public:
	CoroutineManager(size_t workerSize,size_t stackSize);
	~CoroutineManager();
	void yield();
	void newCoroutine();
	void yield(size_t destination);
	void scheduleBack();

	void schedule(size_t workerNumber);
	void runCoroutine(size_t workerNumber);
	bool doWork(VarisEvent* event);

	static void onCoroutineFinished(CoroutineManager*);
};

class CorEvent:public VarisEvent{
public:
	void callback();
	CorEvent(int fd,int listenType,Coroutine* cor);
};

class CorHelper{
public:
	CorHelper(int fd,int listenType,VarisEvent* event,EventManager* manager);
	~CorHelper();
	void yield();
private:
	CorEvent* event_;
	EventManager* evManager_;
	Coroutine* cor_;
};

#endif
