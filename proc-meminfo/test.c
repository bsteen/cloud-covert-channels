#include <stdlib.h>
#include <unistd.h>

int main(){
    void *ptr = malloc(1000000000);
    sleep(10);
    free(ptr);

    return 0;
}