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

void *worker_spin(void *arg) {
    int my_tid = uthread_self();
    cout<<endl;
    cout<<"Thread "<<my_tid<<" says Hi!!!!!"<<endl;
    spin_lock.lock();
    cout<<"Thread "<<my_tid<<" entered critical section"<<endl;
    for(int i=0;i<100000;i++);
    cout<<"Thread "<<my_tid<<" completed critical section"<<endl;
    spin_lock.unlock();
    
    return NULL;
}


void *worker_condvar_1(void *arg) {
    int my_tid = uthread_self();
    cout<<endl;
    cout<<"Thread "<<my_tid<<" says Hi!!!!!"<<endl;
    cout<<"Thread "<<my_tid<<" waiting for condvar lock"<<endl;
    lock.lock();
    cv.wait(lock);
    cout<<"Thread "<<my_tid<<" received signal and started working"<<endl;
    c_cv--;
    cout<<"Thread "<<my_tid<<" changed c value to "<<c_cv<<endl;
    for(int i=0;i<100000;i++);
    lock.unlock();
    return NULL;
}

void *worker_condvar_2(void *arg) {
    int my_tid = uthread_self();
    cout<<endl;
    cout<<"Thread "<<my_tid<<" says Hi!!!!!"<<endl;
    lock.lock();
    c_cv++;
    cout<<"Thread "<<my_tid<<" changed c value to "<<c_cv<<endl;
    cout<<"Thread "<<my_tid<<" sending signal to another thread"<<endl;
    cv.signal();
    cout<<"Thread "<<my_tid<<" received control back"<<endl;
    for(int i=0;i<100000;i++);
    lock.unlock();
    return NULL;
}

void *worker_condvar_3(void *arg) {
    int my_tid = uthread_self();
    cout<<endl;
    cout<<"Thread "<<my_tid<<" says Hi!!!!!"<<endl;
    c_cv++;
    cout<<"Thread "<<my_tid<<" changed c value to "<<c_cv<<endl;
    cout<<"Thread "<<my_tid<<" sending broadcast signal to other threads"<<endl;
    cv.broadcast();
    cout<<"Thread "<<my_tid<<" received control back"<<endl;
    for(int i=0;i<100000;i++);
    return NULL;
}

void *worker(void *arg) {
    int my_tid = uthread_self();
    cout<<endl;
    cout<<"Thread "<<my_tid<<" says Hi!!!!!"<<endl;
    for(int i=0;i<100000;i++);
    return NULL;
}

//testing mutex locks
void test_mutex_locks(){
	int quantum_usecs = 10000;
	int thread_count = 4;
	int ret = uthread_init(quantum_usecs);
    	if (ret != 0) {
        	cerr << "uthread_init FAIL!\n" << endl;
        	exit(1);
    	}
    	int *threads = new int[thread_count];
    	// Create threads
    	for (int i = 0; i < thread_count; i++) {
        	int tid = uthread_create(worker_mutex, NULL);
        	cout<<"Thread "<<tid<<" is created"<<endl;
        	threads[i] = tid;
    	}

    	for (int i = 0; i < thread_count; i++) {
    		cout<<"Main Thread joining on Thread "<<threads[i]<<endl;
        	uthread_join(threads[i], NULL);
        	
    	}
    	delete[] threads;
}

void test_spin_locks(){
	int quantum_usecs = 10000;
	int thread_count = 4;
	int ret = uthread_init(quantum_usecs);
    	if (ret != 0) {
        	cerr << "uthread_init FAIL!\n" << endl;
        	exit(1);
    	}
    	int *threads = new int[thread_count];
    	// Create threads
    	for (int i = 0; i < thread_count; i++) {
        	int tid = uthread_create(worker_spin, NULL);
        	cout<<"Thread "<<tid<<" is created"<<endl;
        	threads[i] = tid;
    	}

    	for (int i = 0; i < thread_count; i++) {
    		cout<<"Main Thread joining on Thread "<<threads[i]<<endl;
        	uthread_join(threads[i], NULL);
    	}
    	delete[] threads;
}

void test_condVar_locks(){
	int quantum_usecs = 10000;
	int thread_count = 2;
	int ret = uthread_init(quantum_usecs);
    	if (ret != 0) {
        	cerr << "uthread_init FAIL!\n" << endl;
        	exit(1);
    	}
    	int *threads = new int[thread_count];
    	// Create threads
    	
        int tid = uthread_create(worker_condvar_1, NULL);
        cout<<"Thread "<<tid<<" is created"<<endl;
        threads[0] = tid;
        
        tid = uthread_create(worker_condvar_2, NULL);
        cout<<"Thread "<<tid<<" is created"<<endl;
        threads[1] = tid;

    	for (int i = 0; i < thread_count; i++) {
    		cout<<"Main Thread joining on Thread "<<threads[i]<<endl;
        	uthread_join(threads[i], NULL);
    	}
    	delete[] threads;
}

void test_condVar_broadcast(){
	int quantum_usecs = 10000;
	int thread_count = 4;
	int ret = uthread_init(quantum_usecs);
    	if (ret != 0) {
        	cerr << "uthread_init FAIL!\n" << endl;
        	exit(1);
    	}
    	int *threads = new int[thread_count];
    	// Create threads
    	for(int i=0;i<thread_count-1;i++){
    		int tid = uthread_create(worker_condvar_1, NULL);
        	cout<<"Thread "<<tid<<" is created"<<endl;
        	threads[i] = tid;
    	}
    	int tid = uthread_create(worker_condvar_3, NULL);
        cout<<"Thread "<<tid<<" is created"<<endl;
        threads[thread_count-1] = tid;

    	for (int i = 0; i < thread_count; i++) {
    		cout<<"Main Thread joining on Thread "<<threads[i]<<endl;
        	uthread_join(threads[i], NULL);
    	}
    	delete[] threads;
}

void test_conVar_Hoarse_semantics(){
	int quantum_usecs = 10000;
	int thread_count = 4;
	int ret = uthread_init(quantum_usecs);
    	if (ret != 0) {
        	cerr << "uthread_init FAIL!\n" << endl;
        	exit(1);
    	}
    	int *threads = new int[thread_count];
    	// Create threads
    	for (int i = 0; i < 2; i++) {
        	int tid = uthread_create(worker_condvar_1, NULL);
        	cout<<"Thread "<<tid<<" is created"<<endl;
        	threads[i] = tid;
    	}
    	
    	for (int i = 2; i < 4; i++) {
        	int tid = uthread_create(worker_condvar_2, NULL);
        	cout<<"Thread "<<tid<<" is created"<<endl;
        	threads[i] = tid;
    	}

    	for (int i = 0; i < thread_count; i++) {
    		cout<<"Main Thread joining on Thread "<<threads[i]<<endl;
        	uthread_join(threads[i], NULL);
    	}
    	delete[] threads;
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
    	threads[0] = tid;
    	uthread_decrease_priority(tid);
    	
    	uthread_yield();
    	
    	tid = uthread_create(worker_mutex, NULL);
    	threads[1] = tid;
    	uthread_increase_priority(tid);
    	
    	tid = uthread_create(worker, NULL);
    	threads[2] = tid;
    	
    	tid = uthread_create(worker, NULL);
    	threads[3] = tid;
    	uthread_increase_priority(tid);
    	
    	for (int i = 0; i < thread_count; i++) {
    		cout<<"Main Thread joining on Thread "<<threads[i]<<endl;
        	uthread_join(threads[i], NULL);
    	}
    	delete[] threads;
}

int main(int argc, char *argv[]) {
  	int test_case;
	cout<<"Enter 1 to test mutex locks"<<endl;
	cout<<"Enter 2 to test spin locks"<<endl;
	cout<<"Enter 3 to test condVar wait and signal mechanism"<<endl;
	cout<<"Enter 4 to test condVar broadcast mechanism"<<endl;
	cout<<"Enter 5 to test Hoarse semantics in condVar locks"<<endl;
	cout<<"Please Enter the test case number: ";
	cin>> test_case;
	switch(test_case){
		case 1:
			test_mutex_locks();
			break;
		case 2:
			test_spin_locks();
			break;
		case 3:
			test_condVar_locks();
			break;
		case 4:
			test_condVar_broadcast();
			break;
		case 5:
			test_conVar_Hoarse_semantics();
			break;
		default:
			cout<<"Please enter valid test case number"<<endl;
			break;
	}
    	return 0;
}
