// meminfo.hpp
// Defines interface functions in meminfo.c used by source.c and sink.c
// Defines costants used by meminfo.c, source.c, and sink.c
#ifndef MEMINFO_HPP
#define MEMINFO_HPP

#include <string>
#include <vector>
using namespace std;

#define HIGH_BIT_ALLOC 200000   // Number of kB below baseline FreeMem for sink to detect a '1'
#define LOW_BIT_ALLOC 100000    // Number of kB below FreeMem usage for sink to detect a '0'
#define EXTRA_ALLOC_PER 10		// Extra amount of memory to alloc to compenstate for noise; A percentage of the HIGH/LOW_BIT_ALLOC
#define CHANNEL_TIME  10        // Number of seconds the channel will be active; The channel is active when the sink is recording data
#define CALIB_TIME 1            // Amount of seconds source/sink will spend calculating the average baseline memory usage before starting the channel
#define CALIB_DELAY 100000      // Number of microseconds between each calibration recording; A smaller number means more calibration recordings will be made
#define RECORD_DELAY 1000       // Number of microseconds between sink's recordings; A smaller number means sink will record more values of FreeMem
								// This number must be smaller than SEND_DELAY, or the sink WILL miss source transmissions
#define SEND_DELAY 500000       // Number of microseconds between source's transmissions; A smaller number means source will send bits faster
								// This value corresponds to how long the source keeps a piece of memory allocated
#define NUM_CONSEC_VAL 2        // Number of consecutive reading of the same type (1, 0, or null) that need to occur for a value to be recognized.

vector<unsigned long> get_trans_readings(); //Used by sink
vector<int> get_source_sequence();      // Used by source and sink
string extract_mem_val(string line);    // Used internally meminfo.cpp
void update_mem_free();                 // Used internally meminfo.cpp
void record_calib_reading();            // Used internally meminfo.cpp
void record_trans_reading();            // Used by sink
void calc_base_mem_free();              // Used internally meminfo.cpp
unsigned long do_channel_calibartion(); // Used by source and sink

#endif