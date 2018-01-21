// sink.cpp
#include "meminfo.hpp"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

vector<int> data;				// Recoreded data from the channel

unsigned long NULL_value = 0;   // A copy of base_mem_free from meminfo.cpp. MemFree value used to represent a null value during transmission 
unsigned long ZERO = 0;		 	// MemFree value used to represent a zero during transmission
unsigned long ONE = 0;		  	// MemFree value used to represent a one during transmission

unsigned long ZERO_UPPER = 0;	// Range of values that will be considered a 1 or 0
unsigned long ZERO_LOWER = 0;
unsigned long ONE_UPPER = 0;
unsigned long ONE_LOWER = 0;

// Find FreeMem values that will represent null, zero, and one
void setup_channel(){
	cout << "Running channel calibration (" << CALIB_TIME << " seconds)..." << endl;

	NULL_value = do_channel_calibartion();	// Run the channel calibration function
	ZERO = NULL_value - LOW_BIT_ALLOC;
	ONE = NULL_value - HIGH_BIT_ALLOC;

	unsigned long var = (HIGH_BIT_ALLOC - LOW_BIT_ALLOC) * DETECT_VARIANCE;

	ZERO_LOWER = ZERO - (var);
	ZERO_UPPER = ZERO + (var);
	ONE_LOWER = ONE - (var);
	ONE_UPPER = ONE + (var);

	if(ZERO_LOWER <= 0){
		cout << "System does not have enough free memory for the channel to run!" << endl;
		cout << "END SINK PROGRAM" << endl;
		exit(1);
	}

	printf("Calibration complete:\n\tNull value will be represented with %lu\n", NULL_value);
	printf("\tZero value is represented with %lu [%lu, %lu]\n\tOne value is represented with %lu [%lu, %lu]\n", ZERO, ZERO_LOWER, ZERO_UPPER, ONE, ONE_LOWER, ONE_UPPER);

	return;
}

// Record value from meminfo for later analysis
void record_transmission(){
	cout << "Recording source transmission now (" << CHANNEL_TIME << " seconds)..." << endl;

	struct timespec start, current;
	unsigned long elapsed_nano_sec = 0;
	unsigned long record_time = CHANNEL_TIME * 1000000000UL;

	clock_gettime(CLOCK_MONOTONIC, &start);

	while(elapsed_nano_sec < record_time){
		record_trans_reading();		// Record current value of FreeMem
		usleep(RECORD_DELAY);		// Wait this many microseconds before recording again

		clock_gettime(CLOCK_MONOTONIC, &current);
        elapsed_nano_sec = 1000000000UL * (current.tv_sec - start.tv_sec) + current.tv_nsec - start.tv_nsec;
	}

	cout << "\tDone recording transmission." << endl;
	return;
}

// Converts the raw MemFree kB values stored trans_readings into 1's and 0's
// A "null value" was be found between a bit for it to be recognized
// This prevents two simultaneous bits with the same value from being seen as one bit
// Stores these values in the vector "data"
// TO IMPLEMENT: Use NUM_CONSEC_VAL to establish a "hold time" for 1's and 0's
//				 This feature could reduce transmission errors from noise, but would also increase transmission loss rate
void convert_transmission(){
	cout << "Coverting recorded meminfo values into 1's and 0's..." << endl;
	vector<unsigned long> trans_readings = get_trans_readings();	// Copy over raw meminfo readings

	int zero_confirms = 0;	// NOT IMPLEMENTED YET
	int one_confirms = 0;	// NOT IMPLEMENTED YET
	bool null_found = true;

	// Because this channel uses MemFree (amount of available memory left), NULL_value will the be the largest of the 3 numbers, ZERO will be smaller
	// than NULL_value and larger than ONE, and ONE will be the smallest of the three.
	for(int i = 0; i < trans_readings.size(); i++){
		if(ONE_LOWER <= trans_readings[i] && trans_readings[i] <= ONE_UPPER && null_found){
			cout << "Found 1 @" << trans_readings[i] << " kb" << endl;
			data.push_back(1);
			null_found = false;
		}
		else if(trans_readings[i] <= ZERO && null_found){
			cout << "Found 0 @" << trans_readings[i] << " kb" << endl;
			data.push_back(0);
			null_found = false;
		}
		else{
			// Represents a null value (An "idle" value in between transmission of a 1 or 0)
			// cout << "Found null @" << trans_readings[i] << " kb" << endl;
			null_found = true;
		}
	}

	cout << "\tDone conversion." << endl;
	return;
}

// Helper function used by find_start_index()
// Compares source_sequence with a sequence from data using an offset index
// Does not do out-of-bounds-checking on the data vector; Assumes caller does this check
bool does_seq_match(int offset_index){
	vector<int> source_sequence = get_source_sequence();

	for(int i = 0; i < source_sequence.size(); i++){
		if(source_sequence[i] != data[offset_index + i]){
			return false;
		}
	}
	
	return true;
}

// Analyses recorded data to find the source's starting sequence
// Returns the index in trans_readings that points to the start of the source's transmission
// -1 is returned if the sequence isn't found
int find_start_index(){
	cout << "Looking for source's start sequence in recorded data..." << endl;
	vector<int> source_sequence = get_source_sequence();

	for(int i = 0; i < data.size(); i++){
		if(source_sequence.size() + i > data.size()){
			break;	// The data vector can not contain any more potentially matching sequences
		}
		else if(does_seq_match(i)){
			cout << "\tFound starting sequence!" << endl;
			return i;
		}
	}

	cout << "\tCould not find source's start sequence!" << endl;
	return -1;
}

// Prints out the data from the transmission in 8 bit chunks
// This is all ones and zeros starting right after the source's start sequence
void readout_data(int index){
	cout << "Reading out received data now..." << endl;

	int newline_count = 0;

	for(int i = index; i < data.size(); i++){
		cout << data[i];
		newline_count++;

		if(newline_count >= 8){
			cout << endl;
			newline_count = 0;
		}
	}

	cout << endl << "Done reading out data." << endl;
	return;
}


int main(){
	cout << "START SINK PROGRAM" << endl;
	
	setup_channel();
	record_transmission();
	convert_transmission();

	int index = find_start_index();

	if(index == -1){
		cout << "END SINK PROGRAM" << endl;
		exit(1);
	}
	else{
		readout_data(index);
	}

	cout << "END SINK PROGRAM" << endl;
	return 0;
}