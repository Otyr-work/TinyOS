#define NO_ERROR 0

#include <stdio.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>

struct job {
    
    int number;
    
};


void * worker (void * args) {
    
    struct job * curr_job = (struct job *)args;

    printf("worker recieved: %d\n", curr_job->number);

    printf("Thread ID: %ld\n", syscall(SYS_gettid));

    return NULL;

}





int main() {
    
    int number = 42;

    printf("Thread ID: %ld\n", syscall(SYS_gettid));

    pthread_t thread;
    
    
    struct job c_job = { .number = 42 };

    pthread_create(&thread, NULL, worker, &c_job);

    pthread_join(thread, NULL);

    printf("Meain thread done\n\n");

    return NO_ERROR;

}
