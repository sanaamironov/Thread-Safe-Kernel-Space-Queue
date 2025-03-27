/*
 * File:        kqueue.h
 * Author:      Sanaa Mironov
 * Created:     2025-03-27
 * Description:
 *   Kernel-space header for a dynamic, thread-safe queue implementation.
 *   Defines the queue structure and function prototypes for use in
 *   system call handlers or other kernel modules.
 *
 *   Part of the "Kernel-Space Queue via System Calls" teaching project.
 *
 * Course:      Operating Systems
 * University:  UMBC
 * License:     MIT License (for educational use)
 */
#ifndef KERNEL_QUEUE_H
#define KERNEL_QUEUE_H

#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
// -----------------------------------------------------------------------------
// Queue Node Structure
// -----------------------------------------------------------------------------

// Represents a single item in the kernel queue
typedef struct Node
{
    int data;          // Stored integer value
    struct Node *next; // Pointer to next node
} Node;

// -----------------------------------------------------------------------------
// Thread-Safe Queue Structure
// -----------------------------------------------------------------------------

typedef struct Queue
{
    Node *front;            // Points to front of queue
    Node *rear;             // Points to rear of queue
    struct mutex lock;      // Kernel mutex for queue synchronization
    wait_queue_head_t cond; // Wait queue for blocking dequeue
} Queue;

// -----------------------------------------------------------------------------
// Public Kernel API for Queue Operations
// -----------------------------------------------------------------------------

void initQueue(Queue *q);        // Initialize queue
void deleteQueue(Queue *q);      // Cleanup queue memory
int enqueue(Queue *q, int data); // Enqueue value into queue
int dequeue(Queue *q, int *val); // Dequeue value (blocking)

#endif