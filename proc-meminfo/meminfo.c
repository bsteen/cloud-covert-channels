// meminfo.c
// Interface functions used by source.c and sink.c to read from /proc/meminfo.c

#include "meminfo.h"
#include <stdio.h>
#include <stdlib.h>

unsigned long mem_free = 0;     // Amount of RAM in kB left unused by the system

unsigned long get_mem_free(){
    return mem_free;
}

void update_mem_free(){
    FILE *file = fopen("/proc/meminfo","r");

    if(file != NULL){
        char *line = NULL;
        size_t size = 0;

        getline(&line, &size, file);    // Get first line and throw it away; don't need it
        free(line);
        line = NULL;
        size = 0;
        getline(&line, &size, file);    // Get the second line of /proc/meminfo; Contains MemFree value

        printf("%s", line);

        free(line);
        if(fclose(file) != 0){
            printf("Could not close /proc/meminfo\n");
        }
    }
    else{
        printf("Could not open /proc/meminfo\n");
    }

    return;
}

// For testing functions
int main(){
    update_mem_free();
}