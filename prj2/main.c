#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "main.h"
#include "queue.h"
#include "log.h"
#include "util.h"

/**
 * pthread_sleep takes an integer number of seconds to pause the current thread
 * original by Yingwu Zhu
 * updated by Muhammed Nufail Farooqi
 * updated by Fahrican Kosar
 */


mutex_t mutex1;
mutex_t question_mutex, bnews_mutex;

Queue *queue;

bool _running = false;
int n, q, t;
float p, b;
size_t idx;
int threadID;
bool questionAsked;

int main(int argc, char* argv[]) {
	srand(time(0));
	char arg = '\0';

	while ((arg = getopt(argc, argv, "n:t:q:p:b:")) != -1) {
		switch (arg) {
			case 'n': n = atoi(optarg); break;
			case 't': t = atoi(optarg); break;
			case 'q': q = atoi(optarg); break;
			case 'p': p = atof(optarg); break;
			case 'b': b = atof(optarg); break;
			default: fprintf(stderr, "Usage: %s -n <int> -t <float> -q <int> -p <float>\n", argv[0]); exit(EXIT_FAILURE);
		}
	}
	printf("n = %d, q = %d, t = %d, p = %f, b = %f\n", n, q, t, p, b);

	_running = true;

	queue = createQueue(n+2);
   	if(pthread_mutex_init(&mutex1, NULL))
		return -1;
	if(pthread_mutex_init(&(question_mutex), NULL))
		return -1;
    if(pthread_mutex_init(&(bnews_mutex), NULL))
		return -1;

	create_new_thread(moderator);
	pthread_sleep(0.1);
	for(threadID = 0; threadID<n; threadID++){
			create_new_thread(commmentator);

	}
	create_new_thread(bnews);

	join(0);
  	for(int i = 1; i < n+1; i++){
		join(i);
  	}

	freeQueue(queue);
	return 0;
}



size_t get_commentator(){
	return (size_t)dequeue(queue);
}

void ask_question(){
	static int i = 0;
	if(i != 0) lock(question_mutex);
	log("Moderator asks question %d", i++);
	unlock(question_mutex);
	//wait for all answwers
	lock(mutex1);
	unlock(mutex1);
}

void get_answers(){
		while(!isEmpty(queue)){
			lock(question_mutex);
			idx = get_commentator();
			unlock(question_mutex);
		}
}

void* moderator(void *vargp){
	lock(question_mutex);
	for(int i=0; i<q; i++){
		//clear(queue);

		ask_question();
		get_answers();

		/*
		if(isEmpty(queue)){
			lock(question_mutex);
			questionAsked = false;
			unlock(question_mutex);
		}
		//log("idx = %zu", idx);
		*/
	}
	return NULL;
}

void wait_question(int tid){
	//wait for question to be asked
	lock(question_mutex);
	unlock(question_mutex);
}

void speak(int tid){
	float tmp = (float)(rand() % 100)/100.0f * (float)t;
	lock(question_mutex);
	log("Commentator #%d's turn to speak for %f seconds", tid , tmp);
	float st = 0, dt = 0.01f;
	while(st < tmp){
		pthread_sleep(dt);
		st += dt;
		//lock(bnews_mutex);
	}
	log("Commentator #%d finished speaking",tid);
	unlock(question_mutex);
}

void* commmentator(void *vargp){
	lock(mutex1);
	int tid;
    tid = (int)vargp;
    int n = 0;
	for(int i=0; i<q; i++){
		wait_question(tid);
		if(probabilityCheck(p)){
			int pos = enqueue(queue, tid);
			log("Commentator #%d generates answer, position in queue:%d",tid, pos);
			if(pos == n-1)
				unlock(mutex1);
		}
		if(tid == idx){
			speak(tid);
		}
	}
	//log("Commentator #%d is cut short due to a breaking news", tid);
	return NULL;
}


void* bnews(void *vargs){
	while(_running){
		//unlock(bnews_mutex);
		if(probabilityCheck(b)){
			//lock(bnews_mutex);
			log("Breaking News!");
			pthread_sleep(5);
			log("Breaking news ends");
			//unlock(bnews_mutex);
		}else{
			pthread_sleep(1);
		}
	}
	return NULL;
}


