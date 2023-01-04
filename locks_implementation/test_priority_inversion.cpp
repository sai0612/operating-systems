#include "uthread.h"
#include "Lock.h"
#include "CondVar.h"
#include "SpinLock.h"
#include <cassert>
#include <cstdlib>
#include <iostream>

using namespace std;

#define UTHREAD_TIME_QUANTUM 10000
#define SHARED_BUFFER_SIZE 10
#define PRINT_FREQUENCY 100000
#define RANDOM_YIELD_PERCENT 50


// Shared buffer synchronization
static Lock lock;
static CondVar cv;
static SpinLock spin_lock;

int c_cv = 0;

void *worker(void *arg) {
    int my_tid = uthread_self();
    cout<<endl;
    cout<<"Thread "<<my_tid<<" says Hi!!!!!"<<endl;
    for(int i=0;i<100000;i++);
    return NULL;
}

void *worker_mutex(void *arg) {
    int my_tid = uthread_self();
    lock.lock();
    cout<<endl;
    cout<<"Thread "<<my_tid<<" says Hi!!!!!"<<endl;
    
    cout<<"Thread "<<my_tid<<" entered critical section"<<endl;
    for(int i=0;i<100000;i++);
    cout<<"Thread "<<my_tid<<" completed critical section"<<endl;
    lock.unlock();
    
    return NULL;
}

void *worker_mutex2(void *arg) {
    int my_tid = uthread_self();
    lock.lock();
    cout<<endl;
    cout<<"Thread "<<my_tid<<" says Hi!!!!!"<<endl;
    
    cout<<"Thread "<<my_tid<<" entered critical section"<<endl;
    for(int i=0;i<100000;i++);
    cout<<"Thread "<<my_tid<<" completed critical section"<<endl;
    lock.unlock();
    for(int i=0;i<100000000;i++);
    cout<<"Thread "<<my_tid<<" exited"<<endl;
    return NULL;
}

void test_priority_inversion(){
	int quantum_usecs = 10000;
	int thread_count = 4;
	int ret = uthread_init(quantum_usecs);
	uthread_decrease_priority(0);
    	if (ret != 0) {
        	cerr << "uthread_init FAIL!\n" << endl;
        	exit(1);
    	}
    	int *threads = new int[thread_count];
    	int tid = uthread_create(worker_mutex, NULL);
    	cout<<"Thread "<<tid<<" is created"<<endl;
    	threads[0] = tid;
    	uthread_decrease_priority(threads[0]);
    	uthread_yield();
    	tid = uthread_create(worker_mutex, NULL);
    	cout<<"Thread "<<tid<<" is created"<<endl;
    	threads[1] = tid;
    	uthread_increase_priority(threads[1]);
    	tid = uthread_create(worker, NULL);
    	cout<<"Thread "<<tid<<" is created"<<endl;
    	threads[2] = tid;
    	tid = uthread_create(worker, NULL);
    	cout<<"Thread "<<tid<<" is created"<<endl;
    	threads[3] = tid;
    	uthread_increase_priority(threads[3]);
    	
    	
    	for (int i = 0; i < thread_count; i++) {
    		cout<<"Main Thread joining on Thread "<<threads[i]<<endl;
        	uthread_join(threads[i], NULL);
    	}
    	delete[] threads;
}

void test_priority_inversion_2(){
	int quantum_usecs = 10000;
	int thread_count = 4;
	int ret = uthread_init(quantum_usecs);
	uthread_decrease_priority(0);
    	if (ret != 0) {
        	cerr << "uthread_init FAIL!\n" << endl;
        	exit(1);
    	}
    	int *threads = new int[thread_count];
    	int tid = uthread_create(worker_mutex2, NULL);
    	cout<<"Thread "<<tid<<" is created"<<endl;
    	threads[0] = tid;
    	uthread_decrease_priority(threads[0]);
    	uthread_yield();
    	tid = uthread_create(worker_mutex2, NULL);
    	cout<<"Thread "<<tid<<" is created"<<endl;
    	threads[1] = tid;
    	uthread_increase_priority(threads[1]);
    	tid = uthread_create(worker, NULL);
    	cout<<"Thread "<<tid<<" is created"<<endl;
    	threads[2] = tid;
    	tid = uthread_create(worker, NULL);
    	cout<<"Thread "<<tid<<" is created"<<endl;
    	threads[3] = tid;
    	uthread_increase_priority(threads[3]);
    	
    	
    	for (int i = 0; i < thread_count; i++) {
    		cout<<"Main Thread joining on Thread "<<threads[i]<<endl;
        	uthread_join(threads[i], NULL);
    	}
    	delete[] threads;
}

int main(int argc, char *argv[]) {
  	test_priority_inversion();
  	
  	//To further prove priority inversion works please uncomment this line
  	//test_priority_inversion_2();
    	return 0;
}
