#ifndef MANAGEPROCLOCKS_H
#define MANAGEPROCLOCKS_H

#define MAX_PROC_LOCK_ENTRIES 256	// Max number of /proc/locks entries this program can store
#define TOGGLE_THRESHOLD 15			// Number of times the receiver has to toggle a lock in order for to ACK the senders
									// Also is the number of times the sender has to toggle its data/transmission locks for the sender to identify them
#define NUM_DATA_LOCKS 32 			// Max is 32 right now since I use unsigned int
#define NUM_TOTAL_LOCKS NUM_DATA_LOCKS + 1
#define NUM_TRANSFERS 32			// Each transfer is "NUM_DATA_LOCKS" bits in size; e.g.: 32 data locks is 4 bytes in one transmission
#define BILLION 1000000000L			// Used for timing calculations

// Used to store entries from /proc/locks
typedef struct LockEntry{
	char device_number[32]; //Format is MM:mm:INODE; Might need to increase this array size if inode numbers are too large
	int bit_state;			// Was the lock present (set to 1) or not present this time (set to 0)
	int num_toggles;
	int entry_updated; 		// For this current update call, has the lock's state already been recorded?
}LockEntry;

LockEntry proclocks_list[MAX_PROC_LOCK_ENTRIES]; // Contains all the locks ever found in /proc/locks during the handshake
char current_proc_lock_entries[MAX_PROC_LOCK_ENTRIES][32];	// List of all device numbers CURRENTLY in /proc/locks; Is an array of strings
														   	// This should only be used internally by manageProcLocks.c
															// Other modules should look through proclocks_list after calling updateProcLocksList()

// Functions
void updateProcLocksList();	// Function used by sender and receiver
void collectProcLockData();
void addNewLocks();
void updateFoundLocks();
void updateUnFoundLocks();
int getProcLocksListIndex(char *entry);
void extractDeviceNumber(char *entry, char *dev_num);
int get_num_lock_entries(); // Gets the number of locks num_LockEntries
int get_num_current_locks();

#endif