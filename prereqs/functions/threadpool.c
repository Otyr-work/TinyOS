#define NO_ERROR 0

#include <stdio.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>





void * worker (void * args) {


    printf("This is the worker thread\n");

    printf("Thread ID: %ld\n", syscall(SYS_gettid));

    sleep(15);

    return NULL;

}





int main() {

    printf("Thread ID: %ld\n", syscall(SYS_gettid));

    pthread_t thread;

    pthread_create(&thread, NULL, worker, NULL);

    pthread_join(thread, NULL);

    printf("Meain thread done\n\n");

    return NO_ERROR;

}
