
#include <pthread.h>

#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct Queue{
	int front;
	int end;
	int size;
	int capacity;
	int* list;
	pthread_mutex_t mutex;
} Queue;

Queue* createQueue(int capacity);
void freeQueue(Queue* queue);
int isFull(struct Queue* queue);
int isEmpty(struct Queue* queue);
int enqueue(struct Queue* queue, int data);
int dequeue(struct Queue* queue);
int front(struct Queue* queue);
int end(struct Queue* queue);
int getSize(struct Queue* queue);
void clear(struct Queue* queue);

#endif
