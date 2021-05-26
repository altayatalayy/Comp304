#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef __UTIL_H__
#define __UTIL_H__


#define MAX_THREAD_NUM 5

#define mutex_t pthread_mutex_t
#define lock(mutex) pthread_mutex_lock(&(mutex))
#define unlock(mutex) pthread_mutex_unlock(&(mutex))
#define init_mutex(mutex) pthread_mutex_init(&(mutex), NULL);

void join(size_t idx);
int pthread_sleep(double seconds);
void create_new_thread(void *(func)(void* vargp));
bool probabilityCheck(float p);

#endif
