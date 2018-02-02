// source.cpp
#include "meminfo.hpp"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_BITS_TO_SEND 10		// Number of data bits the source will to the sink, NOT including the start sequence

unsigned long NULL_value = 0;	// A copy of base_mem_free from meminfo.cpp. MemFree value used to represent a null value during transmission
unsigned long ZERO = 0;			// MemFree value used to represent a zero during transmission
unsigned long ONE = 0;			// MemFree value used to represent a one during transmission

// Find FreeMem values that will represent null, zero, and one
void setup_channel(){
	cout << "Running channel calibration (" << CALIB_TIME << " second(s))..." << endl;

	NULL_value = do_channel_calibartion();	// Run the channel calibration function
	ZERO = NULL_value - LOW_BIT_ALLOC;
	ONE = NULL_value - HIGH_BIT_ALLOC;

	if(ZERO <= 0 || ONE <= 0){
		cout << "System does not have enough free memory for the channel to run!" << endl;
		cout << "END SOURCE PROGRAM" << endl;
		exit(1);
	}

	cout << "\tHold time for a bit is " << HOLD_TIME * 0.000001 << " seconds" << endl << "Calibration complete." << endl;

	return;
}

// "Send" a bit by allocating a specific amount of memory, holding the allocation, and then freeing it
void send_bit(int bit){
	unsigned int alloc_amount = 0;
	void *memory_ptr;

	if(bit == 1){
		alloc_amount = HIGH_BIT_ALLOC * 1000;
	}
	else{
		alloc_amount = LOW_BIT_ALLOC * 1000;
	}

	memory_ptr = malloc(alloc_amount);

	if(memory_ptr == NULL){
		cout << "Error; Could not allocation memory (" << alloc_amount << " bytes)" << endl;
	}
	else{
		memset(memory_ptr, 'a', alloc_amount);	// Write to memory so system displays it as in use
		usleep(HOLD_TIME);	// Time when the sink is detecting a bit
		free(memory_ptr);
		usleep(HOLD_TIME);	// Time when the sink is detecting a null value
	}

	return;
}

// Sequence of bits that precedes the actual data
// Lets the sink identify the source's transmission
void send_start_seq(){
	cout << "Source sending start sequence now..." << endl;

	vector<int> source_sequence = get_source_sequence();
	for(int i = 0; i < source_sequence.size(); i++){
		send_bit(source_sequence[i]);
	}

	cout << "\tDone sending start sequence." << endl;
	return;
}

// Send data to sink by allocating system memory
// Source must send all of its data within CHANNEL_TIME seconds
// Right now, the source just sends an alternating pattern of 1's and 0's
void send_data(){
	cout << "Source sending data now..." << endl << "\t";

	int bit = 0;
	for(int i = 0; i < NUM_BITS_TO_SEND; i++){
		cout << bit;
		send_bit(bit);
		bit = !bit;
	}

	cout << endl << "\tDone sending transmission." << endl;
	return;

}

int main(){
	cout << "START SOURCE PROGRAM" << endl;
	struct timespec start, current;
	unsigned long elapsed_nano_sec = 0;
	unsigned long record_time = CHANNEL_TIME * 1000000000UL;

	setup_channel();
	cout << "Channel is set to be active for " << CHANNEL_TIME << " second(s)." << endl;

	clock_gettime(CLOCK_MONOTONIC, &start);
	send_start_seq();
	send_data();
	clock_gettime(CLOCK_MONOTONIC, &current);

	elapsed_nano_sec = 1000000000UL * (current.tv_sec - start.tv_sec) + current.tv_nsec - start.tv_nsec;

	if(elapsed_nano_sec >= record_time){
		cout << "ERROR: Channel expired before source finished transmission!" << endl;
	}
	
	cout << "Source transfer statistics:" << endl
	<< "\t" << elapsed_nano_sec / 1000000000.0  << " seconds to complete transmission" << endl
	<< "\t" << NUM_BITS_TO_SEND + get_source_sequence().size() << " bits transfered" << endl
	<< "\t" << (NUM_BITS_TO_SEND + get_source_sequence().size()) / (elapsed_nano_sec / 1000000000.0) << " bits/second" << endl;

	cout << "END SOURCE PROGRAM" << endl;
	return 0;
}