#include "Lock.h"
#include "uthread_private.h"

// TODO

Lock::Lock(){
	flag = false;
}

//attempt to acquire lock
void Lock::lock(){
	disableInterrupts();
	if(flag==false){
		flag = true;
	}
	else{
		lock_waiting_q.push(running);
		running->setState(State::BLOCK);
		switchThreads();
	}
	running->increaseLockCount();
	enableInterrupts();
}

void Lock::unlock(){
	disableInterrupts();
	running->decreaseLockCount();
	_unlock();
	enableInterrupts();
}

void Lock::_unlock(){
	if(!signal_waiting_q.empty()){
		TCB* tcb = signal_waiting_q.front();
		signal_waiting_q.pop();
		running->setState(State::READY);
		addToReady(running);
		switchToThread(tcb);
		
	}
	if(!lock_waiting_q.empty()){
		TCB* tcb = lock_waiting_q.front();
		lock_waiting_q.pop();
		tcb->setState(State::READY);
		addToReady(tcb);
	} else{
		flag = false;
	}
	
}

void Lock::_signal(TCB *tcb){
	signal_waiting_q.push(running);
	switchToThread(tcb);
}
