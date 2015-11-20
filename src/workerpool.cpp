/*
 * workerpool.cpp
 *
 *  Created on: 2015年11月19日
 *      Author: cheese
 */

#include "workerpool.h"
#include <unistd.h>
#include "multithreadwork.h"
#include "threadpool.h"
int WorkerPool::Worker::run(){
	work_->work();
	uint64_t u;
	u = 1;
	int fd = work_->getFd();
	write(fd,&u,sizeof(uint64_t));
	return 1;
}

WorkerPool::WorkerPool(size_t threadCount,size_t queueSize){
	pool_ = new ThreadPool(threadCount,queueSize);
}

void WorkerPool::add(MultiThreadWork* _work){
	Worker* work = new Worker(_work);
	pool_->execute(work);
}



