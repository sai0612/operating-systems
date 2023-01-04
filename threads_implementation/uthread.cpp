#include "uthread.h"
#include "TCB.h"
#include <cassert>
#include <deque>
#include <bits/stdc++.h>

using namespace std;

// Finished queue entry type
typedef struct finished_queue_entry {
  TCB *tcb;             // Pointer to TCB
  void *result;         // Pointer to thread result (output)
} finished_queue_entry_t;

// Join queue entry type
typedef struct join_queue_entry {
  TCB *tcb;             // Pointer to TCB
  int waiting_for_tid;  // TID this thread is waiting on
} join_queue_entry_t;

// You will need to maintain structures to track the state of threads
// - uthread library functions refer to threads by their TID so you will want
//   to be able to access a TCB given a thread ID
// - Threads move between different states in their lifetime (READY, BLOCK,
//   FINISH). You will want to maintain separate "queues" (doesn't have to
//   be that data structure) to move TCBs between different thread queues.
//   Starter code for a ready queue is provided to you
// - Separate join and finished "queues" can also help when supporting joining.
//   Example join and finished queue entry types are provided above

// Queues
static deque<TCB*> readyQueue;
static deque<TCB*> blockQueue;
static deque<finished_queue_entry_t*> finishedQueue;
static deque<join_queue_entry_t*> joinQueue;
sigset_t sig;
TCB *current_thread;
int quanta;
list<int> threadIds;
map<int, TCB*> threads;
bool interrupts_enabled = true;
// Interrupt Management --------------------------------------------------------

// Start a countdown timer to fire an interrupt
static void startInterruptTimer()
{
        // TODO
        struct itimerval timer;
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = current_thread->getQuantum();
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 0;
        setitimer(ITIMER_VIRTUAL, &timer, NULL);
}

// Block signals from firing timer interrupt
static void disableInterrupts()
{
        // TODO
        assert(interrupts_enabled);
        sigprocmask(SIG_BLOCK, &sig, NULL);
        interrupts_enabled = false;
        
}

// Unblock signals to re-enable timer interrupt
static void enableInterrupts()
{
        // TODO
        assert(!interrupts_enabled);
        interrupts_enabled = true;
        sigprocmask(SIG_UNBLOCK, &sig, NULL);
}


// Queue Management ------------------------------------------------------------

// Add TCB to the back of the ready queue
void addToReadyQueue(TCB *tcb)
{
        readyQueue.push_back(tcb);
}

// Removes and returns the first TCB on the ready queue
// NOTE: Assumes at least one thread on the ready queue
TCB* popFromReadyQueue()
{
        assert(!readyQueue.empty());

        TCB *ready_queue_head = readyQueue.front();
        readyQueue.pop_front();
        return ready_queue_head;
}

// Removes the thread specified by the TID provided from the ready queue
// Returns 0 on success, and -1 on failure (thread not in ready queue)
int removeFromReadyQueue(int tid)
{
        for (deque<TCB*>::iterator iter = readyQueue.begin(); iter != readyQueue.end(); ++iter)
        {
                if (tid == (*iter)->getId())
                {
                        readyQueue.erase(iter);
                        return 0;
                }
        }

        // Thread not found
        return -1;
}



// Helper functions ------------------------------------------------------------

// Switch to the next ready thread
static void switchThreads()
{
        // TODO
        // flag is a local stack variable to each thread
    	volatile int flag = 0;
	
   	// getcontext() will "return twice" - Need to differentiate between the two
    	int ret_val = getcontext(&current_thread->_context);
    	
	//int ret_val = current_thread->saveContext();
    	// If flag == 1 then it was already set below so this is the second return
    	// from getcontext (run this thread)
    	if (flag == 1) {
    		startInterruptTimer();
    		enableInterrupts();
        	return;
    	}
	cout << "SWITCH: currentThread = " << current_thread->getId();
    	disableInterrupts();
    	flag = 1;
    	if(current_thread->getState() == State::RUNNING){
    		current_thread->setState(State::READY);
    		addToReadyQueue(current_thread);
    	}
    	
    	TCB* available_thread = popFromReadyQueue();
    	bool finished = false;
    	for(int i=0;i<finishedQueue.size();i++){
        	if(finishedQueue[i]->tcb->getId() == available_thread->getId()){
        		finished = true;
        	}
        }
    	while(finished == true){
    		removeFromReadyQueue(available_thread->getId());
    		available_thread = popFromReadyQueue();
    		finished = false;
    		for(int i=0;i<finishedQueue.size();i++){
        		if(finishedQueue[i]->tcb->getId() == available_thread->getId()){
        			finished = true;
        		}
        	}
    	}
    	current_thread = available_thread;
    	current_thread->setState(State::RUNNING);
    	cout << "   SWITCHED TO: THREAD = " << current_thread->getId() <<endl;
    	setcontext(&current_thread->_context);
    	return;
    	
}


// Library functions -----------------------------------------------------------

// The function comments provide an (incomplete) summary of what each library
// function must do

// Starting point for thread. Calls top-level thread function
void stub(void *(*start_routine)(void *), void *arg)
{
        // TODO
        startInterruptTimer();
        enableInterrupts();
        void *return_val = (*start_routine)(arg);
        uthread_exit(return_val);
}

int uthread_init(int quantum_usecs)
{
        // Initialize any data structures
        // Setup timer interrupt and handler
        // Create a thread for the caller (main) thread
        quanta = quantum_usecs;
        struct sigaction sig;
        
        if(sigemptyset(&sig.sa_mask)< -1){
        	cout<<"ERROR: failed to empty the set"<<endl;
        	exit(1);
        }
        
        sig.sa_flags = 0;
        sig.sa_handler = helper_signal;
        if(sigaddset(&sig.sa_mask, SIGVTALRM)){
        	cout<<"ERROR: failed to add to set"<<endl;
        	exit(1);
        }
        sigaction(SIGVTALRM, &sig, NULL);
        
        for(int i=1; i<=MAX_THREAD_NUM;i++){
        	threadIds.push_back(i);
        }
        
        //creating Main thread
        TCB *tcb = new TCB(0, State::RUNNING);
        threads[0] = tcb;
        tcb->increaseQuantum(quanta);
        current_thread = tcb;
        cout<<"Main thread is created"<<endl<<endl;
        return 0;
}

int uthread_create(void* (*start_routine)(void*), void* arg)
{
        // Create a new thread and add it to the ready queue
        if(threadIds.size()==0){
        	cout<<"No. of threads created exceeded"<<endl;
        	exit(1);
        }
        disableInterrupts();
        int tid = threadIds.front();
        threadIds.pop_front();
        TCB* new_thread = new TCB(tid, start_routine, arg, State::READY);
        new_thread->increaseQuantum(quanta);
        addToReadyQueue(new_thread);
        threads[tid] = new_thread;
        enableInterrupts();
        return tid;
}

int uthread_join(int tid, void **retval)
{
        // If the thread specified by tid is already terminated, just return
        // If the thread specified by tid is still running, block until it terminates
        // Set *retval to be the result of thread if retval != nullptr
        bool finished = false;
        void *fentry = NULL;
        for(int i=0;i<finishedQueue.size();i++){
        	if(finishedQueue[i]->tcb->getId() == tid){
        		finished = true;
        		fentry = finishedQueue[i]->result;
        	}
        }
        if(finished == false){
        	disableInterrupts();
        	current_thread->setState(State::BLOCK);
        	join_queue_entry_t entry = {current_thread, tid};
        	joinQueue.push_back(&entry);
        	enableInterrupts();
        	uthread_yield();
        }
        for(int i=0;i<finishedQueue.size();i++){
        	if(finishedQueue[i]->tcb->getId() == tid){
        		fentry = finishedQueue[i]->result;
        	}
        }
        disableInterrupts();
        if(retval!=NULL){
        	*retval = fentry;
        }
        
        for(deque<finished_queue_entry_t*>::iterator itr = finishedQueue.begin(); itr!=finishedQueue.end(); itr++){
        	if((*itr)->tcb->getId() == tid){
        		finishedQueue.erase(itr);
        		break;
        	}
        }
        threadIds.push_back(tid);
        threads.erase(tid);
        enableInterrupts();
        return 1;
}

int uthread_yield(void)
{
        // TODO
        switchThreads();
        if(readyQueue.size()==0){
        	return 0;
        }
        return 1;
}

void helper_signal(int i){
	uthread_yield();
}

void uthread_exit(void *retval)
{
        // If this is the main thread, exit the program
        // Move any threads joined on this thread back to the ready queue
        // Move this thread to the finished queue
        disableInterrupts();
        if(threads.size()==1){
        	cout<<"no threads";
        	return;
        }
        for(deque<join_queue_entry_t*>::iterator itr = joinQueue.begin(); itr!=joinQueue.end(); itr++){
        	if((*itr)->waiting_for_tid == current_thread->getId()){
        		(*itr)->tcb->setState(State::READY);
        		addToReadyQueue((*itr)->tcb);
        		joinQueue.erase(itr);
        		break;
        	}
        }
        current_thread->setState(State::FINISHED);
        finished_queue_entry_t entry = {current_thread, retval};
        finishedQueue.push_back(&entry);
        removeFromReadyQueue(current_thread->getId());
        enableInterrupts();
        uthread_yield();
}

int uthread_suspend(int tid)
{
        // Move the thread specified by tid from whatever state it is
        // in to the block queue
        
        //checking if the specified thread id is in finished state
        for(deque<finished_queue_entry_t*>::iterator itr = finishedQueue.begin(); itr!=finishedQueue.end(); itr++){
        	if((*itr)->tcb->getId() == tid){
        		return -1;
        	}
        }
        if(threads.find(tid)==threads.end()){
        	return -1;
        }
        disableInterrupts();
        threads[tid]->setState(State::BLOCK);
        removeFromReadyQueue(tid);
        blockQueue.push_back(threads[tid]);
        if(current_thread->getId() == tid){
        	startInterruptTimer();
        	enableInterrupts();
        	uthread_yield();
        }
        enableInterrupts();
        return 1;
        
}

int uthread_resume(int tid)
{
        // Move the thread specified by tid back to the ready queue
        if(threads.find(tid)==threads.end()||threads[tid]->getState() != State::BLOCK){
        	return -1;
        }
        disableInterrupts();
        for(deque<TCB*>::iterator itr = blockQueue.begin(); itr!=blockQueue.end(); itr++){
        	if((*itr)->getId() == tid){
        		blockQueue.erase(itr);
        		break;
        	}
        }
        threads[tid]->setState(State::READY);
        addToReadyQueue(threads[tid]);
        enableInterrupts();
        return 1;
}

int uthread_self()
{
        return current_thread->getId();
}

int uthread_get_total_quantums()
{
        return threads.size()*quanta;
}

int uthread_get_quantums(int tid)
{
        return threads[tid]->getQuantum();
}
