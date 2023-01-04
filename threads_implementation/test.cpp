#include "uthread.h"
#include <iostream>
#include <unistd.h>
#include <chrono>

using namespace std;

// worker thread to test context switching
void *worker1(void *arg) {
    int my_tid = uthread_self();
    cout<<"Thread "<<my_tid<<" says Hi!!!!!"<<endl;
    int counter =0;
    while(true){
    	for(int i=0;i<3;i++){
    		counter = counter+1;
    		cout<<"Thread "<<my_tid<<" counter is: "<<counter<<endl;
    		
    	}
    	if(counter>=6){
    		break;
    	}
    	uthread_yield();
    }
    
    return NULL;
}


//worker2 for testing the scheduling, time slice, join, resume and suspend functions.
void *worker2(void *arg) {
    int my_tid = uthread_self();
    cout<<endl;
    cout<<"Thread "<<my_tid<<" says Hi!!!!!"<<endl;
    int counter =0;
    while(true){
    	counter++;
    	cout<<"Thread "<<my_tid<<" counter is: "<<counter<<endl;
    	if(counter>=6){
    		break;
    	}
    	for(int i=0;i<3000000;i++){
    	}
    }
    
    return NULL;
}

//worker thread for testing minor functions uthread_get_quantum() and uthread_get_total_quantum()
void *worker3(void *arg) {
    int my_tid = uthread_self();
    cout<<endl;
    cout<<"Thread "<<my_tid<<" says Hi!!!!!"<<endl;
    int counter =0;
    while(true){
    	counter++;
    	if(counter>=6){
    		break;
    	}
    }
    
    return NULL;
}

//test_context_switching function tests context switching between four threads to see if the context is getting saved and reloaded correctly.
void test_context_switching(){
	int quantum_usecs = 100000;
	int thread_count = 4;
	int ret = uthread_init(quantum_usecs);
    if (ret != 0) {
        cerr << "uthread_init FAIL!\n" << endl;
        exit(1);
    }
    int *threads = new int[thread_count];
    // Create threads
    for (int i = 0; i < thread_count; i++) {
        int tid = uthread_create(worker1, NULL);
        cout<<"Thread "<<tid<<" is created"<<endl;
        threads[i] = tid;
    }
    cout<<endl;
	
    while(uthread_yield()){
    	cout<<"Enters main thread"<<endl;
    }
    unsigned long g_cnt = 0;
    for (int i = 0; i < thread_count; i++) {
        uthread_join(threads[i], NULL);
    }
    delete[] threads;
}

//test_scheduling_time_slicing() function tests if the processes are preempted correctly after the time slice expires.
//we are examining the round robin scheduling behavior by providing two different quantums.
void test_scheduling_time_slicing(){
	cout<<"---------------------------------------------------------------"<<endl;
    cout<<"Testing for Quantum=100000 microseconds"<<endl;
	int quantum_usecs = 100000;
	int thread_count = 4;
	int ret = uthread_init(quantum_usecs);
    if (ret != 0) {
        cerr << "uthread_init FAIL!\n" << endl;
        exit(1);
    }
    int *threads = new int[thread_count];
    // Create threads
    for (int i = 0; i < thread_count; i++) {
        int tid = uthread_create(worker2, NULL);
        cout<<"Thread "<<tid<<" is created"<<endl;
        threads[i] = tid;
    }
    cout<<endl;
    for (int i = 0; i < thread_count; i++) {
        uthread_join(threads[i], NULL);
    }
    delete[] threads;
    
    
    cout<<"---------------------------------------------------------------"<<endl;
    cout<<"Testing for Quantum=100 microseconds"<<endl;
    quantum_usecs = 100;
	thread_count = 4;
	ret = uthread_init(quantum_usecs);
    if (ret != 0) {
        cerr << "uthread_init FAIL!\n" << endl;
        exit(1);
    }
    threads = new int[thread_count];
    // Create threads
    for (int i = 0; i < thread_count; i++) {
        int tid = uthread_create(worker2, NULL);
        cout<<"Thread "<<tid<<" is created"<<endl;
        threads[i] = tid;
    }
    cout<<endl;

    for (int i = 0; i < thread_count; i++) {
        uthread_join(threads[i], NULL);
    }
    delete[] threads;
}

//test_join() functions tests the behavior of the uthread_join()
void test_join(){
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
        int tid = uthread_create(worker2, NULL);
        cout<<"Thread "<<tid<<" is created"<<endl;
        threads[i] = tid;
    }

    for (int i = 0; i < thread_count; i++) {
    	cout<<"Main Thread joining on Thread "<<threads[i]<<endl;
        uthread_join(threads[i], NULL);
        cout<<"Thread "<<threads[i]<< " exited, Main Thread unblocked"<<endl;
    }
    delete[] threads;
}

//test_suspend_resume tests if uthread_resume() and uthread_suspend() are working correctly.
void test_suspend_resume(){
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
        int tid = uthread_create(worker2, NULL);
        cout<<"Thread "<<tid<<" is created"<<endl;
        threads[i] = tid;
    }
    
    uthread_suspend(threads[1]);
    cout<<"Thread "<<threads[1]<< " is suspended by Main thread"<<endl;

    for (int i = 0; i < thread_count; i++) {
        uthread_join(threads[i], NULL);
        if(i==0){
        	uthread_resume(threads[1]);
        	cout<<"Thread "<<threads[1]<< " is resumed by Main thread"<<endl;
        }
    }
    delete[] threads;
}

//test_uthread_get_quantums() tests uthread_get_quantums() function 
void test_uthread_get_quantums(){
	int quantum_usecs = 1000000;
	int thread_count = 4;
	int ret = uthread_init(quantum_usecs);
    if (ret != 0) {
        cerr << "uthread_init FAIL!\n" << endl;
        exit(1);
    }
    int *threads = new int[thread_count];
    // Create threads
    for (int i = 0; i < thread_count; i++) {
        int tid = uthread_create(worker3, NULL);
        cout<<"Thread "<<tid<<" is created"<<endl;
        threads[i] = tid;
    }
    
    for (int i = 0; i < thread_count; i++) {
    	cout<<"Thread "<<threads[i]<< " Quantum: "<< uthread_get_quantums(threads[i])<<endl;
    }

    for (int i = 0; i < thread_count; i++) {
        uthread_join(threads[i], NULL);
    }
    delete[] threads;
}

//test_uthread_get_total_quantums() tests uthread_get_total_quantums() function().
void test_uthread_get_total_quantums(){
	int quantum_usecs = 1000000;
	int thread_count = 4;
	int ret = uthread_init(quantum_usecs);
    if (ret != 0) {
        cerr << "uthread_init FAIL!\n" << endl;
        exit(1);
    }
    int *threads = new int[thread_count];
    // Create threads
    for (int i = 0; i < thread_count; i++) {
        int tid = uthread_create(worker3, NULL);
        cout<<"Thread "<<tid<<" is created"<<endl;
        threads[i] = tid;
    }
    
    cout<<"Total Quantum time: "<< uthread_get_total_quantums()<<endl;

    for (int i = 0; i < thread_count; i++) {
        uthread_join(threads[i], NULL);
    }
    delete[] threads;
}

int main(int argc, char *argv[]) {
	int test_case;
	cout<<"Enter 1 to test context switching"<<endl;
	cout<<"Enter 2 to text scheduling and time slicing"<<endl;
	cout<<"Enter 3 to test join functionality"<<endl;
	cout<<"Enter 4 to test resume and suspend"<<endl;
	cout<<"Enter 5 to test uthread_get_quantum()"<<endl;
	cout<<"Enter 6 to test uthread_get_total_quantums()"<<endl;
	cout<<"Please enter the test case number: ";
	cin>> test_case;
	switch(test_case){
		case 1:
			test_context_switching();
			break;
		case 2:
			test_scheduling_time_slicing();
			break;
		case 3:
			test_join();
			break;
		case 4:
			test_suspend_resume();
			break;
		case 5:
			test_uthread_get_quantums();
			break;
		case 6:
			test_uthread_get_total_quantums();
			break;
		default:
			cout<<"Please enter valid test case number"<<endl;
			break;
	}
    return 0;
}
