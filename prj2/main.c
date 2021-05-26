#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "main.h"
#include "queue.h"
#include "log.h"

/**
 * pthread_sleep takes an integer number of seconds to pause the current thread
 * original by Yingwu Zhu
 * updated by Muhammed Nufail Farooqi
 * updated by Fahrican Kosar
 */


mutex_t mutex1;
mutex_t question_mutex;

Queue *queue;

int n, q;
float t, p, b;
bool _running = false;

int main(int argc, char* argv[]) {
	char arg = '\0';

	while ((arg = getopt(argc, argv, "n:t:q:p:b:")) != -1) {
		switch (arg) {
			case 'n': n = atoi(optarg); break;
			case 't': t = atof(optarg); break;
			case 'q': q = atoi(optarg); break;
			case 'p': p = atof(optarg); break;
			case 'b': b = atof(optarg); break;
			default: fprintf(stderr, "Usage: %s -n <int> -t <float> -q <int> -p <float>\n", argv[0]); exit(EXIT_FAILURE);
		}
	}
	printf("n = %d, q = %d, t = %f, p = %f, b = %f\n", n, q, t, p, b);

	_running = true;

	queue = createQueue(n+1);
   	//pthread_mutex_init(&mutex1, NULL);

	create_new_thread(moderator);
	for(int i = 0; i<n; i++){
			create_new_thread(commmentator);
	}
	create_new_thread(bnews);

	pthread_join(tid[0], NULL);
	_running = false;
  	for(int i = 1; i < thread_count; i++){
  	     pthread_join(tid[i], NULL);
  	}

	freeQueue(queue);
	return 0;
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
	pthread_create(&tid[thread_count++], NULL, func, NULL);
}

size_t get_commentator(){
	return dequeue(queue);
}

void* moderator(void *vargp){
	for(int i=0; i<q; i++){
		log("Moderator asks question %d", i);
		lock(question_mutex);
		size_t idx = get_commentator();
		//printf("idx = %d\n", idx);
		//wait_commentator(idx);
		unlock(question_mutex);
	}

	return NULL;
}

void* commmentator(void *vargp){
	if(probabilityCheck(p)){
		//get_in_queue();
		lock(question_mutex);
		enqueue(queue, (int)pthread_self());
		unlock(question_mutex);
	}
	return NULL;
}

void* bnews(void *vargs){
	while(_running){
		if(probabilityCheck(b)){
			log("Breaking News!");
			pthread_sleep(5);
			log("Breaking news ends");
		}else{
			pthread_sleep(1);
		}
	}
	return NULL;
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

//bool questionReady = false;
//pthread_cond_t conditionvar;

//queue new queue answerQueue
//int questionNumber = 50;


