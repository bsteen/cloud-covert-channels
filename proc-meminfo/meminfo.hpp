// meminfo.hpp
// Defines interface functions in meminfo.c used by source.c and sink.c
// Defines costants used by meminfo.c, source.c, and sink.c
#include <string>
using namespace std;

#define HIGH_BIT_ALLOC 200000   // Number of kB above baseline memory usage for sink to detect a '1'
#define LOW_BIT_ALLOC 100000    // Number of kB above baseline memory usage for sink to detect a '0'
#define CALIB_TIME 5            // Amount of seconds source/sink will spend calculating the average baseline memory usage before starting the channel
#define CALIB_DELAY 100000      // Number of microseconds between each calibration recording; Smaller number means more calibration recordings will be made
#define NUM_CONSEC_VAL 2        // Number of consecutive reading of the same type (1, 0, or null) that need to occur for a value to be recognized.

unsigned long get_mem_free();           // Used by source and sink
unsigned long get_base_mem_free();      // Used by source and sink
string extract_mem_val(string line);    // Used internally meminfo.cpp
void update_mem_free();                 // Used internally meminfo.cpp
void record_calib_reading();            // Used internally meminfo.cpp
void record_trans_reading();            // Used by sink
void calc_base_mem_free();              // Used internally meminfo.cpp
unsigned long do_channel_calibartion(); // Used by source and sink