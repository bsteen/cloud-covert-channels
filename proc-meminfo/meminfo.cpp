// meminfo.cpp
// Interface functions used by source.c and sink.c to read from /proc/meminfo.c

#include "meminfo.hpp"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <unistd.h>
using namespace std;

unsigned long mem_free = 0;     // Amount of RAM in kB left unused by the system

unsigned long get_mem_free(){
    return mem_free;
}

// Returns the kB value of line; e.g.: "MemFree:        11528160 kB" => "11528160"
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
// Takes, on average 0.0001 sec (100 us) to run
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