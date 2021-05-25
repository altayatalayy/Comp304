#include "queue.h"

#include <stdlib.h>

Queue* createQueue(int capacity){

    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->end = capacity - 1;
    queue->list = (int*)malloc(queue->capacity * sizeof(int));
    return queue;
}


int isFull(struct Queue* queue) {
    return (queue->size == queue->capacity);
}

int isEmpty(struct Queue* queue) {
    return (queue->size == 0);
}

void enqueue(struct Queue* queue, int data) {
    if (isFull(queue))
        return;
    queue->end = (queue->end + 1) % queue->capacity;
    queue->list[queue->end] = data;
    queue->size = queue->size + 1;
}


int dequeue(struct Queue* queue) {
    if (isEmpty(queue))
        return -1;
    int data = queue->list[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return data;
}

int front(struct Queue* queue) {

    if (isEmpty(queue))
        return -1;
    return queue->list[queue->front];
}

int end(struct Queue* queue) {
    if (isEmpty(queue))
        return -1;
    return queue->list[queue->end];
}
