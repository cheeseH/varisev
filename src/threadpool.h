/*
 * threadpool.h
 *
 *  Created on: 2015年11月19日
 *      Author: cheese
 */

/*
 * ThreadPool.h
 *
 *  Created on: 2015年10月29日
 *      Author: cheese
 */


#ifndef THREADPOOL_H_
#define THREADPOOL_H_
#include "varisheader.h"
#include "thread.h"
#include "concurrentqueue.h"
#include <stddef.h>
class ThreadPool{
private:
	ConcurrentQueue<Runnable*>* workQueue_;
	size_t threadCount_;
	size_t queueSize_;
	class WorkerThread:public Thread{
	private:
		std::atomic<bool> toStop_;
		ConcurrentQueue<Runnable*>*  workQueue_;
	public:
		int run();
		void close();
		void setQueue(ConcurrentQueue<Runnable*>*);
		WorkerThread(ConcurrentQueue<Runnable*>* workQueues);
		WorkerThread();
	};
public:
	ThreadPool(size_t threadCount,size_t queueSize);
	~ThreadPool();
	WorkerThread* workThreads_;
	void execute(Runnable* target);
	void stop();

};

#endif

