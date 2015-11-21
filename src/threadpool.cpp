/*
 * threadpool.cpp
 *
 *  Created on: 2015年11月19日
 *      Author: cheese
 */


#include "threadpool.h"
ThreadPool::WorkerThread::WorkerThread(ConcurrentQueue<Runnable*>* workQueues):toStop_(false),workQueue_(workQueues){};
ThreadPool::WorkerThread::WorkerThread():toStop_(false),workQueue_(NULL){};
void ThreadPool::WorkerThread::setQueue(ConcurrentQueue<Runnable*>* workQueues){
	workQueue_ = workQueues;
}

int ThreadPool::WorkerThread::run(){
	int ret = 1;
	running_ = true;
	while(!toStop_){
		Runnable* target;
		workQueue_->pop(target);
		if((ret = target->run() )!= 1){

			toStop_ = true;

		};
		delete target;

	}
	running_ = false;
	return ret;

}

void ThreadPool::WorkerThread::close(){
	toStop_ = true;
}

ThreadPool::ThreadPool(size_t threadCount,size_t queueSize){
	threadCount_ = threadCount;
	queueSize_ = queueSize;
	workQueue_ = new ConcurrentQueue<Runnable*>(queueSize);
	workThreads_ = new WorkerThread[threadCount];
	while(threadCount--){
		workThreads_[threadCount].setQueue(workQueue_);
		workThreads_[threadCount].start();
	}
}

void ThreadPool::execute(Runnable* target){
	workQueue_->push(target);
}

void ThreadPool::stop(){
	for(size_t i = 0;i<threadCount_;i++){
		workThreads_[i].close();
	}
}




