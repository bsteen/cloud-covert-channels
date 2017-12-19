#include "stdio.h"
#include <time.h>
#include <unistd.h>

#define BILLION 1000000000L

int main(void) {
    struct timespec start, end;
    unsigned long elapsed_seconds = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);

	while(elapsed_seconds < 5000000000){

        usleep(10000);  // Wait 0.01 seconds
	    clock_gettime(CLOCK_MONOTONIC, &end);

        elapsed_seconds = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
		
	    // printf("%d: %d\n", i++, elapsed_seconds);
	}
  
    printf("Done\n");
    return 0;
}
