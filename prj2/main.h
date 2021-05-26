
#include <stdbool.h>

#ifndef __MAIN_H__
#define __MAIN_H__

#define MAX_THREAD_NUM 5
pthread_t tid[MAX_THREAD_NUM] = {0};
size_t thread_count = 0; // Total number of utilized threads
int pthread_sleep(double seconds);
void create_new_thread(void *(func)(void* vargp));
void* moderator(void *vargp);
void* commmentator(void *vargp);


#define mutex_t pthread_mutex_t
#define lock(mutex) pthread_mutex_lock(&(mutex))
#define unlock(mutex) pthread_mutex_unlock(&(mutex))
void* bnews(void *vargs);

bool probabilityCheck(float p);
//bool questionReady = false;
//pthread_cond_t conditionvar;

//queue new queue answerQueue
//int questionNumber = 50;


#endif
