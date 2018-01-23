// meminfo.hpp
// Defines interface functions in meminfo.c used by source.c and sink.c
// Defines costants used by meminfo.c, source.c, and sink.c
#ifndef MEMINFO_HPP
#define MEMINFO_HPP

#include <string>
#include <vector>
using namespace std;

#define HIGH_BIT_ALLOC 200000	// Number of kB below baseline FreeMem for sink to detect a '1'
#define LOW_BIT_ALLOC 100000	// Number of kB below FreeMem usage for sink to detect a '0'
#define DETECT_VARIANCE 0.15	// Percent (0 to 1) over or under a memory value can be and still be detected as a 1 or 0
#define CHANNEL_TIME  2			// Number of seconds the channel will be active; The channel is active when the sink is recording data
#define CALIB_TIME 1            // Amount of seconds source/sink will spend calculating the average baseline memory usage before starting the channel
#define CALIB_DELAY 100000      // Number of microseconds between each calibration recording; A smaller number means more calibration recordings will be made
#define RECORD_DELAY 10000		// Number of microseconds between sink's recordings; A smaller number means sink will record more values of FreeMem
								// This number must be smaller than SEND_DELAY, or the sink WILL miss source transmissions
#define SEND_DELAY 100000		// Number of microseconds between source's transmissions; A smaller number means source will send bits faster
								// This value is related to how long the source keeps a piece of memory allocated
#define NUM_CONFIRMS 4			// Number of consecutive reading of the same value (1 or 0) that need to occur for a bit to be recognized
								// This is related to the RECORD_DELAY value; If you increase RECORD_DELAY, you should also increase this value

vector<unsigned long> get_trans_readings(); // Used by sink
vector<int> get_source_sequence();			// Used by source and sink
string extract_mem_val(string line);		// Used internally meminfo.cpp
void update_mem_free();						// Used internally meminfo.cpp
void record_calib_reading();				// Used internally meminfo.cpp
void record_trans_reading();				// Used by sink
void calc_base_mem_free();					// Used internally meminfo.cpp
unsigned long do_channel_calibartion();		// Used by source and sink

#endif