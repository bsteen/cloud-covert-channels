#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <time.h>
#include <unistd.h>
#include "manageProcLocks.h"

char data_locks[NUM_DATA_LOCKS][32];        // Locks used to determine if sender is still active and/or is sending valid information
char transmit_lock[32];                     // These data and transmit arrays contain the device numbers of the locks
unsigned int received_data[NUM_TRANSFERS];  // Data received by the sender is stored in this array; CHANGE THIS TO AN OUTPUT FILE LATER

FILE *ACK_file; // File that will hold the lock used for ACKing the sender

// Watch the /proc/locks file to see which locks are being toggled
void initiate_handshake(){
    struct timespec start, current;
    unsigned long elapsed_seconds = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);

	// Listen for sender to flip the locks that will be used for communication
    while(elapsed_seconds < BILLION * 5){   // Listen for 5 seconds
        updateProcLocksList();
        usleep(10000);  // Wait 10 ms
	    clock_gettime(CLOCK_MONOTONIC, &current);
        elapsed_seconds = BILLION * (current.tv_sec - start.tv_sec) + current.tv_nsec - start.tv_nsec;
	}

    return;
}

// Out of all the locks seen during the handshake, select the ones that have been toggled the most
// These locks with the lower inode numbers will be the data locks
// The lock with the highest inode number will be the transmission lock
// This function returns 1 if locks that meet the threshold are found, otherwise 0 is returned.
int determine_sender_locks(){
    int locks_found = 0;
    int locks_in_proclocks_list = get_num_lock_entries();
    int i;

    for(i = 0; i < locks_in_proclocks_list; i++){
        // Check the times the lock has toggled; It should fall within this range
        if(proclocks_list[i].num_toggles >= TOGGLE_THRESHOLD && proclocks_list[i].num_toggles < TOGGLE_THRESHOLD * 3){
            // The first lock will be the transmit lock
            if(locks_found == 0){
                strcpy(transmit_lock, proclocks_list[i].device_number);
                printf("\tTransmit lock found: %s %d\n", transmit_lock,  proclocks_list[i].num_toggles);
                locks_found++; 
            }
            else{ // The next NUM_TOTAL_LOCKS - 1 locks will be the data locks
                strcpy(data_locks[NUM_DATA_LOCKS - locks_found], proclocks_list[i].device_number);
                printf("\tData lock %d found: %s %d\n", NUM_DATA_LOCKS - locks_found, data_locks[NUM_DATA_LOCKS - locks_found], proclocks_list[i].num_toggles);
                locks_found++;
            }

            if(locks_found >= NUM_TOTAL_LOCKS){
                break;
            }
        }
    }
    
    printf("\t%d/%d valid locks were found.\n", locks_found, NUM_TOTAL_LOCKS);

    if(locks_found == NUM_TOTAL_LOCKS){
        return 1;
    }
    else{
        return 0;
    }
}

// Locks or unlocks the ACK_file
void set_ACK_lock(int toggle_value){

    int flock_error;

    if(toggle_value){
        flock_error = flock(fileno(ACK_file), LOCK_SH);
    }
    else{
        flock_error = flock(fileno(ACK_file), LOCK_UN);
    }

	if(flock_error == -1){
		printf("%s\n", strerror(errno));
	}

    return;
}

// Acknowledge the sender's handshake
// Toggle the ACK lock many time to let the sender know the specified data locks were recognized
// This will also let the sender know which lock is the receiver's ACK lock
void ACK_handshake(){
    int toggle_value = 1;
    struct timespec start, current;
    unsigned long elapsed_seconds = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);

    while(elapsed_seconds < BILLION * 5){   // send ACK for 5 seconds
        set_ACK_lock(toggle_value);
		toggle_value = !toggle_value;
        usleep(200000);  // Wait 200 ms
	    clock_gettime(CLOCK_MONOTONIC, &current);
        elapsed_seconds = BILLION * (current.tv_sec - start.tv_sec) + current.tv_nsec - start.tv_nsec;
	}

    set_ACK_lock(0);    // Turn off the ACK lock
    return;
}

// Finds the lock entry in proclocks_list and returns the locks current value
int get_lock_value(char *device_number){
    int i;
    int num_lock_entries = get_num_lock_entries();

    for(i = 0; i < num_lock_entries; i++){
        if(strcmp(device_number, proclocks_list[i].device_number) == 0){
            return proclocks_list[i].bit_state;
        }
    }

    printf("An error occurred; Device number %s was not found in proclocks_list\n", device_number);
    return 0;
}

// After a new transmission is received, send ACK to sender by setting the ACK lock
void ACK_sender(){
    set_ACK_lock(1);
    usleep(500);   // Hold the ACK lock for 500 us
    set_ACK_lock(0);

    return;
}

// While the sender is still sending data, the receiver will parse the list of locks and extract the bits values of the data locks
void receive_data(){
    unsigned int transmission_number = 0;
    int connection_active = 1;

    struct timespec prev_trans_time, current_time; 
    unsigned long elapsed_seconds = 0;
    clock_gettime(CLOCK_MONOTONIC, &prev_trans_time);   // The last time the sender has communicated with the receiver

    do{
        updateProcLocksList(); // Update the master list of locks and their bit states
        
        if(get_lock_value(transmit_lock)){   // If transmit lock is set, read the data
            printf("\tReceived transmission %u: ", transmission_number);
            int i;
            received_data[transmission_number] = 0;     // Clear the place where data is to be stored

            // Get lock data and put into received_data at transmission_number
            for(i = 0; i < NUM_DATA_LOCKS; i++){
                received_data[transmission_number] |= get_lock_value(data_locks[i]) << i;
            }

            printf("0x%x\n", received_data[transmission_number]);

            transmission_number++;
            clock_gettime(CLOCK_MONOTONIC, &prev_trans_time);   // Update the last time the sender has communicated with the receiver
            ACK_sender();   // After the data has been received, send ACK to the sender.
        }
        else{   // If the transmit lock is 0, then the sender is preparing to send a new value or has ended the connection
            clock_gettime(CLOCK_MONOTONIC, &current_time);
            elapsed_seconds = BILLION * (current_time.tv_sec - prev_trans_time.tv_sec) + current_time.tv_nsec - prev_trans_time.tv_nsec;

            if(elapsed_seconds >= BILLION * TIMEOUT){
                connection_active = 0;  // If TIMEOUT seconds have past and the sender has not activated the connection lock, the connection is dead
            }
        }
    }while(connection_active);

    return;
}

int main(){
    printf("---STARTING RECEIVER PROGRAM---\n");    

    ACK_file = fopen("lockfileACK", "a");
    if(ACK_file == NULL){
        printf("Error creating/opening lockfileACK\n");
        printf("---END RECEIVER PROGRAM---\n");
		exit(1);
    }

    printf("Listening for handshake from sender...\n");
    initiate_handshake();
    printf("Done listening for handshake.\nAnalysing lock list...\n");

    if(determine_sender_locks()){
        printf("ACKing handshake...\n");
        ACK_handshake();
        printf("ACK sent! Now receiving data...\n");
        receive_data();
        printf("Connection with sender ended.\n");
    }
    else{
        printf("Sender was not found.\n");
    }

    fclose(ACK_file);
    printf("---END RECEIVER PROGRAM---\n");    

    return 0;
}