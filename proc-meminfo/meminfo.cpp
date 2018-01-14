// meminfo.cpp
// Interface functions used by source.cpp and sink.cpp to read from /proc/meminfo

#include "meminfo.hpp"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <vector>

unsigned long mem_free = 0;         // Amount of RAM in kB left unused by the system
unsigned long base_mem_free = 0;    // The average (baseline) MemFree value calculated before the channel begins transmitting; Used as the "null" value during transmission
                                    // with null being neither 0 or 1

vector<unsigned long> trans_readings;       // Transmission readings; Values read from MemFree line of /proc/meminfo during channel transmission
vector<unsigned long> calib_readings;       // Calibration readings; Recorded values of MemFree before transmission starts; used for calculating base_mem_free

unsigned long get_mem_free(){
    return mem_free;
}

unsigned long get_base_mem_free(){
    return base_mem_free;
}

// Returns the kB value the MemFree line from /proc/meminfo;
// e.g.: takes in "MemFree:        11528160 kB" and returns "11528160"
string extract_mem_val(string line){
    int start = 0;
    int end = 0;

    // Removed the "MemFree:" part of the string
    while(line.at(start) != ' '){
        start++;
    }

    // Removed the spaces between "MemFree:" and the kB value
    while(line.at(start) == ' '){
        start++;
    }

    end = start;

    // Find the index of the last digit
    while(line.at(end) != ' '){
        end++;
    }

    // Return the number only
    return line.substr(start, end - start);
}

// Get the current MemFree value from /proc/meminfo and store it in mem_free
// Average execution time: 0.0001 sec (100 us)
void update_mem_free(){
    ifstream file;
    file.open("/proc/meminfo");

    if(file.is_open()){
        string line;

        getline(file, line);    // Get first line and throw it away; don't need it
        getline(file, line);    // Get the second line of /proc/meminfo; Contains MemFree value

        line = extract_mem_val(line);
        mem_free = strtoul(line.c_str(), NULL, 0);  // Convert the string to an unsigned long and update the global variable
    }
    else{
        cout << "Could not open /proc/meminfo" << endl;
    }

    return;
}

// Gets the newest value of mem_free and adds it to the vector of calib_readings
// Average execution time: 
void record_calib_reading(){
    update_mem_free();
    calib_readings.push_back(mem_free);
    return;
}

// Gets the newest value of mem_free and adds it to the vector of trans_readings
// Average execution time: 
void record_trans_reading(){
    update_mem_free();
    trans_readings.push_back(mem_free);     // POTENTIAL ISSUE: As the program runs more memory is needed to store the recordings, which will affect the value of MemFree
                                            // Fix: Instead of storing in vector, write value to file; This would be much slower, however
    return;
}

// Calculate the average (baseline) value of MemFree by find the average of the values stored in calibration
void calc_base_mem_free(){
    unsigned long sum = 0;

    for(int i = 0; i < calib_readings.size(); i++){
        sum += calib_readings[i];
    }

    base_mem_free = sum / calib_readings.size();
    return;
}

// Calculates the the baseline value of MemFree and returns the value
// Called by source and sink before the channel starts to get the "null" transmission value
unsigned long do_channel_calibartion(){
    struct timespec start, current;
    unsigned long elapsed_nano_sec = 0;
    unsigned long calib_time= CALIB_TIME * 1000000000;

    while(elapsed_nano_sec < calib_time){
        record_calib_reading();     // Take another calibation reading
        usleep(CALIB_DELAY);        // Wait this amount of microseconds before the next reading
    }

    // Calculate the baseline MemFree value and then return it
    calc_base_mem_free();
    return base_mem_free;
}

// For testing functions; not used in actual application
int main(){

    // struct timespec start, current;
    // unsigned long elapsed_nano_sec = 0;
    // unsigned long sum = 0;

    // Calculate the average time it takes for update_mem_free() to run
    // for(int i = 0; i < 10; i++){
    //         clock_gettime(CLOCK_MONOTONIC, &start);
    //         update_mem_free();
    //         clock_gettime(CLOCK_MONOTONIC, &current);

    //         elapsed_nano_sec = 1000000000 * (current.tv_sec - start.tv_sec) + current.tv_nsec - start.tv_nsec;
    //         cout << elapsed_nano_sec / 1000000000.0 << endl;
    //         sum += elapsed_nano_sec;
    //         sleep(1);
    // }
    // cout << (sum / 10)/1000000000.0 << endl;
}
