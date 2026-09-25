#define NO_ERROR 0

#include <stdio.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>

#define WORKER_COUNT 3
#define JOB_COUNT 6

struct job {
  
    void (*function)(void *);
    void * argument;
    
};

struct job jobs[JOB_COUNT];
int next_job = 0;

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

void print_function(void * args) {
    
    int * number = (int *) args;
    
    printf("worker recieved: %d\n", *number);
    
}

void * worker (void * args) {
    
    int flag interrupt_queue = 0;
    
    while (interrupt_queue != -1){
        
        // continue from here
        
    }
    
    struct job * curr_job = (struct job *)args;
    
    curr_job->function(curr_job->argument);
    
    printf("Thread ID: %ld\n", syscall(SYS_gettid));
    
    return NULL;
    
}

int main() {
    
    int number_1 = 42;
    int number_2 = 100;
    int number_3 = 777;
    
    struct job jobs[3] = {
        
        { .function = print_function, .argument = &number_1 },
        { .function = print_function, 
        .argument = &number_2 },    
        { .function = print_function,
        .argument = &number_3 }
        
    };
    
    for (int i = 0; i < 3; i++) {
        
        pthread_t thread;
        
        pthread_create(&thread, NULL, worker, &jobs[i]);
        
        pthread_join(thread, NULL);
        
    }
    
    printf("Thread ID: %ld\n", syscall(SYS_gettid));
    
    printf("Main thread done\n\n");
    
    return NO_ERROR;
    
}
