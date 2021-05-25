

#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct Queue{
	int front;
	int end;
	int size;
	int capacity;
	int* list;
} Queue;

Queue* createQueue(int capacity);

int isFull(struct Queue* queue);

int isEmpty(struct Queue* queue);

void enqueue(struct Queue* queue, int data);

int dequeue(struct Queue* queue);


int front(struct Queue* queue);

int end(struct Queue* queue);

#endif
