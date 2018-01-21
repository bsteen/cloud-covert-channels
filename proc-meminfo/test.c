#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(){            
    void *ptr = malloc(524288000);
    memset(ptr, 'a', 524288000);
    sleep(2);
    free(ptr);

    return 0;
}