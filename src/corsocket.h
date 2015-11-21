/*
 * corsocket.h
 *
 *  Created on: 2015年11月16日
 *      Author: cheese
 */

#ifndef CORSOCKET_H_
#define CORSOCKET_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include "varisheader.h"
//class CorEvent : public VarisEvent{
//public:
//	void callback();
//	CorEvent(int fd,int listenEventType,CorSocket* parent,void* buf,size_t len,int* res);
//private:
//	CorSocket* parent_;
//	void* buf_;
//	size_t len_;
//	int* res_;
//};

class CorSocket{
private:
	EventManager* evManager_;
	int sockfd_;
	int error_;
	VarisEvent* parent_;
	friend class CorEvent;
public:
	CorSocket(int nonblockfd,EventManager* evManager,VarisEvent* event);
	int Recv(void* buf,size_t len);
	int Send(void* buf,size_t len);
	virtual ~CorSocket();
private:
	int InnerSend(void* buf,size_t len);
};

#endif
