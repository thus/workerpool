workerpool
==========

A simple worker pool implementation. Supports starting an arbitrary number
of worker threads and passing data to the threads.

Example:
------
```c
/* Gets called on worker startup */
void init_callback(int id, void *user)
{
    puts("Initializing worker);
}

/* Gets called until the worker is shut down */
void work_callback(int id, void *user)
{
    puts("Working");
}

/* Gets called on worker shut down */
void deinit_callback(int id, void *user)
{
    puts("Deinitializing worker");
}

int main(int argc, char **argv)
{
    /* Start workers */
    WorkerPool *wpool = workerpool_init(5, &init_callback,
                                        &work_callback, &deinit_callback,
                                        NULL);

    /* Shut down workers */
    workerpool_deinit(wpool);

    return 0;
}
```

More examples could be found in the examples directory.

