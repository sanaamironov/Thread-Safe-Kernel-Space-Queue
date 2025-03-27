/*
 * File:        kqueue.c
 * Author:      Sanaa Mironov
 * Created:     2025-03-27
 * Description:
 *   Implements a dynamic, thread-safe queue using kernel memory management
 *   and synchronization primitives. Used in system calls and kernel-space testing.
 *
 *   Part of the "Kernel-Space Queue via System Calls" teaching project.
 *
 * Course:      Operating Systems
 * University:  UMBC
 * License:     MIT License (for educational use)
 */

#include "queue.h"

// -----------------------------------------------------------------------------
// Initialize queue with empty front and rear, mutex and wait queue
// -----------------------------------------------------------------------------
void initQueue(Queue *q)
{
    q->front = q->rear = NULL;
    mutex_init(&q->lock);
    init_waitqueue_head(&q->cond);
}

// -----------------------------------------------------------------------------
// Free all remaining nodes and reset queue state
// -----------------------------------------------------------------------------
void deleteQueue(Queue *q)
{
    mutex_lock(&q->lock);

    Node *curr = q->front;
    while (curr)
    {
        Node *temp = curr;
        curr = curr->next;
        kfree(temp);
    }

    q->front = q->rear = NULL;
    mutex_unlock(&q->lock);
}

// -----------------------------------------------------------------------------
// Allocate and insert a new value into the queue (thread-safe)
// -----------------------------------------------------------------------------
int enqueue(Queue *q, int data)
{
    Node *newNode = kmalloc(sizeof(Node), GFP_KERNEL);
    if (!newNode)
        return -ENOMEM;

    newNode->data = data;
    newNode->next = NULL;

    mutex_lock(&q->lock);

    if (!q->rear)
    {
        q->front = q->rear = newNode;
    }
    else
    {
        q->rear->next = newNode;
        q->rear = newNode;
    }

    mutex_unlock(&q->lock);
    wake_up_interruptible(&q->cond); // Wake up any waiting consumer

    printk(KERN_INFO "\033[1;32m[enqueue] PID: %d Value: %d\033[0m\n", current->pid, data);
    return 0;
}

// -----------------------------------------------------------------------------
// Remove a value from the front of the queue (blocks if empty)
// -----------------------------------------------------------------------------
int dequeue(Queue *q, int *val)
{
    // Block until queue is not empty
    wait_event_interruptible(q->cond, q->front != NULL);

    mutex_lock(&q->lock);

    if (!q->front)
    {
        mutex_unlock(&q->lock);
        return -EAGAIN;
    }

    Node *temp = q->front;
    *val = temp->data;
    q->front = q->front->next;

    if (!q->front)
        q->rear = NULL;

    kfree(temp);
    mutex_unlock(&q->lock);

    printk(KERN_INFO "\033[1;34m[dequeue] PID: %d Value: %d\033[0m\n", current->pid, *val);
    return 0;
}