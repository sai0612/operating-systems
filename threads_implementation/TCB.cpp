#define STACK_SIZE 4096
#include "TCB.h"

TCB::TCB(int tid, void *(*start_routine)(void* arg), void *arg, State state)
{
	this-> _tid = tid;
	this->_state = state;
	saveContext();
	_context.uc_stack.ss_sp = new char[STACK_SIZE];
    	_context.uc_stack.ss_size = STACK_SIZE;
    	_context.uc_stack.ss_flags = 0;
    	makecontext(&_context, (void(*)())stub, 2, start_routine, arg);
}

TCB::TCB(int tid, State state)
{
	this-> _tid = tid;
	this->_state = state;
	saveContext();
}

TCB::~TCB()
{
}

void TCB::setState(State state)
{
	this->_state = state;
}

State TCB::getState() const
{
	return this->_state;
}

int TCB::getId() const
{
	return this->_tid;
}

void TCB::increaseQuantum(int quantum)
{
	this->_quantum += quantum;
}

int TCB::getQuantum() const
{
	return this->_quantum;
}

int TCB::saveContext()
{
	return getcontext(&_context);
}

void TCB::loadContext()
{
	setcontext(&_context);
}
