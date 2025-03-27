/*
 * File:        sys_queue.c
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
#include "queue.h"

// Global instance of the kernel queue
static Queue q;
static bool initialized = false;

// -----------------------------------------------------------------------------
// Custom system call: my_enqueue(int val)
// Adds a value to the queue
// -----------------------------------------------------------------------------
SYSCALL_DEFINE1(my_enqueue, int, val)
{
    if (!initialized)
    {
        initQueue(&q);
        initialized = true;
    }

    return enqueue(&q, val);
}

// -----------------------------------------------------------------------------
// Custom system call: my_dequeue(int *user_ptr)
// Removes a value from the queue and copies it to user-space
// -----------------------------------------------------------------------------
SYSCALL_DEFINE1(my_dequeue, int __user *, user_ptr)
{
    int val;

    if (!initialized)
    {
        initQueue(&q);
        initialized = true;
    }

    int ret = dequeue(&q, &val);
    if (ret < 0)
        return ret;

    // Copy result back to user space
    if (copy_to_user(user_ptr, &val, sizeof(int)))
        return -EFAULT;

    return 0;
}
