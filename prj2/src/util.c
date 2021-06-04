#include "util.h"

static size_t thread_count = 0;
static pthread_t tid[MAX_THREAD_NUM] = {0};

void join_all(void){
	for(int i=0; i<thread_count; i++)
		 pthread_join(tid[i], NULL);
}

int pthread_sleep(double seconds){
    pthread_mutex_t mutex;
    pthread_cond_t conditionvar;
    if(pthread_mutex_init(&mutex,NULL)){
        return -1;
    }
    if(pthread_cond_init(&conditionvar,NULL)){
        return -1;
    }

    struct timeval tp;
    struct timespec timetoexpire;
    // When to expire is an absolute time, so get the current time and add
    // it to our delay time
    gettimeofday(&tp, NULL);
    long new_nsec = tp.tv_usec * 1000 + (seconds - (long)seconds) * 1e9;
    timetoexpire.tv_sec = tp.tv_sec + (long)seconds + (new_nsec / (long)1e9);
    timetoexpire.tv_nsec = new_nsec % (long)1e9;

    pthread_mutex_lock(&mutex);
    int res = pthread_cond_timedwait(&conditionvar, &mutex, &timetoexpire);
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&conditionvar);

    //Upon successful completion, a value of zero shall be returned
    return res;
}


void create_new_thread(void *(func)(void* vargp)){
	pthread_create(&tid[thread_count], NULL, func, (void *)thread_count);
	thread_count++;
}

bool probabilityCheck(float p){
	bool flag = false;
	p *= 100.0f;
	float r = rand() % 101;
	if(r < p){
		flag = true;
		return flag;
	}else{
		return flag;
	}
}
