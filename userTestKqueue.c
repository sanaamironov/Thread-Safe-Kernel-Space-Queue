/*
 * File:        sys_call_test.c
 * Author:      Sanaa Mironov
 * Created:     2025-03-27
 * Description:
 *   Defines the custom system calls for interacting with a kernel-space
 *   thread-safe queue. These syscalls allow user-space programs to enqueue
 *   and dequeue integers through a kernel-backed data structure.
 *
 *   Part of the "Kernel-Space Queue via System Calls" teaching project.
 *
 * Course:      Operating Systems
 * University:  UMBC
 * License:     MIT License (for educational use)
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

// Syscall numbers
#define SYS_my_enqueue 548
#define SYS_my_dequeue 549

// ANSI color macros
#define CLR_RESET "\033[0m"
#define CLR_GREEN "\033[1;32m"
#define CLR_BLUE "\033[1;34m"
#define CLR_YELLOW "\033[1;33m"
#define CLR_RED "\033[1;31m"
#define CLR_MAGENTA "\033[1;35m"

#define NUM_ITEMS 10

void *producer(void *arg)
{
    for (int i = 0; i < NUM_ITEMS; ++i)
    {
        int val = (i + 1) * 10;
        int ret = syscall(SYS_my_enqueue, val);
        if (ret == 0)
        {
            printf(CLR_GREEN "[Producer | TID %lu] Enqueued: %d\n" CLR_RESET, pthread_self(), val);
        }
        else
        {
            printf(CLR_RED "[Producer | TID %lu] Failed to enqueue: %d (%s)\n" CLR_RESET, pthread_self(), val, strerror(errno));
        }
        usleep(100000); // 100ms delay
    }
    return NULL;
}

void *consumer(void *arg)
{
    for (int i = 0; i < NUM_ITEMS; ++i)
    {
        int val = 0;
        int ret = syscall(SYS_my_dequeue, &val);
        if (ret == 0)
        {
            printf(CLR_BLUE "[Consumer | TID %lu] Dequeued: %d\n" CLR_RESET, pthread_self(), val);
        }
        else
        {
            printf(CLR_RED "[Consumer | TID %lu] Failed to dequeue (%s)\n" CLR_RESET, pthread_self(), strerror(errno));
        }
        usleep(150000); // 150ms delay
    }
    return NULL;
}

int main()
{
    pthread_t prod1, prod2, cons1, cons2;

    printf(CLR_MAGENTA "🧪 [TEST] Starting threaded kernel-queue syscall test\n" CLR_RESET);

    // Create producer threads
    pthread_create(&prod1, NULL, producer, NULL);
    pthread_create(&prod2, NULL, producer, NULL);

    // Create consumer threads
    pthread_create(&cons1, NULL, consumer, NULL);
    pthread_create(&cons2, NULL, consumer, NULL);

    // Wait for all threads
    pthread_join(prod1, NULL);
    pthread_join(prod2, NULL);
    pthread_join(cons1, NULL);
    pthread_join(cons2, NULL);

    printf(CLR_MAGENTA "✅ [TEST COMPLETE] All threads finished.\n" CLR_RESET);
    return 0;
}
