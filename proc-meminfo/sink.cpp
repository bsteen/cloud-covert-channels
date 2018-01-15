// sink.cpp
#include "meminfo.hpp"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

vector<bool> data;              // Recoreded data from the channel
unsigned long null_value = 0;   // A copy of base_mem_free from meminfo.cpp. A FreeMem 
unsigned long zero = 0;         // MemFree used to represent a zero during transmission
unsigned long one = 0;          // MemFree used to represent a one during transmission

// Find FreeMem values that will represent null, zero, and one
void setup_channel(){
    cout << "Starting sink program...\nRunning calibration (" << CALIB_TIME << ")" << endl;
    
    null_value = do_channel_calibartion();
    zero = null_value - LOW_BIT_ALLOC;
    one = null_value - HIGH_BIT_ALLOC;

    if(zero <= 0 || one <= 0){
        cout << "System does not have enough free memory for the channel to run!" << endl;
        exit(1);
    }

    cout << "Calibration complete. Null value is represented with " << null_value
    << "\n Zero value is represented with " << zero << "\n One value is represented with " << one << endl;

    return;
}

// Record values of FreeMem for later analysis
void record_transmission(){
    cout << "Recording source transmission now..." << endl;

    struct timespec start, current;
    unsigned long elapsed_nano_sec = 0;
    unsigned long record_time = CHANNEL_TIME * 1000000000UL;

    while(elapsed_nano_sec < record_time){
        record_trans_reading();     // Record current value of FreeMem
        usleep(RECORD_DELAY);       // Wait this many microseconds before recording again
    }

    cout << "Done recording transmission." << endl;
    return;
}

// Analyses recorded data to find the source's starting sequence
// Returns the index in trans_readings that points to the start of the source's transmission
// -1 is returned if the sequence isn't found
int find_start_index(){
    cout << "Looking for source's start sequence in recorded data..." << endl;

    //  IMPLEMENT LOGIC

    cout << "Could not find source's start sequence!" << endl;
    return -1;
}

void decode_transmission(int index){
    //  IMPLEMENT LOGIC
    return;
}

int main(){
    setup_channel();
    record_transmission();
    int index = find_start_index();

    if(index == -1){
        
        exit(1);
    }
    else{
        decode_transmission(index);
    }

    return 0;
}