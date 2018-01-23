// sink.cpp
#include "meminfo.hpp"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

vector<int> data;				// Recoreded data from the channel
unsigned long NULL_value = 0;   // A copy of base_mem_free from meminfo.cpp. MemFree value used to represent a null value during transmission
unsigned long ZERO = 0;		 	// MemFree value used to represent a zero during transmission
unsigned long ONE = 0;		  	// MemFree value used to represent a one during transmission

// Used for range of values that will be considered a 1 or 0
unsigned long ZERO_UPPER_LIMIT = 0;		// The largest FreeMem value that will be considered a 0
unsigned long ONE_UPPER_LIMIT = 0;		// The largest FreeMem value that will be considered a 1

// Find FreeMem values that will represent null, zero, and one
void setup_channel(){
	cout << "Running channel calibration (" << CALIB_TIME << " seconds)..." << endl;

	NULL_value = do_channel_calibartion();	// Run the channel calibration function
	ZERO = NULL_value - LOW_BIT_ALLOC;
	ONE = NULL_value - HIGH_BIT_ALLOC;

	// DETECT_VARIANCE allows for some leniency in the detection of 1s and 0s
	// If DETECT_VARIANCE is zero then ZERO_UPPER_LIMIT = ZERO and ONE_UPPER_LIMIT = ONE
	unsigned long var = (HIGH_BIT_ALLOC - LOW_BIT_ALLOC) * DETECT_VARIANCE;
	ZERO_UPPER_LIMIT = ZERO + (var);
	ONE_UPPER_LIMIT = ONE + (var);

	if(ONE <= 0){
		cout << "System does not have enough free memory for the channel to run!" << endl;
		cout << "END SINK PROGRAM" << endl;
		exit(1);
	}

	printf("Calibration complete:\n\tNull value will be represented with >%lu\n", ZERO_UPPER_LIMIT);
	printf("\tZero value is represented with [%lu, %lu]\n\tOne value is represented with [0, %lu]\n", ONE_UPPER_LIMIT + 1, ZERO_UPPER_LIMIT, ONE_UPPER_LIMIT);

	return;
}

// Record FreeMem value from /proc/meminfo for later analysis
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

// Runs after write_out_raw_readings() and convert_transmission() have outputted their data
// to text files. Runs the python script that plots the channel data
// Only runs when the sink is passed "-p" as an argument
void plot_data(){
	cout << "Creating plot from data..." << endl;

	if(system("python3 plot.py") == 0){
		cout << "\tDone creating plot." << endl;
	}
	else{
		cout << "\tError creating plot!" << endl;
	}

	return;
}

// Write out the contents of trans_readings to a text file
// Output file format has first 2 lines as threshold values; following numbers are the recordings
void write_out_raw_readings(){
	cout << "Writing our recorded data..." << endl;
	ofstream output_file;
	output_file.open("output/FreeMem_values.txt", ios::trunc);
	vector<unsigned long> trans_readings = get_trans_readings();	// Copy over raw meminfo readings

	// First 2 lines are the threshold values
	output_file << ZERO_UPPER_LIMIT << endl;
	output_file << ONE_UPPER_LIMIT << endl;

	for(int i = 0; i < trans_readings.size(); i++){
		output_file << trans_readings[i] << endl;
	}

	output_file.close();
	cout << "\tDone writing out data." << endl;

	return;
}

// Converts the raw MemFree values stored trans_readings into 1's and 0's
// A "null value" must be found between a transmission and the next one for it to be recognized
// This prevents two simultaneous bits from being seen as tranmission or double counted
// Stores these values in the vector "data"
// If "write_out" is true, the indexes where 1's and 0's are detected will be written out to a file with the following format: <1 or 0> <index> <FreeMem value>
// This ouput data is used to plot the channel data
// TO IMPLEMENT: Use NUM_CONSEC_VAL to establish a "hold time" for 1's and 0's
//				 This feature could potentially reduce transmission errors from noise, but would also increase transmission loss rate
void convert_transmission(bool write_out){
	cout << "Converting recorded meminfo values into 1's and 0's..." << endl;
	vector<unsigned long> trans_readings = get_trans_readings();	// Copy over raw meminfo readings

	int zero_confirms = 0;	// NOT IMPLEMENTED YET
	int one_confirms = 0;	// NOT IMPLEMENTED YET
	bool null_found = true;

	ofstream indexes;
	if(write_out){
		cout << "\tAlso writing out detected value indexes..." << endl;
		indexes.open("output/One-Zero_indexes.txt", ios::trunc);
	}

	// Because this channel uses MemFree (amount of available memory left), NULL_value will the be the largest of the 3 numbers, ZERO will be smaller
	// than NULL_value and larger than ONE, and ONE will be the smallest of the three.
	for(int i = 0; i < trans_readings.size(); i++){
		if(trans_readings[i] <= ONE_UPPER_LIMIT && null_found){
			printf("\tFound 1 @%d (%lu kb)\n", i, trans_readings[i]);
			data.push_back(1);
			null_found = false;

			if(write_out){
				indexes << 1 << " " << i << " " << trans_readings[i] << endl;
			}
		}
		else if(trans_readings[i] <= ZERO_UPPER_LIMIT && null_found){
			printf("\tFound 0 @%d (%lu kb)\n", i, trans_readings[i]);
			data.push_back(0);
			null_found = false;

			if(write_out){
				indexes << 0 << " " << i << " " << trans_readings[i] << endl;
			}
		}
		else if(trans_readings[i] > ZERO_UPPER_LIMIT){ // Represents a null value (An "idle" value in between transmission of a 1 or 0)
			printf("\tFound null @%d (%lu kb)\n", i, trans_readings[i]);
			null_found = true;
		}
		// else{
		// 	// This iteration contains duplicate information; Ignore it
		// }
	}

	if(write_out){
		indexes.close();
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
	int source_sequence_size = get_source_sequence().size();

	for(int i = 0; i < data.size(); i++){
		if(source_sequence_size + i > data.size()){
			break;	// The data vector can not contain any more potentially matching sequences
		}
		else if(does_seq_match(i)){
			cout << "\tFound starting sequence!" << endl;
			return i + source_sequence_size;
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


int main(int argc, char* argv[]){
	cout << "START SINK PROGRAM" << endl;

	setup_channel();
	record_transmission();

	// Output the raw recordings to a text file and make a plot
	if(argc == 2 && strcmp(argv[1], "-p") == 0){
		write_out_raw_readings();
		convert_transmission(true);
		plot_data();
	}
	else{
		convert_transmission(false);
	}

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
