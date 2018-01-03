#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <time.h>
#include <unistd.h>
#include "manageProcLocks.h"

// The sender should be started slightly before the receiver

FILE *filelist[NUM_TOTAL_LOCKS];	// Array containing all the files to be locked/unlocked

// Locks or unlocks the specified file in filelist
void flock_file(int lock_num, int flock_operation){
	int flock_error = flock(fileno(filelist[lock_num]), flock_operation);

	if(flock_error == -1){
		printf("%s\n", strerror(errno));
	}

	return;
}

// Wrapper function to raise or lower the transmit lock
// The transmit lock will always be the last lock in the list
void set_transmit_lock(int set_value){
	if(set_value){
		flock_file(NUM_TOTAL_LOCKS - 1, LOCK_SH);
	}
	else{
		flock_file(NUM_TOTAL_LOCKS - 1, LOCK_UN);
	}
	
	return;
}

// Set the data locks using the bits of "data"
void set_data_locks(char data){
	int i;
	for(i = 0; i < NUM_DATA_LOCKS; i++){
		if(data >> i & 1){
            flock_file(i, LOCK_SH);
        }
        else{
            flock_file(i, LOCK_UN);
        }
	}

	return;
}

// Used during handshake; acts as the signal to tell the receiver to see which locks are going to be used
void set_all_locks(int set_value){
	int i;
	for(i = 0; i < 9; i++){
		if(set_value){
			flock_file(i, LOCK_SH);
		}
		else{
			flock_file(i, LOCK_UN);
		}
	}

	return;
}

// Establish a connection between the receiver
void initiate_handshake(){
	int i;
	int toggle_value = 1;
	struct timespec start_time, current_time;
    unsigned long elapsed_seconds = 0;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

	// Send out handshake to the receiver by toggling all sender locks over 5 seconds
	// Over 5 seconds, the sender should toggle all its locks 20 times
	while(elapsed_seconds < BILLION * 5){
		set_all_locks(toggle_value);
		toggle_value = !toggle_value;

		usleep(250000);	// Wait 0.25 seconds
		clock_gettime(CLOCK_MONOTONIC, &current_time);
        elapsed_seconds = BILLION * (current_time.tv_sec - start_time.tv_sec) + current_time.tv_nsec - start_time.tv_nsec;
	}

	set_all_locks(0); // Unlock all the sender locks before trying to see if receiver has ACKed

	return;
}

// The receiver ACKs the send by toggling 1 lock many times over 5 seconds.
// Go through all the recorded locks and see if they have reached the toggle threshold
// If one has, that means the receiver has ACKed the sender container
int has_receiver_ACKed(){
	int i;
	struct timespec start_time, current_time;
    unsigned long elapsed_seconds = 0;
	clock_gettime(CLOCK_MONOTONIC, &start_time);

	while(elapsed_seconds < BILLION * 5){
		updateProcLocksList(); // Update the sender's records of the locks

		usleep(10000);	// Wait 0.1 of a second
		clock_gettime(CLOCK_MONOTONIC, &current_time);
        elapsed_seconds = BILLION * (current_time.tv_sec - start_time.tv_sec) + current_time.tv_nsec - start_time.tv_nsec;
	}

	int lock_entry_count = get_num_lock_entries();

    for(i = 0; i < lock_entry_count; i++){
        if(proclocks_list[i].num_toggles >= TOGGLE_THRESHOLD && proclocks_list[i].num_toggles < TOGGLE_THRESHOLD * 3){
			printf("\tFound ACK lock %s with %d toggles.\n", proclocks_list[i].device_number, proclocks_list[i].num_toggles);
            return 1; // The handshake has succesfully completed
        }
    }

    return 0; // The receiver did not ACK the sender
}

// After the connection has been established, send the data that needs to be transferred
void transfer_data(){
	int i;
	srand(time(NULL));

	// Loop that goes through all the data and encodes it into the locks
	// ONLY	HAVE THIS FOR LOOP OR THE ONE BELOW ACTIVE. THE OTHER SHOULD BE COMMENTED OUT!
	/*
	char dataArray[NUM_TRANSFERS] = {0x09, 0xF9, 0x11, 0x02, 0x9D, 0x74, 0xE3, 0x5B, 0xD8, 0x41, 0x56, 0xC5, 0x63, 0x56, 0x88, 0xC0};
	for(i = 0; i < NUM_TRANSFERS; i++){

		printf("\tSending byte %d: 0x%02x\n", i, dataArray[i] & 0xFF);
		
		set_data_locks(dataArray[i]);	// Encode data to locks
		set_transmit_lock(1);	// Raise transmit lock so receiver knows lock data is now valid

		usleep(20000);	// Wait for receiver to parse data from locks
						// This value can cause issues; the receiver my go out of sink

		set_transmit_lock(0); // Lower transmit lock so receiver knows data is not ready yet
		
		usleep(40); // Let the receiver detect the lowered transmission lock
					// This value can cause issues; the receiver my go out of sink
	}
	*/

	// Generate NUM_TRANSFERS number of random bytes and encodes it into the locks
	// ONLY	HAVE THIS FOR LOOP OR THE ONE ABOVE ACTIVE. THE OTHER SHOULD BE COMMENTED OUT!
	for(i = 0; i < NUM_TRANSFERS; i++){
		char data = (char)(rand());
		printf("\tSending transmission %d: 0x%08x\n", i, data);
		
		set_data_locks(data);	// Encode data to locks
		set_transmit_lock(1);	// Raise transmit lock so receiver knows lock data is now valid

		usleep(20000);	// Wait for receiver to parse data from locks
						// This value can cause issues; the receiver my go out of sink

		set_transmit_lock(0); // Lower transmit lock so receiver knows data is not ready yet
		
		usleep(40); // Let the receiver detect the lowered transmission lock
					// This value can cause issues; the receiver my go out of sink
	}

	set_transmit_lock(0);	// Lower transmit lock; No longer transmitting data
	return;
}
	
int main(){
	printf("---STARTING SENDER PROGRAM---\n");

    // Create empty files to place the lock on
	// filelist[0] represents the lowest bit; will have the lowest inode number out of the locks
	// filelist[NUM_TOTAL_LOCKS - 2] represents the highest bit; will have the highest inode number out of the locks
	// filelist[NUM_TOTAL_LOCKS - 1] represents the signal lock
	int i;
	char filename[16];
	for(i = 0; i < NUM_TOTAL_LOCKS; i++){
		sprintf(filename, "lockfile%d", i);
		filelist[i] = fopen(filename, "a");
		
		if(filelist[i] == NULL){
			printf("Error opening lockfile%d", i);
			exit(1);
		}
	}

	// This gives the receiver time to start (hopefully)
	printf("Initiating handshake with receiver...\n");
	initiate_handshake();
	printf("Handshake complete! Checking for receiver ACK...\n");

    if(has_receiver_ACKed()){
		printf("Receiver has ACKed! Starting data transfer...\n");
		transfer_data();
		printf("Data transfer complete!\n");
	}
	else{
		printf("Receiver didn't send ACK. No data will be transferred.\n");
	}

	// Unlock all the locks (if they haven't already) and close files
    for(i = 0; i < NUM_TOTAL_LOCKS; i++){
      flock_file(i, LOCK_UN);
      fclose(filelist[i]);
	}

	printf("All files unlocked and closed.\n---END OF SENDER PROGRAM---\n");
	
    return 0;
}
