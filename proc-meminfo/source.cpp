// source.cpp
#include "meminfo.hpp"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

unsigned long NULL_value = 0;	// A copy of base_mem_free from meminfo.cpp. MemFree value used to represent a null value during transmission
unsigned long ZERO = 0;			// MemFree value used to represent a zero during transmission
unsigned long ONE = 0;			// MemFree value used to represent a one during transmission

unsigned int hold_time = SEND_DELAY / 2;	// How many microseconds the source transmit a bit or "hold" a value in memory
											// Also the amount of time a null value will be transmitted

// Find FreeMem values that will represent null, zero, and one
void setup_channel(){
	cout << "Running channel calibration (" << CALIB_TIME << " seconds)..." << endl;

	NULL_value = do_channel_calibartion();	// Run the channel calibration function
	ZERO = NULL_value - LOW_BIT_ALLOC;
	ONE = NULL_value - HIGH_BIT_ALLOC;

	if(ZERO <= 0 || ONE <= 0){
		cout << "System does not have enough free memory for the channel to run!" << endl;
		cout << "END SOURCE PROGRAM" << endl;
		exit(1);
	}

	cout << "Hold time for bit is " << hold_time * 0.000001 << " sec" << endl;

	// cout << "Calibration complete:\n\tNull value will be represented with " << NULL_value
	// << "\n\tZero value is represented with " << ZERO << "\n\tOne value is represented with " << ONE << endl;

	return;
}

// "Send" a bit by allocating a specific amount of memory, holding the allocation, and then freeing it
void send_bit(int bit){
	unsigned int alloc_amount = 0;
	void *memory_ptr;

	if(bit == 1){
		alloc_amount = HIGH_BIT_ALLOC * 1024;
	}
	else{
		alloc_amount = LOW_BIT_ALLOC * 1024;
	}

	memory_ptr = malloc(alloc_amount);

	if(memory_ptr == NULL){
		cout << "Error; Could not allocation memory (" << alloc_amount << " bytes)" << endl;
	}
	else{
		usleep(hold_time);	// Time when the sink is detecting a bit
		free(memory_ptr);
		usleep(hold_time);	// Time when the sink is detecting a null value
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
	cout << "Source sending data now..." << endl;

	int bit = 1;

	for(int i = 0; i < 100; i++){
		send_bit(bit);
		bit = !bit;
	}

	cout << "\tDone sending transmission." << endl;
	return;

}

int main(){
	cout << "START SOURCE PROGRAM" << endl;
	struct timespec start, current;
	unsigned long elapsed_nano_sec = 0;
	unsigned long record_time = CHANNEL_TIME * 1000000000UL;

	setup_channel();

	clock_gettime(CLOCK_MONOTONIC, &start);
	send_start_seq();
	// send_data();
	clock_gettime(CLOCK_MONOTONIC, &current);

	elapsed_nano_sec = 1000000000UL * (current.tv_sec - start.tv_sec) + current.tv_nsec - start.tv_nsec;

	if(elapsed_nano_sec >= record_time){
		cout << "\tChannel expired before source finished transmission!" << endl;
	}
	cout << "Source took " << elapsed_nano_sec / 1000000000.0  << " seconds to transmit data" << endl;

	cout << "END SOURCE PROGRAM" << endl;
	return 0;
}