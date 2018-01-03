#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manageProcLocks.h"

int num_current_locks = 0;	// The number of locks currently in /proc/locks (NOT THE SAME AS num_LockEntries)
							// Value is reset to 0 every time the updateProcLocksList() function is called
int num_LockEntries = 0;	// The number of lock entries in proclocks_list
							// It keeps the running tally of the total number of unique locks found in /proc/locks; it is never reset/de-incremented during runtime

int get_num_lock_entries(){
    return num_LockEntries;
}

int get_num_current_locks(){
    return num_current_locks;
}

// Takes in the full /proc/locks entry and a buffer to place the device number from the entry
// Places the device number inside the buffer
void extractDeviceNumber(char *entry, char *dev_num){
        int i;
         // Get rid of all the stuff before the device number
         for(i = 0; i < 5; i++){
             while((*entry) != ' '){
                 entry++;
             }
             if(*(entry + 1) == ' '){
                 entry++;
             }
             entry++;
         }
    
         i = 1; // Acts as the length of the device number
         while(*(entry + i) != ' '){  // Count up to the last character in the device number
             i++;
         }
    
         memcpy(dev_num, entry, i);
         dev_num[i] = '\0';
    
         return;
}

// Records all the entries in /proc/locks by placing their device number in current_proc_lock_entries
// Reset the num_current_locks global value every time
void collectProcLockData(){
    FILE *procLocksFile = fopen("/proc/locks", "r");
    if(procLocksFile == NULL){
        printf("Could not open /proc/locks\n");
        return;
    }

    char *line = NULL;
    size_t sizeof_line = 0;
    num_current_locks = 0; // Resets the "size" of current_proc_lock_entries

    while(getline(&line, &sizeof_line, procLocksFile) != -1){

        // Extracts the device number and places it in current_proc_lock_entries
        extractDeviceNumber(line, current_proc_lock_entries[num_current_locks]);

        // Set arguments before getting the next line
        num_current_locks++;
        sizeof_line = 0;
        free(line);

        if(num_current_locks >= MAX_PROC_LOCK_ENTRIES - 1){
            printf("current_proc_lock_entries is full! There are %d locks in /proc/locks!\n", num_current_locks);
        }
    }

    free(line); // getline allocs space even when it returns -1
    fclose(procLocksFile);
    return;
}

// If a lock is already in the proclocks_list, return its index
// Else return -1
int getProcLocksListIndex(char *dev_num){
	int i;
    for(i = 0; i < num_LockEntries; i++){
        if(strcmp(dev_num, proclocks_list[i].device_number) == 0){
            return i;
        }
    }
    return -1;
}

// A lock is a new entry if appears in /proc/locks (i.e. is in current_proc_lock_entries), but is not already recorded in proclocks_list
// Add this entry to proclocks_list in this case
void addNewLocks(){
    int i;
    for(i = 0; i < num_current_locks; i++){
        if(getProcLocksListIndex(current_proc_lock_entries[i]) == -1){
            if(num_LockEntries >= MAX_PROC_LOCK_ENTRIES - 1){
                printf("WARNING, proclocks_list is full! %d unique locks have been recorded!\n", num_LockEntries);
                return;
            }
            else{
                strcpy(proclocks_list[num_LockEntries].device_number, current_proc_lock_entries[i]);
                proclocks_list[num_LockEntries].bit_state = 1;
                proclocks_list[num_LockEntries].num_toggles = 0;
                proclocks_list[num_LockEntries].entry_updated = 1;  // Lock entry has been "updated" by being added to the list.
                num_LockEntries++;
            }
        }
    }
    return;
}

// At this point, all locks that are in current_proc_lock_entries have an entry in proclocks_list
// Find the entry in proclocks_list that corresponds to each locks in current_proc_lock_entries and update its proclocks_list data
void updateFoundLocks(){
    int i;
    for(i = 0; i < num_current_locks; i++){
        int index = getProcLocksListIndex(current_proc_lock_entries[i]); // index will be between 0 and MAX_PROC_LOCK_ENTRIES since we already ran addNewLocks
        if(!proclocks_list[index].entry_updated){
		    if(proclocks_list[index].bit_state == 0){	// If the lock's previous state was 0, then updated the bit state to 1 and count a toggle
                proclocks_list[index].bit_state = 1;
                proclocks_list[index].num_toggles++;
            }
		    proclocks_list[index].entry_updated = 1;
        }
    }
}

// Locks that have already been recorded, but weren't currently present in /proc/locks are logically set to zero
// Go through the proclocks_list and updates any locks that haven't been updated because of updateFoundLocks
// Also resets every locks updated variable for the next time this module is called.
void updateUnFoundLocks(){
    int i;
    for(i = 0; i < num_LockEntries; i++){
        if(!proclocks_list[i].entry_updated){
            if(proclocks_list[i].bit_state == 1){	// If the lock's previous state was 1, then update the bit state to 0 and count a toggle
                proclocks_list[i].bit_state = 0;
                proclocks_list[i].num_toggles++;
            }
        }
        proclocks_list[i].entry_updated = 0;
    }
}

// The handler function for all other functions in manageProcLocks
// The sender or receiver needs to call this function to update their lock records
void updateProcLocksList(){
    collectProcLockData();
    addNewLocks();
	updateFoundLocks();
    updateUnFoundLocks();
}