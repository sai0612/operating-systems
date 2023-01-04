#include "uthread.h"
#include "Lock.h"
#include "SpinLock.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;

#define UTHREAD_TIME_QUANTUM 10000
#define SHARED_BUFFER_SIZE 10
#define PRINT_FREQUENCY 100000
#define RANDOM_YIELD_PERCENT 50


// Shared buffer synchronization
static Lock lock;
static SpinLock spin_lock;


void *worker_mutex(void *arg) {
    int my_tid = uthread_self();
    int points_per_cs = *(int*)arg;
    cout<<endl;
    cout<<"Thread "<<my_tid<<" says Hi!!!!!"<<endl;
    cout<<"Thread "<<my_tid<<" entered critical section"<<endl;
    lock.lock();
    cout<<points_per_cs<<endl;
    for(long j=0;j<(long)points_per_cs*1000;j++);
    lock.unlock();
    cout<<"Thread "<<my_tid<<" completed critical section"<<endl;
    
    return NULL;
}

void *worker_spin(void *arg) {
    int my_tid = uthread_self();
    int points_per_cs = *(int*)arg;
    cout<<endl;
    cout<<"Thread "<<my_tid<<" says Hi!!!!!"<<endl;
    cout<<"Thread "<<my_tid<<" entered critical section"<<endl;
    spin_lock.lock();
    for(long j=0;j<points_per_cs*1000;j++);
    spin_lock.unlock();
    cout<<"Thread "<<my_tid<<" completed critical section"<<endl;
    return NULL;
}

int spinlock_helper(int points_per_cs){
	auto start_time = std::chrono::high_resolution_clock::now();
	int quantum_usecs = 1000;
	int thread_count = 99;
	int ret = uthread_init(quantum_usecs);
    	if (ret != 0) {
        	cerr << "uthread_init FAIL!\n" << endl;
        	exit(1);
    	}
    	int *threads = new int[thread_count];
    	// Create threads
    	for (int i = 0; i < thread_count; i++) {
        	int tid = uthread_create(worker_spin, &points_per_cs);
        	cout<<"Thread "<<tid<<" is created"<<endl;
        	threads[i] = tid;
    	}

    	for (int i = 0; i < thread_count; i++) {
    		cout<<"Main Thread joining on Thread "<<threads[i]<<endl;
        	uthread_join(threads[i], NULL);
        	
    	}
    	
	auto end_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> secs = end_time-start_time;
    	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(secs);
    	cout << ms.count() <<"count";
    	cout << "aaaaaa";
    	delete[] threads;
	return ms.count();
}

int lock_helper(int points_per_cs){
	auto start_time = std::chrono::high_resolution_clock::now();
	int quantum_usecs = 10000;
	int thread_count = 99;
	int ret = uthread_init(quantum_usecs);
    	if (ret != 0) {
        	cerr << "uthread_init FAIL!\n" << endl;
        	exit(1);
    	}
    	int *threads = new int[thread_count];
    	// Create threads
    	for (int i = 0; i < thread_count; i++) {
        	int tid = uthread_create(worker_mutex, &points_per_cs);
        	cout<<"Thread "<<tid<<" is created"<<endl;
        	threads[i] = tid;
    	}

    	for (int i = 0; i < thread_count; i++) {
    		cout<<"Main Thread joining on Thread "<<threads[i]<<endl;
        	uthread_join(threads[i], NULL);
        	
    	}
    	
	auto end_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> secs = end_time-start_time;
    	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(secs);
    	
    	cout << ms.count() <<"count";
    	cout << "aaaaaa";
    	delete[] threads;
	return ms.count();
}

void test_spinlock_vs_lock(){
	ofstream spin_lock_file("spin_lock_file.txt");
	ofstream lock_file("lock_file.txt");
	for(int i=1; i<=6000;i=i+100){
		int a = spinlock_helper(i);
		cout<< a;
		string s = to_string(i)+" "+to_string(a)+"\n";
		spin_lock_file << s;
		a = lock_helper(i);
		cout<< a;
		string s1 = to_string(i)+" "+to_string(a)+"\n";
		lock_file << s1;
	}

  	spin_lock_file.close();
  	lock_file.close();
	
}

int main(int argc, char *argv[]) {
  	
	test_spinlock_vs_lock();
	return 0;
}
