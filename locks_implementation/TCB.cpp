/*
 *
 */

#include "TCB.h"
#include <cassert>

TCB::TCB(int tid, Priority priority, void *(*start_routine)(void* arg), void *arg, State state): _tid(tid), _priority(priority), _quantum(0), _state(state), _lock_count(0)
{
        _stack = nullptr;

        // Only allocate a stack and setup the context if this is not the main
        // thread
        if (start_routine != NULL)
        {
                // Allocate a stack for the new thread
	        _stack = new char[STACK_SIZE];

                // Set up the context with the newly allocated stack
                getcontext(&_context);
                _context.uc_stack.ss_sp = _stack;
                _context.uc_stack.ss_size = STACK_SIZE;
                _context.uc_stack.ss_flags = 0;

                // Set the context to call the stub
                makecontext(&_context, (void(*)())stub, 2, start_routine, arg);
        }
}

TCB::~TCB()
{
        if (_stack)
        {
	        delete[] _stack;
        }
}

void TCB::setState(State state)
{
	_state = state;
}

State TCB::getState() const
{
	return _state;
}

int TCB::getId() const
{
	return _tid;
}

void TCB::increaseQuantum()
{
	_quantum++;
}

int TCB::getQuantum() const
{
	return _quantum;
}

void TCB::increaseLockCount()
{
	_lock_count++;
}

void TCB::decreaseLockCount()
{
	assert(_lock_count > 0);
	_lock_count--;
}

int TCB::getLockCount()
{
	return _lock_count;
}

ucontext_t* TCB::getContext()
{
	return &_context;
}

Priority TCB::getPriority()
{
	return _priority;
}

void TCB::increasePriority()
{
	if(_priority==Priority::GREEN){
		_priority = Priority::ORANGE;
	}else if(_priority==Priority::ORANGE){
		_priority = Priority::RED;
	}
}

void TCB::decreasePriority()
{
	if(_priority==Priority::RED){
		_priority = Priority::ORANGE;
	}else if(_priority==Priority::ORANGE){
		_priority = Priority::GREEN;
	}
}
