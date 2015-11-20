/*
 * corsocket.cpp
 *
 *  Created on: 2015年11月16日
 *      Author: cheese
 */


#include "corsocket.h"
#include "eventmanager.h"
#include "coroutinemanager.h"
//void CorEvent::callback(){
//	parent_->setCoroutine(cor_);
//	int res;
//	switch(onEventType_){
//	case T_READ:
//		res = parent_->InnerRecv(buf_,len_);
//		break;
//	case T_WRITE:
//		res = parent_->InnerSend(buf_,len_);
//		break;
//	default:
//		break;
//	}
//
//}
//
//CorEvent::CorEvent(int fd,int listenEventType,CorSocket* parent,void* buf,size_t len,int* res):VarisEvent(fd,listenEventType),parent_(parent),buf_(buf),len_(len),res_(res){
//	listenEventType_|= T_COROUTINE;
//	cor_ = parent_->cor_;
//}
CorSocket::CorSocket(int nonblockfd,EventManager* evmanager,VarisEvent* event):sockfd_(nonblockfd),evManager_(evmanager),parent_(event){

}

CorSocket::~CorSocket(){

}

int CorSocket::Recv(void* buf,size_t len){
	CorHelper*  helper = new CorHelper(sockfd_,T_READ,parent_,evManager_);
	size_t received = 0;
			while(received < len){
				int ret = read(sockfd_,buf+received,len-received);
				//客户端关闭了网络链接
				if(ret == 0){
					delete helper;
					return received;
				}
				else if(ret == -1){
					if(errno == EAGAIN || errno == EWOULDBLOCK){
						helper->yield();
					}
					else if(errno == EINTR){
						continue;
					}
					else{
						error_ = errno;
						delete helper;
						return ret;
					}
				}
				else
					received+=ret;
			}
			//remember to release the event
			perror("herhe");
			delete helper;
			return received;
}

//int CorSocket::InnerRecv(void* buf,size_t len){
//		int received = 0;
//		while(received < len){
//			int ret = recv(sockfd_,buf+received,len,MSG_DONTWAIT);
//			//客户端关闭了网络链接
//			if(ret == 0){
//				return received;
//			}
//			else if(ret == -1){
//				if(errno == EAGAIN || errno == EWOULDBLOCK){
//					cor_->yield();
//				}
//				else if(errno == EINTR){
//					continue;
//				}
//				else{
//					error_ = errno;
//					return ret;
//				}
//			}
//			else
//				received+=ret;
//		}
//		//remember to release the event
//		return received;
//}

int CorSocket::InnerSend(void* buf,size_t len){

}

int CorSocket::Send(void* buf,size_t len){
	CorHelper* helper = new CorHelper(sockfd_,T_READ,parent_,evManager_);
	int sended = 0;
	while(sended < len){
		int ret = write(sockfd_,buf+sended,len-sended);
		//客户端关闭了网络链接
		if(ret == 0){
			delete helper;
			return sended;
		}
		else if(ret == -1){
			if(errno == EAGAIN || errno == EWOULDBLOCK){
				helper->yield();
			}
			else if(errno == EINTR){
				continue;
			}
			else{
				error_ = errno;
				delete helper;
				return ret;
			}
		}
		else
			sended+=ret;
	}
	//remember to release the event
	delete helper;
	return sended;
}



