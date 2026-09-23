#define NO_ERROR 0

#include <stdio.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>

struct job {
    
    void (*function)(void *);
    void * argument;
    
};


void print_function(void * args) {
    
    int * number = (int *) args;
    
    printf("worker recieved: %d\n", *number);
    
}

void * worker (void * args) {
    
    struct job * curr_job = (struct job *)args;

   curr_job->function(curr_job->argument);

    printf("Thread ID: %ld\n", syscall(SYS_gettid));

    return NULL;

}





int main() {
    
    int number = 42;
    
    
    struct job c_job = { 
        .function = print_function,
        .argument = &number
    };

    

    
    printf("Thread ID: %ld\n", syscall(SYS_gettid));

    pthread_t thread;
    
    

    pthread_create(&thread, NULL, worker, &c_job);

    pthread_join(thread, NULL);

    printf("Meain thread done\n\n");

    return NO_ERROR;

}
