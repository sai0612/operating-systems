#include "uthread.h"
#include "uthread_private.h"
#include "TCB.h"
#include <vector>
#include <queue>
#include <stdlib.h>
#include <map>
#include <algorithm>
#include <cassert>

using namespace std;

#define NUM_OF_QUEUE 3
#define FAIL -1
#define SUCCESS 0
#define MAIN_THREAD 0
#define MICRO_TO_SECOND 1000000
#define THREAD_ERROR "thread library error: "
#define SYS_ERROR "system error: "
#define NOT_FOUND_ID 0
#define SUSPEND_MAIN 1
#define SET_TIME_ERROR 2
#define WRONG_INPUT 3
#define SIGNAL_ACTION_ERROR 4
#define TOO_MANY_THREADS 5

typedef struct join_queue_entry {
  TCB *tcb;
  int waiting_for_tid;
} join_queue_entry_t;

typedef struct finished_queue_entry {
  TCB *tcb;
  void *result;
} finished_queue_entry_t;

static map<Priority, vector<TCB*>> ready; // ready queue
TCB* running; // The "Running" thread.
static vector<TCB*> blocked; // The "Blocked" vector, which represents a queue of threads.
static vector<join_queue_entry_t> join_queue;
static vector<finished_queue_entry_t> finished_queue;
static map<int, TCB*> _threads; // All threads together
static int _quantum_counter = 0;
struct itimerval _timer;
struct sigaction _sigAction;
int* sig;
static map<TCB*, int> priority_inv;

static int removeFromReady(int tid);
static TCB* popReady();

/**
 * function responsable for printing each kind of error
 */
void printError(int type, string pre)
{
	switch (type)
	{
	case NOT_FOUND_ID:
	{
		cerr << pre << "the requested tid not found" << endl;
		break;
	}
	case SUSPEND_MAIN:
	{
		cerr << pre << "You are trying to suspend the main thread" << endl;
		break;
	}
	case SET_TIME_ERROR:
	{
		cerr << pre << "Unable to set timer" << endl;
		break;
	}
	case WRONG_INPUT:
	{
		cerr << pre << "wrong input" << endl;
		break;
	}
	case SIGNAL_ACTION_ERROR:
	{
		cerr << pre << "signal action error" << endl;
		break;
	}
	case TOO_MANY_THREADS:
	{
		cerr << pre << "too many threads" << endl;
		break;
	}
	default:
		break;
	}
}


/*
 * This function finds & returns the next smallest nonnegative integer not already taken by an existing thread,
 * or -1 if there are no available free ids.
 */
int getNextId()
{
	int idToReturn = 0;
	for (map<int, TCB*>::iterator iter = _threads.begin(); iter != _threads.end();
			idToReturn++, iter++)
	{
		if (iter->first != idToReturn)
		{
			break;
		}
	}
	return idToReturn;
}

/**
 * add thread to the requsted ready queue
 */
void addToReady(TCB* th)
{
	if(th == NULL)
	{
		return;
	}
	ready[th->getPriority()].push_back(th);
    //ready.insert(ready.end(), th);
}


/**
 * set time and check if set is done correctly
 */
static void setTime()
{
	if (setitimer(ITIMER_VIRTUAL, &_timer, NULL) == FAIL)
	{
		printError(SET_TIME_ERROR, SYS_ERROR);
		exit(1);
	}
}


/*
 * returns and remove from Ready the first thread in the queue 
 */
TCB* getReady(Priority priority)
{
	TCB* ret = ready[priority][0];
	ready[priority].erase(ready[priority].begin());
	return ret;
}

/*
 * returns a thread from Ready and removes it from there.
 * returns NULL in case there are no threads in Ready.
 */
TCB* popReady()
{
    if(ready.find(Priority::RED)!=ready.end()&&!ready[Priority::RED].empty()){
        return getReady(Priority::RED);
    }else if(ready.find(Priority::ORANGE)!=ready.end()&&!ready[Priority::ORANGE].empty()){
    	return getReady(Priority::ORANGE);
    }else if(ready.find(Priority::GREEN)!=ready.end()&&!ready[Priority::GREEN].empty()){
    	return getReady(Priority::GREEN);
    }

	return NULL;
}


/*
 * removes the thread with the given tid from ready.
 */
int removeFromReady(int tid)
{
	if(_threads.find(tid)!=_threads.end()){
		TCB* tcb = _threads[tid];
		Priority priority = tcb->getPriority();
		if(find(ready[priority].begin(), ready[priority].end(), _threads[tid]) != ready[priority].end())
		{
		
			for (vector<TCB*>::iterator iter = ready[priority].begin(); iter != ready[priority].end(); ++iter)
			{
				if (*iter == _threads.at(tid))
				{
					ready[priority].erase(iter);
					return SUCCESS;
				}
			}
		}
	}
	
	return FAIL;
}


/*
 * removes the thread with the given tid from blocked.
 */
void removeFromBlock(int tid)
{
	for (vector<TCB*>::iterator iter = blocked.begin(); iter != blocked.end(); ++iter)
	{
		if (*iter == _threads.at(tid))
		{
			blocked.erase(iter);
			break;
		}
	}
}

/*
 * Removes the thread with the given tid from the finished queue
 * Returns true if the specified tid is in the finished queue, false otherwise
 * Also returns the thread result to retval
 */
bool removeFromFinished(int tid, void **retval)
{
	for (vector<finished_queue_entry_t>::iterator iter = finished_queue.begin(); iter != finished_queue.end(); ++iter)
	{
		if (iter->tcb->getId() == tid)
		{
			// Save the result pointer
                        if (retval)
                        {
                                *retval = iter->result;
                        }

                        // Free the TCB entry and delete the TCB
                        // NOTE: Could delete the TCB earlier to free its stack
                        //       while waiting for a join
                        _threads.erase(tid);
                        delete iter->tcb;

                        // Remove from the finished queue
                        finished_queue.erase(iter);

                        return true;
		}
	}

        return false;
}

/*
 * Moves any threads that have joined on tid to the ready queue
 */
void moveFromJoinToReady(int tid)
{
	for (vector<join_queue_entry_t>::iterator iter = join_queue.begin(); iter != join_queue.end(); /* Nothing to do */)
        {
                // Check if this thread is waiting to join on this TID
                if (tid == iter->waiting_for_tid)
                {
                        // Move the thread from the join queue to the ready
                        // queue
                        iter->tcb->setState(READY);
                        addToReady(iter->tcb);
                        iter = join_queue.erase(iter);
                }
                else
                {
                        iter++;
                }
        }
}

// Switch to the thread provided
void switchToThread(TCB *next)
{
        bool already_switched_contexts = false;

        // Save the running thread context
        getcontext(running->getContext());

        // If this is the first return from saveContext then fall through,
        // otherwise this is the second return so break out
        if (already_switched_contexts)
        {
		return;
        }

        // Next return from saveContext break out
        already_switched_contexts = true;

        // Pick a new thread to run and restart the quantum
        running = next;
        running->setState(RUNNING);
        running->increaseQuantum();
        _quantum_counter++;
        setTime();

        // Switch to the new thread
        setcontext(running->getContext());
        assert(false);
}

// Switch to the next thread on the ready queue
void switchThreads()
{
	TCB *next = popReady();
	assert(next);
	switchToThread(next);
}

static bool interrupts_enabled = true;
void disableInterrupts()
{
        assert(interrupts_enabled);
	sigprocmask(SIG_BLOCK,&_sigAction.sa_mask, NULL);
        interrupts_enabled = false;
}

void enableInterrupts()
{
        assert(!interrupts_enabled);
        interrupts_enabled = true;
	sigprocmask(SIG_UNBLOCK,&_sigAction.sa_mask, NULL);
}

/**
 * switch between running thread and the this thread
 */
static void timeHandler(int signum)
{
        uthread_yield();
}

/*=================================================================================================
 * ======================================Library Functions=========================================
 * ================================================================================================
 */

/* Stub function */
void stub(void *(*start_routine)(void *), void *arg)
{
        enableInterrupts();
        void *result = start_routine(arg);
        uthread_exit(result);
}

/* Initialize the thread library */
int uthread_init(int quantum_usecs)
{
	if (quantum_usecs <= 0)
	{
		printError(WRONG_INPUT ,THREAD_ERROR);
		return FAIL;
	}

	//initialize sigaction
	_sigAction.sa_handler = timeHandler;
	if(sigemptyset (&_sigAction.sa_mask) == FAIL)
	{
		printError(SIGNAL_ACTION_ERROR, SYS_ERROR);
		exit(1);
	}
	if(sigaddset(&_sigAction.sa_mask, SIGVTALRM))
	{
		printError(SIGNAL_ACTION_ERROR, SYS_ERROR);
		exit(1);
	}
	_sigAction.sa_flags = 0;
	if(sigaction(SIGVTALRM,&_sigAction,NULL) == FAIL)
	{
		printError(SIGNAL_ACTION_ERROR, SYS_ERROR);
		exit(1);
	}

	//initialize timer
	_timer.it_value.tv_sec = (int)(quantum_usecs/MICRO_TO_SECOND);
	_timer.it_value.tv_usec = quantum_usecs % MICRO_TO_SECOND;
	_timer.it_interval.tv_sec = (int)(quantum_usecs/MICRO_TO_SECOND);
	_timer.it_interval.tv_usec = quantum_usecs % MICRO_TO_SECOND;

	//initialize main
	TCB* mainTh = new TCB(0, Priority::ORANGE, NULL, NULL, READY);
	_threads.insert(pair<int, TCB*>(0, mainTh));
	running = mainTh;
	mainTh->setState(RUNNING);
	mainTh->increaseQuantum();
	_quantum_counter++;
	setTime();
	return SUCCESS;
}

/* Create a new thread whose entry point is f */
//int uthread_create(void *(*start_routine)(void), void *arg)
int uthread_create(void* (*start_routine)(void*), void* arg)
{
	//can't add any more!!
	if(_threads.size() == 100)
	{
		printError(TOO_MANY_THREADS, THREAD_ERROR);
		return FAIL;
	}

        disableInterrupts();

	int tid = getNextId();
	TCB* th = new TCB(tid, Priority::ORANGE, start_routine, arg, READY);
	_threads.insert(pair<int, TCB*>(tid, th));

	addToReady(th);

	enableInterrupts();
	return tid;
}

/* Join a thread */
//int uthread_join(int tid)
int uthread_join(int tid, void **retval)
{
        disableInterrupts();

        // Check if thread is on finish queue
        void *result = nullptr;
        bool already_terminated = removeFromFinished(tid, &result);

        if (!already_terminated)
        {
                // Add this thread to the join queue waiting on thread id
                join_queue_entry_t join_entry =
                {
                        .tcb = _threads.at(uthread_self()),
                        .waiting_for_tid = tid
                };
                join_queue.push_back(join_entry);

                // Switch to another thread
                switchThreads();

                // The thread has been woken up so the join has completed
                already_terminated = removeFromFinished(tid, &result);
                assert(already_terminated);
        }

        // Update the result pointer
        if (retval)
        {
                *retval = result;
        }

        enableInterrupts();

	return 0;
}

int uthread_yield(void)
{
 	disableInterrupts();

        // Move the running thread to the ready state
 	running->setState(READY);
        addToReady(running);

        // Switch to another thread
 	switchThreads();

 	enableInterrupts();

 	return SUCCESS;
}

/* Terminates this thread */
void uthread_exit(void *retval)
{
	int tid = running->getId();

	//terminate main
	if (tid == MAIN_THREAD)
	{
                // Clean up the thread TCBs
	        for (map<int, TCB*>::iterator iter = _threads.begin(); iter != _threads.end(); iter++)
	        {
                        delete iter->second;
	        }
		exit(0);
	}

	disableInterrupts();

        // Move any threads joining on this thread to the ready queue
        moveFromJoinToReady(tid);

        // Move this thread to the finished queue
        finished_queue_entry_t finished_queue_entry =
        {
                .tcb = _threads.at(tid),
                .result = retval
        };
        finished_queue.push_back(finished_queue_entry);

        // Switch to the next ready thread
	switchThreads();

        // Does not return from switchThreads
        assert(false);
}

/* Suspend a thread */
int uthread_suspend(int tid)
{
	if (tid == MAIN_THREAD)
	{
		printError(SUSPEND_MAIN, THREAD_ERROR);
		return FAIL;
	}
	if (!_threads.count(tid))
	{
		printError(NOT_FOUND_ID, THREAD_ERROR);
		return FAIL;
	}
	disableInterrupts();
	if (tid == running->getId())
	{
		running->setState(BLOCK);
		blocked.push_back(running);
                switchThreads();
	}
	else
	{
		if(removeFromReady(_threads[tid]->getId()) == FAIL)  // Exists, not in ready and not running - in blocked.
		{
			enableInterrupts();
			return FAIL;
		}
		// Was in Ready
		_threads[tid]->setState(BLOCK);
		blocked.push_back(_threads[tid]);
	}
	enableInterrupts();
	return SUCCESS;
}

/* Resume a thread */
int uthread_resume(int tid)
{
	if(!_threads.count(tid))
	{
		printError(NOT_FOUND_ID, THREAD_ERROR);
		return FAIL;
	}
	//if not in block, don't resume
	if(find(blocked.begin(), blocked.end(), _threads[tid]) != blocked.end())
	{
		disableInterrupts();
		int pos = find(blocked.begin(), blocked.end(), _threads[tid]) - blocked.begin();
		TCB* th = blocked.at((unsigned int) pos);
		th->setState(READY);
		addToReady(th);
		removeFromBlock(tid);
		enableInterrupts();
		return SUCCESS;
	}
	else
	{
		return FAIL;
	}
}

/* Get the id of the calling thread */
int uthread_self()
{
	return running->getId();
}

/* Get the total number of library quantums */
int uthread_get_total_quantums()
{
	return _quantum_counter;
}

/* Get the number of thread quantums */
int uthread_get_quantums(int tid)
{
	disableInterrupts();
	int valToRet;
	if (!_threads.count(tid))
	{
		printError(NOT_FOUND_ID, THREAD_ERROR);
		valToRet = FAIL;
	}
	else
	{
		valToRet = _threads.at(tid)->getQuantum();
	}
	enableInterrupts();
	return valToRet;
}


/* Increase the thread's priority by one level */
int uthread_increase_priority(int tid)
{
    // TODO
    	if(_threads.find(tid)==_threads.end()){
    		return FAIL;
    	}
    	TCB* tcb = _threads[tid];
    	disableInterrupts();
    	int remove_succ = removeFromReady(tid);
    	tcb->increasePriority();
    	if(remove_succ==SUCCESS){
    		addToReady(tcb);
    	}
    	priority_inv[tcb]++;
    	enableInterrupts();
    	return SUCCESS;
}

/* Decrease the thread's priority by one level */
int uthread_decrease_priority(int tid)
{
    // TODO
	if(_threads.find(tid)==_threads.end()){
		return FAIL;
    	}
    	TCB* tcb = _threads[tid];
    	disableInterrupts();
    	int remove_succ = removeFromReady(tid);
    	tcb->decreasePriority();
    	if(remove_succ==SUCCESS){
    		addToReady(tcb);
    	}
    	priority_inv[tcb]--;
    	if(priority_inv[tcb]==0){
    		priority_inv.erase(tcb);
    	}
    	enableInterrupts();
    	return SUCCESS;
}

bool higher_prior_exists(){
	Priority curr_prior = running->getPriority();
	if(curr_prior == Priority::RED){
		return false;
	}
	if(curr_prior == Priority::ORANGE && ready.find(Priority::RED)!=ready.end() && ready[Priority::ORANGE].size()>0){
		return true;
	}
	if(curr_prior == Priority::GREEN){
		if(ready.find(Priority::RED)!=ready.end() && ready[Priority::RED].size()>0)
		 	return true;
		if(ready.find(Priority::ORANGE)!=ready.end() && ready[Priority::ORANGE].size()>0)
			return true;
	}
	return false;
}

void priority_inversion(){
	if(running->getPriority() == Priority::RED&&priority_inv.find(running)==priority_inv.end()){
		return;
	}
	if(running->getLockCount()==0&&priority_inv.find(running)!=priority_inv.end()){
		uthread_decrease_priority(running->getId());
		if(priority_inv.find(running)!=priority_inv.end()){
			uthread_decrease_priority(running->getId());
		}
	}
	if(running->getLockCount()>0){
		if(higher_prior_exists()){
			uthread_increase_priority(running->getId());
			if(running->getPriority()==Priority::ORANGE){
				uthread_increase_priority(running->getId());
			}
		}
	}
}


