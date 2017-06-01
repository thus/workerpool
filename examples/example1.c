#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <workerpool.h>

#define NUM_WORKERS 5

typedef struct ThreadData_ {
    int magic_number[NUM_WORKERS];
} ThreadData;

/* This callback function is called when the worker thread is started */
void init_cb(int id, void *user)
{
    ThreadData *td = (ThreadData *)user;

    /* Set magic number for thread */
    td->magic_number[id] = id * 10;

    printf("Thread '%d' ready\n", id);
}

/* This callback function is called continuously until the worker pool
   is shut down. */ 
int work_cb(int id, void *user)
{
    ThreadData *td = (ThreadData *)user;
    int magic_number = td->magic_number[id];

    printf("Thread '%d' has magic number: %d\n", id, magic_number);
}

/* This callback function is called when the worker pool is shut down */
void deinit_cb(int id, void *user)
{
    ThreadData *td = (ThreadData *)user;

    /* Lets just do something random and useless */
    td->magic_number[id] = 0;

    printf("Shutting down thread '%d'\n", id); 
}

int main(int argc, char **argv)
{
    /* Allocate some garbage data to show sharing data amongst the worker
       threads. The worker id can be used to e.g. lookup data in a
       array containing data for the workers. */
    ThreadData *td = calloc(1, sizeof(ThreadData));
    if (td == NULL) {
        fprintf(stderr, "Error allocating memory for thread data\n");
        exit(EXIT_FAILURE);
    }

    /* Start the workers in the worker pool */
    WorkerPool *wpool = workerpool_init(NUM_WORKERS, &init_cb, &work_cb,
                                        &deinit_cb, td);
    if (wpool == NULL) {
        fprintf(stderr, "Error initializing worker pool\n");
        exit(EXIT_FAILURE);
    }

    /* Sleep for one second */
    usleep(1000000);

    /* Stop the workers in the worker pool */
    workerpool_deinit(wpool);

    free(td);

    return 0;
}

