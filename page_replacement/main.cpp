/*
Main program for the virtual memory project.
Make all of your modifications to this file.
You may add or rearrange any code or data as you need.
The header files page_table.h and disk.h explain
how to use the page table and disk interfaces.
*/

#include "page_table.h"
#include "disk.h"
#include "program.h"

#include <cassert>
#include <iostream>
#include <string.h>
#include <bits/stdc++.h>
#include <fstream>

using namespace std;

// Prototype for test program
typedef void (*program_f)(char *data, int length);


// Number of physical frames
int nframes;
int page_faults, disk_reads, disk_writes;
map<int, int> frame_to_page;
queue<int> fifo_queue;
stack<int> lifo_stack;
list<pair<int,int>>l;
unordered_map<int,list<pair<int, int>>::iterator> m;
vector<int> custom_vector;
// Pointer to disk for access from handlers
struct disk *disk = nullptr;
int frame_to_remove = 0, new_frame =0;


// Simple handler for pages == frames
void page_fault_handler_example(struct page_table *pt, int page) {
	//cout << "page fault on page #" << page << endl;

	// Print the page table contents
	//cout << "Before ---------------------------" << endl;
	//page_table_print(pt);
	//cout << "----------------------------------" << endl;

	// Map the page to the same frame number and set to read/write
	// TODO - Disable exit and enable page table update for example
	exit(1);
	//page_table_set_entry(pt, page, page, PROT_READ | PROT_WRITE);

	// Print the page table contents
	//cout << "After ----------------------------" << endl;
	//page_table_print(pt);
	//cout << "----------------------------------" << endl;
}

// TODO - Handler(s) and page eviction algorithms

// random page replacement policy
void page_fault_handler_rand(struct page_table *pt, int page) {
	//cout << "page fault on page #" << page << endl;

	// Print the page table contents
	//cout << "Before ---------------------------" << endl;
	//page_table_print(pt);
	//cout << "----------------------------------" << endl;

	// Map the page to the same frame number and set to read/write
	// TODO - Disable exit and enable page table update for example
	int frame, bits;
	page_table_get_entry(pt, page, &frame, &bits);
	//checking if the page is in physical memory
	if(!(bits&PROT_READ)){
		page_faults++;
		int open_frame;
		//checking if an empty frame is available
		if(disk_reads < nframes){
			open_frame = disk_reads;
			frame_to_page[open_frame] = page;
			page_table_set_entry(pt, page, open_frame, PROT_READ);
			disk_reads++;
			disk_read(disk, page, &pt->physmem[open_frame*PAGE_SIZE]);
		}
		//selecting a random page to evict from the memory and replacing with new page
		else{
			open_frame = rand()%nframes;
			int page_to_replace = frame_to_page[open_frame];
			int f, b;
			page_table_get_entry(pt, page_to_replace, &f, &b);
			if(b&PROT_READ && b&PROT_WRITE){
				disk_write(disk, page_to_replace, &pt->physmem[open_frame*PAGE_SIZE]);
				disk_writes++;
			}
			page_table_set_entry(pt, page_to_replace, f, 0);
			frame_to_page[open_frame] = page;
			page_table_set_entry(pt, page, open_frame, PROT_READ);
			disk_reads++;
			disk_read(disk, page, &pt->physmem[open_frame*PAGE_SIZE]);
		}
		
	}
	//checking if the read bit is set and write bit is not set
	else if(!(bits&PROT_WRITE)){
		page_table_set_entry(pt, page, frame, PROT_READ|PROT_WRITE);
	}
	// Print the page table contents
	//cout << "After ----------------------------" << endl;
	//page_table_print(pt);
	//cout << "----------------------------------" << endl;
}

//following fifo page eviction policy
void page_fault_handler_fifo(struct page_table *pt, int page) {
	//cout << "page fault on page #" << page << endl;

	// Print the page table contents
	//cout << "Before ---------------------------" << endl;
	//page_table_print(pt);
	//cout << "----------------------------------" << endl;

	// Map the page to the same frame number and set to read/write
	// TODO - Disable exit and enable page table update for example
	int frame, bits;
	page_table_get_entry(pt, page, &frame, &bits);
	//checking if the page is in physical memory
	if(!(bits&PROT_READ)){
		page_faults++;
		int open_frame;
		//checking if an empty frame is available
		if(disk_reads < nframes){
			open_frame = disk_reads;
			frame_to_page[open_frame] = page;
			page_table_set_entry(pt, page, open_frame, PROT_READ);
			fifo_queue.push(open_frame);
			disk_reads++;
			disk_read(disk, page, &pt->physmem[open_frame*PAGE_SIZE]);
		}
		//evict the longest resident frame from the memory and replacing with new page
		else{
			open_frame = fifo_queue.front();
			fifo_queue.pop();   
			int page_to_replace = frame_to_page[open_frame];
			int f, b;
			page_table_get_entry(pt, page_to_replace, &f, &b);
			if(b&PROT_READ && b&PROT_WRITE){
				disk_write(disk, page_to_replace, &pt->physmem[open_frame*PAGE_SIZE]);
				disk_writes++;
			}
			page_table_set_entry(pt, page_to_replace, f, 0);
			frame_to_page[open_frame] = page;
			page_table_set_entry(pt, page, open_frame, PROT_READ);
			fifo_queue.push(open_frame);
			disk_reads++;
			disk_read(disk, page, &pt->physmem[open_frame*PAGE_SIZE]);
		}
		
	}
	//checking if the read bit is set and write bit is not set
	else if(!(bits&PROT_WRITE)){
		page_table_set_entry(pt, page, frame, PROT_READ|PROT_WRITE);
	}
	// Print the page table contents
	//cout << "After ----------------------------" << endl;
	//page_table_print(pt);
	//cout << "----------------------------------" << endl;
}

void page_fault_handler_lru(struct page_table *pt, int page) {
	//cout << "page fault on page #" << page << endl;

	// Print the page table contents
	//cout << "Before ---------------------------" << endl;
	//page_table_print(pt);
	//cout << "----------------------------------" << endl;

	// Map the page to the same frame number and set to read/write
	// TODO - Disable exit and enable page table update for example
	int frame, bits;
	page_table_get_entry(pt, page, &frame, &bits);
	//checking if the page is in physical memory
	if(!(bits&PROT_READ)){
		page_faults++;
		int open_frame;
		//checking if there is an empty frame available
		if(disk_reads < nframes){
			open_frame = disk_reads;
			frame_to_page[open_frame] = page;
			page_table_set_entry(pt, page, open_frame, PROT_READ);
            		l.push_front({open_frame,page});
            		m[open_frame]=l.begin();
			disk_reads++;
			disk_read(disk, page, &pt->physmem[open_frame*PAGE_SIZE]);
		}
		//getting the frame to 
		else{
			open_frame=l.back().first;
            		l.pop_back();
           		m.erase(open_frame);
			int page_to_replace = frame_to_page[open_frame];
			int f, b;
			page_table_get_entry(pt, page_to_replace, &f, &b);
			if(b&PROT_READ && b&PROT_WRITE){
				disk_write(disk, page_to_replace, &pt->physmem[open_frame*PAGE_SIZE]);
				disk_writes++;
			}
			page_table_set_entry(pt, page_to_replace, f, 0);
			frame_to_page[open_frame] = page;
			l.push_front({open_frame,page});
        		m[open_frame]=l.begin();
			page_table_set_entry(pt, page, open_frame, PROT_READ);
			disk_reads++;
			disk_read(disk, page, &pt->physmem[open_frame*PAGE_SIZE]);
		}
		
	}else if(!(bits&PROT_WRITE)){
		page_table_set_entry(pt, page, frame, PROT_READ|PROT_WRITE);
	}
	// Print the page table contents
	//cout << "After ----------------------------" << endl;
	//page_table_print(pt);
	//cout << "----------------------------------" << endl;
}

void page_fault_handler_lifo(struct page_table *pt, int page) {
	//cout << "page fault on page #" << page << endl;

	// Print the page table contents
	//cout << "Before ---------------------------" << endl;
	//page_table_print(pt);
	//cout << "----------------------------------" << endl;

	// Map the page to the same frame number and set to read/write
	// TODO - Disable exit and enable page table update for example
	int frame, bits;
	page_table_get_entry(pt, page, &frame, &bits);
	if(!(bits&PROT_READ)){
		page_faults++;
		int open_frame;
		if(disk_reads < nframes){
			open_frame = disk_reads;
			frame_to_page[open_frame] = page;
			page_table_set_entry(pt, page, open_frame, PROT_READ);
			lifo_stack.push(open_frame);
			disk_reads++;
			disk_read(disk, page, &pt->physmem[open_frame*PAGE_SIZE]);
		}
		else{
			open_frame = lifo_stack.top();
			lifo_stack.pop();
			int page_to_replace = frame_to_page[open_frame];
			int f, b;
			page_table_get_entry(pt, page_to_replace, &f, &b);
			if(b&PROT_READ && b&PROT_WRITE){
				disk_write(disk, page_to_replace, &pt->physmem[open_frame*PAGE_SIZE]);
				disk_writes++;
			}
			page_table_set_entry(pt, page_to_replace, f, 0);
			frame_to_page[open_frame] = page;
			page_table_set_entry(pt, page, open_frame, PROT_READ);
			lifo_stack.push(open_frame);
			disk_reads++;
			disk_read(disk, page, &pt->physmem[open_frame*PAGE_SIZE]);
		}
		
	}else if(!(bits&PROT_WRITE)){
		page_table_set_entry(pt, page, frame, PROT_READ|PROT_WRITE);
	}
	// Print the page table contents
	//cout << "After ----------------------------" << endl;
	//page_table_print(pt);
	//cout << "----------------------------------" << endl;
}

//custom page replacement policy, removing the write set pages first
void page_fault_handler_custom(struct page_table *pt, int page) {
	//cout << "page fault on page #" << page << endl;

	// Print the page table contents
	//cout << "Before ---------------------------" << endl;
	//page_table_print(pt);
	//cout << "----------------------------------" << endl;

	// Map the page to the same frame number and set to read/write
	// TODO - Disable exit and enable page table update for example
	int frame, bits;
	frame_to_remove =0;
	frame_to_remove = rand()%nframes;
	page_table_get_entry(pt, page, &frame, &bits);
	//checking if the page is in physical memory
	if(!(bits&PROT_READ)){
		page_faults++;
		int open_frame;
		//checking if an empty frame is available
		if(disk_reads < nframes){
			open_frame = disk_reads;
			frame_to_page[open_frame] = page;
			page_table_set_entry(pt, page, open_frame, PROT_READ);
			custom_vector.push_back(open_frame);
			disk_reads++;
			disk_read(disk, page, &pt->physmem[open_frame*PAGE_SIZE]);
		}
		//finding the page to evict
		else{
			bool found = false;
			vector<int> write_pages;
			//checking and getting the pages with write bit set
			for(auto i = custom_vector.begin(); i != custom_vector.end(); i++){
				int frame_no = *i;
				int page_to_replace = frame_to_page[frame_no];
				int f, b;
				page_table_get_entry(pt, page_to_replace, &f, &b);
				
				if(b&PROT_READ && b&PROT_WRITE){
					write_pages.push_back(frame_no);
					//open_frame = frame_no;
					//custom_vector.erase(i);
					found = true;
					//break;
				}
			}
			// if there are no pages with write bit set, then evict a random page from the pages in the memory
			if(!found){
				auto i = find(custom_vector.begin(),custom_vector.end(),frame_to_remove);
				open_frame = frame_to_remove;
				custom_vector.erase(i);
			}
			// evict a random page from pages with write bit set 
			else {
				open_frame = write_pages[rand()%write_pages.size()];
				auto i = find(custom_vector.begin(),custom_vector.end(),open_frame);
				custom_vector.erase(i);
			}
			
			int page_to_replace = frame_to_page[open_frame];
			int f, b;
			page_table_get_entry(pt, page_to_replace, &f, &b);
			if(b&PROT_READ && b&PROT_WRITE){
				disk_write(disk, page_to_replace, &pt->physmem[open_frame*PAGE_SIZE]);
				disk_writes++;
			}
			page_table_set_entry(pt, page_to_replace, f, 0);
			frame_to_page[open_frame] = page;
			page_table_set_entry(pt, page, open_frame, PROT_READ);
			custom_vector.push_back(open_frame);
			disk_reads++;
			disk_read(disk, page, &pt->physmem[open_frame*PAGE_SIZE]);
		}
		
	}
	//checking if the read bit is set and write bit is not set
	else if(!(bits&PROT_WRITE)){
		page_table_set_entry(pt, page, frame, PROT_READ|PROT_WRITE);
	}
	new_frame = (frame_to_remove+1)%nframes;
	// Print the page table contents
	//cout << "After ----------------------------" << endl;
	//page_table_print(pt);
	//cout << "----------------------------------" << endl;
}

int testing(char* arg1, char* arg2) {
	char *algorithm = arg1;
	char *program_name = arg2;

	// Validate the algorithm specified
	if ((strcmp(algorithm, "rand") != 0) &&
	    (strcmp(algorithm, "fifo") != 0) &&
	    (strcmp(algorithm, "custom") != 0)) {
		cerr << "ERROR: Unknown algorithm: " << algorithm << endl;
		exit(1);
	}

	// Validate the program specified
	program_f program = NULL;
	if (!strcmp(program_name, "sort")) {
		program = sort_program;
	}
	else if (!strcmp(program_name, "scan")) {
		program = scan_program;
	}
	else if (!strcmp(program_name, "focus")) {
		program = focus_program;
	}
	else {
		cerr << "ERROR: Unknown program: " << program_name << endl;
		exit(1);
	}

	// TODO - Any init needed
	page_fault_handler_t page_fault_handler;
	if (!strcmp(algorithm, "rand")) {

		page_fault_handler = page_fault_handler_rand;
	}
	else if (!strcmp(algorithm, "fifo")) {
		page_fault_handler = page_fault_handler_fifo;
	}
	else if (!strcmp(algorithm, "custom")) {
		page_fault_handler = page_fault_handler_custom;
	}
	else {
		cerr << "ERROR: Unknown algorithm: " << algorithm << endl;
		exit(1);
	}
	string algo_name(algorithm);
	string prog_name(program_name);
	string file = algo_name + "_" + prog_name+".txt";
	ofstream lock_file(file);
	for(int i=2;i<=101;i++){
		nframes = i;
		int npages = 100;
		page_faults = 0;
		disk_reads = 0;
		disk_writes = 0;
		disk = nullptr;
		frame_to_page.clear();
		fifo_queue =  queue<int>();
		lifo_stack = stack<int>();
		l.clear();
		m.clear();
		custom_vector.clear();
		frame_to_remove = 0;
		new_frame = 0;
		disk = disk_open("myvirtualdisk", npages);
		if (!disk) {
			cerr << "ERROR: Couldn't create virtual disk: " << strerror(errno) << endl;
			return 1;
		}

		// Create a page table
		struct page_table *pt = page_table_create(npages, nframes, page_fault_handler /* TODO - Replace with your handler(s)*/);
		if (!pt) {
			cerr << "ERROR: Couldn't create page table: " << strerror(errno) << endl;
			return 1;
		}

		// Run the specified program
		char *virtmem = page_table_get_virtmem(pt);
		program(virtmem, npages * PAGE_SIZE);
		cout<<"Number of Frames: "<<nframes<<"\n";
		cout<<"Number of Page Faults: "<<page_faults<<"\n";
		cout<<"Number of Disk Reads: "<<disk_reads<<"\n";
		cout<<"Number of Disk Writes: "<<disk_writes<<"\n";
		cout<<"\n";
		string s1 = to_string(nframes)+" "+to_string(page_faults)+" "+to_string(disk_reads)+" "+to_string(disk_writes)+"\n";
		lock_file << s1;
		// Clean up the page table and disk
		page_table_delete(pt);
		disk_close(disk);
	}
	// Create a virtual disk
	return 0;
}

int main(int argc, char *argv[]) {
	// Check argument count
	if(argc == 3){
		
		testing(argv[1], argv[2]);
		return 0;
	}
	if (argc != 5) {
		cerr << "Usage: virtmem <npages> <nframes> <rand|fifo|lru> <sort|scan|focus>" << endl;
		exit(1);
	}

	// Parse command line arguments
	int npages = atoi(argv[1]);
	nframes = atoi(argv[2]);
	const char *algorithm = argv[3];
	const char *program_name = argv[4];

	// Validate the algorithm specified
	if ((strcmp(algorithm, "rand") != 0) &&
	    (strcmp(algorithm, "fifo") != 0) &&
	    (strcmp(algorithm, "custom") != 0)) {
		cerr << "ERROR: Unknown algorithm: " << algorithm << endl;
		exit(1);
	}

	// Validate the program specified
	program_f program = NULL;
	if (!strcmp(program_name, "sort")) {
		if (nframes < 2) {
			cerr << "ERROR: nFrames >= 2 for sort program" << endl;
			exit(1);
		}

		program = sort_program;
	}
	else if (!strcmp(program_name, "scan")) {
		program = scan_program;
	}
	else if (!strcmp(program_name, "focus")) {
		program = focus_program;
	}
	else {
		cerr << "ERROR: Unknown program: " << program_name << endl;
		exit(1);
	}

	// TODO - Any init needed
	page_fault_handler_t page_fault_handler;
	if (!strcmp(algorithm, "rand")) {

		page_fault_handler = page_fault_handler_rand;
	}
	else if (!strcmp(algorithm, "fifo")) {
		page_fault_handler = page_fault_handler_fifo;
	}
	else if (!strcmp(algorithm, "custom")) {
		page_fault_handler = page_fault_handler_custom;
	}
	else {
		cerr << "ERROR: Unknown algorithm: " << algorithm << endl;
		exit(1);
	}
	
	// Create a virtual disk
	disk = disk_open("myvirtualdisk", npages);
	if (!disk) {
		cerr << "ERROR: Couldn't create virtual disk: " << strerror(errno) << endl;
		return 1;
	}

	// Create a page table
	struct page_table *pt = page_table_create(npages, nframes, page_fault_handler /* TODO - Replace with your handler(s)*/);
	if (!pt) {
		cerr << "ERROR: Couldn't create page table: " << strerror(errno) << endl;
		return 1;
	}

	// Run the specified program
	char *virtmem = page_table_get_virtmem(pt);
	program(virtmem, npages * PAGE_SIZE);
	
	cout<<"Number of Page Faults: "<<page_faults<<"\n";
	cout<<"Number of Disk Reads: "<<disk_reads<<"\n";
	cout<<"Number of Disk Writes: "<<disk_writes<<"\n";
	
	// Clean up the page table and disk
	page_table_delete(pt);
	disk_close(disk);

	return 0;
}
