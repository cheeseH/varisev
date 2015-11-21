/*
 * coroutinemanager.cpp
 *
 *  Created on: 2015年11月16日
 *      Author: cheese
 */
#include "coroutinemanager.h"
#include "eventmanager.h"
#include <iostream>
typedef void (*corfunc)(void);
Coroutine::Coroutine(size_t id,size_t stackSize):id_(id),stackSize_(stackSize),status_(FREE),cManager_(NULL){
	stack_ = new char[stackSize_];
	getcontext(context_);
}
Coroutine::Coroutine(){
	context_ = new ucontext_t();
}
void Coroutine::init(size_t id,size_t stackSize,CoroutineManager* cManager){
	id_ = id;
	stackSize_ = stackSize;
	stack_ = new char[stackSize_];
	getcontext(context_);
	context_->uc_stack.ss_size = stackSize_;
	context_->uc_stack.ss_sp = stack_;
	cManager_ = cManager;
}

size_t Coroutine::getId(){
	return id_;
}

ucontext_t* Coroutine::getContext(){
	return context_;
}
void Coroutine::setContext(ucontext_t* context){
	context_ = context;
	context_->uc_stack.ss_sp = stack_;
	context_->uc_stack.ss_size = stackSize_;
}
void Coroutine::work(){
	swapcontext(cManager_->coroutines_[cManager_->lastCoroutine_].getContext(),context_);
}
void Coroutine::yield(){
	cManager_->scheduleBack();
}

void Coroutine::setStatus(int status){
	status_ = status;
}

int Coroutine::getStatus(){
	return status_;
}




//manager
CoroutineManager::CoroutineManager(size_t workerSize,size_t stackSize){
	realSize_ = workerSize+2;
	stackSize_ = stackSize;
	freeCoroutine_ = std::stack<size_t>();
	coroutines_ = new Coroutine[realSize_];
	for(int i = 0;i<realSize_;i++){
		coroutines_[i].init(i,stackSize_,this);
	}
	for(int i = 2;i<realSize_;i++){
		freeCoroutine_.push(i);
	}
	//the coroutine 1 is epoll coroutine
	lastCoroutine_ = 1;
	curCoroutine_ = 1;
	//update context
	ucontext_t* context = coroutines_[0].getContext();
	makecontext(context,(corfunc)onCoroutineFinished,1,this);
	context->uc_link = NULL;


}



//notice
void CoroutineManager::onCoroutineFinished(CoroutineManager* th){
	while(true){
		Coroutine cor = th->coroutines_[th->curCoroutine_];
		cor.setStatus(FREE);
		th->freeCoroutine_.push(th->curCoroutine_);
		th->curCoroutine_ = 0;
		th->runCoroutine(1);
	}
}
void CoroutineManager::schedule(size_t workerNumber){
	runCoroutine(workerNumber);

}
void CoroutineManager::scheduleBack(){
	runCoroutine(lastCoroutine_);
}

void CoroutineManager::runCoroutine(size_t workerNumber){
	Coroutine* coroutine = &(coroutines_[workerNumber]);
	lastCoroutine_ = curCoroutine_ ;
	curCoroutine_ = coroutine->getId();
	coroutine->work();
}

void CoroutineManager::runCorEvent(VarisEvent* event){
	size_t workerNumber = event->getCoroutine()->getId();
	Coroutine* coroutine = &(coroutines_[workerNumber]);
	lastCoroutine_ = curCoroutine_ ;
	curCoroutine_ = coroutine->getId();
	event->callback();
}

bool CoroutineManager::doWork(VarisEvent* event){
		Coroutine* cor;
		if((cor = event->getCoroutine())!=NULL &&( event->getListenEventType()&T_COROUTINE)){
			cor->setStatus(BUSY);
			runCorEvent(event);
			return true;
		}
		//no coroutine free
		if(freeCoroutine_.empty()){
			return false;
		}
	if(!(event->getListenEventType()&T_COROUTINE)){
			int tmpCoroutine = freeCoroutine_.top();
			freeCoroutine_.pop();
			ucontext_t* context = coroutines_[tmpCoroutine].getContext();
			context->uc_link = coroutines_[0].getContext();
			Coroutine* coroutine = &(coroutines_[tmpCoroutine]);
			coroutine->setStatus(READY);
			event->setCoroutine(coroutine);
			makecontext(context,(corfunc)(VarisEvent::handle),1,event);
			runCoroutine(tmpCoroutine);
			return true;
	}
	return false;

}


void CorEvent::callback(){
//	std::cout<<"id:"<<cor_->getId()<<std::endl;
	if(multi_){
		uint64_t u;
		read(descriptor_,&u,sizeof(uint64_t));
	}
	parent_->setCoroutine(cor_);
	cor_->work();
}

CorEvent::CorEvent(int fd,int listenType,VarisEvent* parent,int multi):VarisEvent(fd,listenType),parent_(parent),multi_(multi){
	cor_ = parent->getCoroutine();
	listenEventType_|=T_CONTINUE;
}

CorHelper::CorHelper(int fd,int listenEventType,VarisEvent* event,EventManager* manager,int multi):evManager_(manager),cor_(event->getCoroutine()){
	listenEventType|= T_COROUTINE;
	event_ = new CorEvent(fd,listenEventType,event,multi);
	evManager_->Register(event_,-1);
}

void CorHelper::yield(){
	cor_->yield();
}

CorHelper::~CorHelper(){
	if(event_->getState()!=OUT)
		evManager_->Release(event_);
}

