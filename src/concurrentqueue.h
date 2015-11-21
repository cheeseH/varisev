/*
 * concurrentqueue.h
 *
 *  Created on: 2015年11月19日
 *      Author: cheese
 */

#ifndef CONCURRENTQUEUE_H_
#define CONCURRENTQUEUE_H_


#include "varisheader.h"
#include <condition_variable>
#include <atomic>
#include <mutex>
#include <thread>
#include <iostream>

template <typename T>
class ConcurrentQueue
{
	T* buffer;
	size_t qmod;
	std::atomic<size_t> head, tail, claimg, claimp;
	std::condition_variable waitForEpt, waitForFll;
	std::mutex emtx, fmtx;
	std::atomic<size_t> popWaitCount, pushWaitCount;
private:
	size_t i2p(size_t i) const;
public:
	ConcurrentQueue(size_t minSize);
	~ConcurrentQueue();

	void pop(T& data);
	void push(const T& data);

	size_t getCapacity() const;
	bool isEmpty() const;
	bool isFull() const;
};

template <typename T>
inline size_t ConcurrentQueue<T>::i2p(size_t i) const
{
	return i & qmod;
}

template <typename T>
ConcurrentQueue<T>::ConcurrentQueue(size_t minSize)
	: head(0), tail(0), claimg(0), claimp(0), popWaitCount(0), pushWaitCount(0)
{
	size_t capacity = 1;
	while(minSize != 0)
	{
		capacity <<= 1;
		minSize >>= 1;
	}

	buffer = new T[capacity];
	qmod = capacity - 1;
}

template <typename T>
ConcurrentQueue<T>::~ConcurrentQueue()
{
	delete[] buffer;
}

template <typename T>
void ConcurrentQueue<T>::pop(T& data)
{
	size_t cg, tn;
	do
	{
		++popWaitCount;
		while((cg = claimg.load()) == (tn = i2p(tail.load())))
		{
			std::unique_lock<std::mutex> lck(emtx);
			waitForEpt.wait(lck);
		}
		--popWaitCount;
	} while(!claimg.compare_exchange_weak(cg, i2p(cg + 1)));
	std::cout<<"after pop:"<<claimg.load()<<std::endl;
	data = buffer[cg];

	size_t oh = cg, nh = i2p(cg + 1);
	while(!head.compare_exchange_weak(oh, nh))
	{
		std::this_thread::yield();
		oh = cg;
	}

	size_t odwc = pushWaitCount.load();
	while(odwc > 0 && odwc <= pushWaitCount.load() && !isEmpty())
	{
		odwc = pushWaitCount.load();
		waitForFll.notify_all();
		std::this_thread::yield();
	}
}

template <typename T>
void ConcurrentQueue<T>::push(const T& data)
{
	size_t cp, h;
	do
	{
		++pushWaitCount;
		while(i2p((cp = claimp.load()) + 1) == (h = head.load()))
		{
				std::cout<<"dead1"<<std::endl;
				std::unique_lock<std::mutex> lck(fmtx);
				waitForFll.wait(lck);
		}
		--pushWaitCount;
	} while(!claimp.compare_exchange_weak(cp, i2p(cp + 1)));

	buffer[cp] = data;

	size_t ot = cp, nt = i2p(cp + 1);
	while(!tail.compare_exchange_weak(ot, nt))
	{
		std::this_thread::yield();
		ot = cp;
	}

	size_t odwc = popWaitCount.load();
	if(odwc > 0 && odwc <= popWaitCount.load() && !isFull())
	{
		odwc = popWaitCount.load();
		waitForEpt.notify_all();
		std::this_thread::yield();
	}
}

template <typename T>
inline size_t ConcurrentQueue<T>::getCapacity() const
{
	return qmod + 1;
}

template <typename T>
inline bool ConcurrentQueue<T>::isEmpty() const
{
	return head.load() == tail.load();
}

template <typename T>
inline bool ConcurrentQueue<T>::isFull() const
{
	return head.load() == i2p(tail.load() + 1);
}



#endif
