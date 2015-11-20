#ifndef WORKERPOOL_H_
#define WORKERPOOL_H_

#include "varisheader.h"
#include "runnable.h"
#include <stddef.h>

class WorkerPool{
private:
	class Worker:public Runnable{
	public:
		int run();
		Worker(MultiThreadWork* work):work_(work){}
	private:
		MultiThreadWork* work_;
	};
	ThreadPool* pool_;
public:
	void add(MultiThreadWork*);
	void stop();
	WorkerPool(size_t threadCount,size_t queueSize);
	~WorkerPool();


};


#endif
