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

#include <inttypes.h>
#include <pthread.h>

#ifndef __WORKERPOOL_H__
#define __WORKERPOOL_H__

/* Flag used to indicate that the workers should shut down */
#define WPOOL_FLAG_SHUTDOWN_WORKERS 0x01

/* Max number of allowed worker threads */
#define WPOOL_WORKERS_MAX 65536

typedef struct WorkerPool_ {
    uint16_t num_workers;
    pthread_t *workers;
    struct WorkerData_ **wdata;
    volatile uint8_t flags;

    void (*init_callback)(int, void *);
    void (*work_callback)(int, void *);
    void (*deinit_callback)(int, void *);

    void *user;
} WorkerPool;

typedef struct WorkerData_ {
    uint16_t id;
    WorkerPool *wpool;
} WorkerData;

WorkerPool *workerpool_init(uint16_t, void (*init_callback)(int, void *),
                            void (*work_callback)(int, void *),
                            void (*deinit_callback)(int, void *), void *user);

void workerpool_deinit(WorkerPool *);

#endif /* __WORKERPOOL_H__ */

