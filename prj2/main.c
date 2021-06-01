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


mutex_t mutex1,mutex2,mutex3,mutex4;
mutex_t question_mutex, bnews_mutex;
mutex_t mutex[4];

Queue *queue;

bool _running = false;
int n, q, t;
float p, b;
size_t idx;
int threadID;
bool questionAsked;

int main(int argc, char* argv[]) {
	srand(420);
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
	if(pthread_mutex_init(&mutex2, NULL))
		return -1;
	if(pthread_mutex_init(&mutex3, NULL))
		return -1;
	if(pthread_mutex_init(&mutex4, NULL))
		return -1;
	if(pthread_mutex_init(&(question_mutex), NULL))
		return -1;
	if(pthread_mutex_init(&(bnews_mutex), NULL))
		return -1;

	for(int a = 0; a<4; a++){
		if(pthread_mutex_init(&mutex[a], NULL))
		return -1;
		lock(mutex[a]);
	}

	lock(mutex1);
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
	//if(i != 0) lock(question_mutex);
	log("Moderator asks question %d", i++);
	//unlock(question_mutex);
	//wait for all answwers
	unlock(question_mutex);
	lock(mutex1);

	//printf("INSIDE ask_question");
	unlock(mutex1);

}

void get_answers(){
		while(!isEmpty(queue)){
		idx = get_commentator();
		unlock(mutex[idx]);
		}
}

void* moderator(void *vargp){
	lock(question_mutex);
	lock (mutex2);
	for(int i=0; i<q; i++){
		//clear(queue);

		ask_question();
		//printf("After ask_question");
		get_answers();
		//printf("After ask_question2");

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
void wait_speak(){
	//wait for question to be asked
	lock(mutex2);
	unlock(mutex2);
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
	
	int tid;
    tid = (int)vargp;
	for(int i=0; i<q; i++){
		wait_question(tid);
		pthread_sleep(0.01);
		if(probabilityCheck(1)){
			int pos = enqueue(queue, tid);
			log("Commentator #%d generates answer, position in queue:%d",tid, pos);
			//printf("%d,     %d \n",pos,n-1);
			if(pos == n-1){
				unlock(mutex1);
				lock(question_mutex);
				//printf("After lock mutex1\n");
			}
		}
		lock(mutex1);
		//printf("After lock mutex1\n");
		unlock(mutex1);
		//printf("After unlock mutex1\n");
		//wait_speak(mutex2);
		lock(mutex[tid]);
		speak(tid);
		unlock(mutex[tid]);
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


