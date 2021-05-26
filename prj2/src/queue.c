#include "queue.h"

#include <stdlib.h>

Queue* createQueue(int capacity){
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->end = capacity - 1;
    queue->list = (int*)malloc(queue->capacity * sizeof(int));
	pthread_mutex_init(&(queue->mutex), NULL);
    return queue;
}

void freeQueue(Queue* queue){
	free(queue->list);
	pthread_mutex_destroy(&(queue->mutex));
}


int isFull(struct Queue* queue) {
   	pthread_mutex_lock(&(queue->mutex));
	int size = queue->size, cap = queue->capacity;
   	pthread_mutex_unlock(&(queue->mutex));
	return (size == cap);
}

int isEmpty(struct Queue* queue) {
   	pthread_mutex_lock(&(queue->mutex));
	int size = queue->size;
   	pthread_mutex_unlock(&(queue->mutex));
	return (size == 0);
}

int enqueue(struct Queue* queue, int data) {
    if (isFull(queue))
        return -1;
	pthread_mutex_lock(&(queue->mutex));
    queue->end = (queue->end + 1) % queue->capacity;
    queue->list[queue->end] = data;
    queue->size = queue->size + 1;
    int answer = queue->size;
	pthread_mutex_unlock(&(queue->mutex));
    return answer;
}


int dequeue(struct Queue* queue) {
	if (isEmpty(queue))
        return -1;
   	pthread_mutex_lock(&(queue->mutex));
    int data = queue->list[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
	pthread_mutex_unlock(&(queue->mutex));
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
void clear(struct Queue* queue){
    for(int i = 0; i < queue->size; i++){
        dequeue(queue);
    }
}
int getSize(struct Queue* queue){
        pthread_mutex_lock(&(queue->mutex));
        int answer = queue->size;
        pthread_mutex_unlock(&(queue->mutex));
        return answer;
}
