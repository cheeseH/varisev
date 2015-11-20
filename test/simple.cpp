/*
 * simple.cpp
 *
 *  Created on: 2015年11月10日
 *      Author: cheese
 */

#include "../src/varisevent.h"
#include "../src/corsocket.h"
#include "../src/eventmanager.h"
#include "../src/coroutinemanager.h"
#include "../src/looper.h"
#include "../src/multithreadwork.h"
#include "../src/workerpool.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <sys/time.h>


class EchoEvent:public VarisEvent{
public:
	EchoEvent(int fd,int onListenType):VarisEvent(fd,onListenType){

	}
	void callback(){
		char* str = "hello varisev";
		write(descriptor_,(void*)str,13);
	}
};
class AcceptEvent:public VarisEvent{
public:
	void callback(){
		int acceptfd = accept(descriptor_,(sockaddr*)&clientaddr,&clilen);
		 if(acceptfd<0){
			 perror("acceptfd<0");
             return;
	     }
		 int opts;
		 opts=fcntl(acceptfd,F_GETFL);
		 if(opts<0)
		 {
			 perror("fcntl(sock,GETFL)");
	         return;
		 }
		 opts = opts|O_NONBLOCK;
		 if(fcntl(acceptfd,F_SETFL,opts)<0)
		 {
			 perror("fcntl(sock,SETFL,opts)");
		     return;
		 }
		 CorSocket* corSock = new CorSocket(acceptfd,eventManager_,this);
		 char* buffer = new char[4];
		 while(true){
			 corSock->Recv(buffer,4);
			 corSock->Send(buffer,4);
//			 char* line = new char[1];
//			 line[0] = '\n';
//			 corSock->Send(line,1);
		 }
	}
	AcceptEvent(int fd,int listenType):VarisEvent(fd,listenType),clientaddr(),clilen(){

	}
private:
	sockaddr_in clientaddr;
	socklen_t clilen;
};

class FileReadWork:public MultiThreadWork{
public:
	FileReadWork(int fd,int buflen,char* buf,int* res):MultiThreadWork(),fd_(fd),buflen_(buflen),buf_(buf),len_(res){

	}

	void work(){
		int received = 0;
		while(received < buflen_){
			int ret = read(fd_,buf_,buflen_-received);
			if(ret == -1){
				if(errno == EAGAIN){
					continue;
				}
				else
					perror(strerror(errno));
			}
				if(ret == 0)
					break;
				else
					received += ret;

			*len_ = received;

		}
}
private:
	int fd_;
	char* buf_;
	int buflen_;
	int* len_;
};

class TimerTestEvent:public VarisEvent{
public:
	TimerTestEvent(int fd,int listenType):VarisEvent(fd,listenType){

	}
	void callback(){
		timeval curTime;
		gettimeofday(&curTime,NULL);
		std::cout<<"sec:"<<curTime.tv_sec<<" usec:"<<curTime.tv_usec<<std::endl;
		int fd = open("test2",O_RDONLY);
		if(fd == -1)
		{
			perror(strerror(errno));
			exit(-1);
		}
		char* buf = new char[1024];
		int* ret = new int(0);
		MultiThreadWork* multi = new FileReadWork(fd,1024,buf,ret);
		runThreadWork(multi);
		std::cout<<"back"<<std::endl;
		std::cout<<buf<<std::endl;
		delete multi;
		delete buf;
		delete ret;
		close(fd);

	}
private:

};




int main(){
//	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
//	sockaddr_in serveraddr;
//	memset(&serveraddr,0,sizeof(serveraddr));
//	serveraddr.sin_family = AF_INET;
//	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
//	serveraddr.sin_port=htons(9075);
//	if(bind(listenfd,(sockaddr *)&serveraddr, sizeof(serveraddr))<0)
//		perror("bind fault\n");
//	if(listen(listenfd, 20)<0){
//		perror("listen fault\n");
//	}
//	 int opts;
//			 opts=fcntl(listenfd,F_GETFL);
//			 if(opts<0)
//			 {
//				 perror("fcntl(sock,GETFL)");
//		         return 0;
//			 }
//			 opts = opts|O_NONBLOCK;
//			 if(fcntl(listenfd,F_SETFL,opts)<0)
//			 {
//				 perror("fcntl(sock,SETFL,opts)");
//			     return 0;
//			 }
	VarisEvent* event = new TimerTestEvent(-1,T_TIME|T_CONTINUE);
	EventManager* manager = new EventManager();
	CoroutineManager* corManager = new CoroutineManager(8,4096);
	Looper* looper = new CorLooper(corManager);
	WorkerPool* pool = new WorkerPool(3,127);
	manager->setLooper(looper);
	manager->setPool(pool);
	manager->Register(event,500);
	manager->Start();
	return 0;
}

