#define NO_ERROR 0

#include <stdio.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>





void * worker (void * args) {
    
    int * number = (int *) args;

    printf("worker recieved: %d\n", *number);

    printf("Thread ID: %ld\n", syscall(SYS_gettid));

    return NULL;

}





int main() {
    
    int number = 42;

    printf("Thread ID: %ld\n", syscall(SYS_gettid));

    pthread_t thread;

    pthread_create(&thread, NULL, worker, &number);

    pthread_join(thread, NULL);

    printf("Meain thread done\n\n");

    return NO_ERROR;

}
