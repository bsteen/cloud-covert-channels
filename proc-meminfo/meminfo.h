// meminfo.h
// Defines interface functions in meminfo.c used by source.c and sink.c
// Defines costants used by meminfo.c, source.c, and sink.c

#define HIGH_BIT_ALLOC 200000       // Number of kB above baseline memory usage for sink to detect a '1'
#define LOW_BIT_ALLOC 100000        // Number of kB above baseline memory usage for sink to detect a '0'
#define BASELINE_AVG_TIME 10        // Amount of time source/sink will spend calculating the average 
                                    // baseline memory usage before starting the channel; NEED TO IMPLEMENT THIS

