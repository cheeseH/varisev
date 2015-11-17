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
			 char* line = new char[1];
			 line[0] = '\n';
			 corSock->Send(line,1);
		 }
	}
	AcceptEvent(int fd,int listenType):VarisEvent(fd,listenType),clientaddr(),clilen(){

	}
private:
	sockaddr_in clientaddr;
	socklen_t clilen;
};

class TimerTestEvent:public VarisEvent{
public:
	TimerTestEvent(int fd,int listenType):VarisEvent(fd,listenType){

	}
	void callback(){
		timeval curTime;
		gettimeofday(&curTime,NULL);
		std::cout<<"sec:"<<curTime.tv_sec<<" usec:"<<curTime.tv_usec<<std::endl;
	}
private:

};


int main(){
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port=htons(9074);
	if(bind(listenfd,(sockaddr *)&serveraddr, sizeof(serveraddr))<0)
		perror("bind fault\n");
	if(listen(listenfd, 20)<0){
		perror("listen fault\n");
	}
	 int opts;
			 opts=fcntl(listenfd,F_GETFL);
			 if(opts<0)
			 {
				 perror("fcntl(sock,GETFL)");
		         return 0;
			 }
			 opts = opts|O_NONBLOCK;
			 if(fcntl(listenfd,F_SETFL,opts)<0)
			 {
				 perror("fcntl(sock,SETFL,opts)");
			     return 0;
			 }
	VarisEvent* event = new AcceptEvent(listenfd,T_READ|T_CONTINUE);
	EventManager* manager = new EventManager();
	CoroutineManager* corManager = new CoroutineManager(8,4096);
	Looper* looper = new CorLooper(corManager);
	manager->setLooper(looper);
	manager->Register(event);
	manager->Start();
	return 0;
}

