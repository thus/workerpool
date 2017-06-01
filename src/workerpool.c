/*
 * Simple worker pool implementation.
 * Copyright (C) 2017  Mats Klepsland <mats.klepsland@gmail.com>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "workerpool.h"

/**
 * \internal
 * \brief Thread started for each worker.
 *
 * \param user Data passed to thread.
 */
static void *worker_thread(void *user)
{
    WorkerData *wdata = (WorkerData *)user;
    WorkerPool *wpool = wdata->wpool;

    if (wpool->init_callback != NULL)
        wpool->init_callback(wdata->id, wpool->user);

    while (1)
    {
        if (wpool->flags & WPOOL_FLAG_SHUTDOWN_WORKERS)
            break;

        wpool->work_callback(wdata->id, wpool->user);

        usleep(50000 /* 50ms */);
    }

    if (wpool->deinit_callback != NULL)
        wpool->deinit_callback(wdata->id, wpool->user);

    return NULL;
}

/**
 * \internal
 * \brief Free worker data.
 *
 * \param wpool Pointer to worker pool.
 */
static void workerpool_worker_data_free(WorkerPool *wpool)
{
    if (wpool == NULL || wpool->wdata == NULL)
        return;

    int i;
    for (i = 0; i < wpool->num_workers; i++)
    {
        if (wpool->wdata[i] != NULL)
            free(wpool->wdata[i]);
    }

    free(wpool->wdata);
}

/**
 * \brief Initialize worker pool.
 *
 * \param num_workers     Number of workers in pool.
 * \param init_callback   Callback to run to initialize worker.
 * \param work_callback   Callback to run to perform work.
 * \param deinit_callback Callback to run to deinitialize worker.
 * \param user            Data to pass to callback functions.
 *
 * \retval wpool Pointer to worker pool.
 * \retval NULL on failure.
 */
WorkerPool *workerpool_init(uint16_t num_workers,
                            void (*init_callback)(int, void *),
                            void (*work_callback)(int, void *),
                            void (*deinit_callback)(int, void *), void *user)
{
    if (num_workers == 0 || work_callback == NULL)
        return NULL;

    if (num_workers > WPOOL_WORKERS_MAX)
        return NULL;

    WorkerPool *wpool = malloc(sizeof(WorkerPool));
    if (wpool == NULL)
        return NULL;

    wpool->flags = 0;
    wpool->num_workers = num_workers;

    wpool->init_callback = init_callback;
    wpool->work_callback = work_callback;
    wpool->deinit_callback = deinit_callback;

    wpool->user = user;

    wpool->workers = calloc(num_workers, sizeof(pthread_t));
    if (wpool->workers == NULL) 
        goto error;

    wpool->wdata = calloc(num_workers, sizeof(WorkerData *));
    if (wpool->wdata == NULL)
        goto error;

    int i;
    for (i = 0; i < num_workers; i++)
    {
        WorkerData *wdata = malloc(sizeof(WorkerData));
        if (wdata == NULL)
             goto error;

        wdata->id = i;
        wdata->wpool = wpool;

        wpool->wdata[i] = wdata;

        if (pthread_create(&wpool->workers[i], NULL,
                           &worker_thread, wdata) != 0)
            goto error;
    }

    return wpool;

error:
    workerpool_worker_data_free(wpool);

    if (wpool->workers)
        free(wpool->workers);

    if (wpool != NULL)
        free(wpool);

    return NULL;
}

/**
 * \brief Deinitialize worker pool.
 *
 * \param wpool Pointer to the worker pool.
 */
void workerpool_deinit(WorkerPool *wpool)
{
    if (wpool == NULL)
        return;

    wpool->flags |= WPOOL_FLAG_SHUTDOWN_WORKERS;

    int i;
    for (i = 0; i < wpool->num_workers; i++)
    {
        pthread_join(wpool->workers[i], NULL);
    }

    workerpool_worker_data_free(wpool);

    free(wpool->workers);

    free(wpool);
}

