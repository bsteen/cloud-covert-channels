#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include <errno.h>

int main(){

    FILE *filelist[8];
    int i;

    //Create empty files to place the lock on.
    filelist[0] = fopen("lockfile0", "a");
    filelist[1] = fopen("lockfile1", "a");
    filelist[2] = fopen("lockfile2", "a");
    filelist[3] = fopen("lockfile3", "a");
    filelist[4] = fopen("lockfile4", "a");
    filelist[5] = fopen("lockfile5", "a");
    filelist[6] = fopen("lockfile6", "a");
    filelist[7] = fopen("lockfile7", "a");

    //Using a shared (read) lock right now to easily find it in the lock list.
    int flock_error;
    for(i = 0; i < 8; i++){
      flock_error = flock(fileno(filelist[i]), LOCK_SH);
      
      if(flock_error == -1){
        printf("%s\n", strerror(errno));
      }
    }

    printf("All files locked>>>");
    getchar();

    flock(fileno(filelist[6]), LOCK_UN);
    printf("Lock 6 removed>>>");
    getchar();

    flock(fileno(filelist[6]), LOCK_SH);
    printf("Lock 6 added>>>");
    getchar();


    //Release the lock and close file.
    for(i = 0; i < 8; i++){
      flock(fileno(filelist[i]),LOCK_UN);
      fclose(filelist[i]);
    }
    return 0;
}
