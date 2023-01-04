#include "CondVar.h"
#include "uthread_private.h"

// TODO

CondVar::CondVar(){

}

void CondVar::wait(Lock &lock){
	
	disableInterrupts();
	cond_waiting_q.push(running);
	running->setState(State::BLOCK);
	lock._unlock();
	lock_obj = &lock;
	switchThreads();
	
	enableInterrupts();
}

void CondVar::signal(){
	disableInterrupts();
	if(!cond_waiting_q.empty()){
		TCB* tcb = cond_waiting_q.front();
		cond_waiting_q.pop();
		lock_obj->_signal(tcb);
	}
	enableInterrupts();
}

void CondVar::broadcast(){
	disableInterrupts();
	while(!cond_waiting_q.empty()){
		TCB* tcb = cond_waiting_q.front();
		cond_waiting_q.pop();
		lock_obj->_signal(tcb);
	}
	enableInterrupts();
}
