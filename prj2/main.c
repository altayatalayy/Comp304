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
mutex_t mutex[4], mutex_2[4], mutex_3[4];

Queue *queue;

bool _running = false;
int n, q, t;
float p, b;
int threadID;

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
		if(pthread_mutex_init(&mutex_2[a], NULL))
			return -1;
		lock(mutex_2[a]);
		if(pthread_mutex_init(&mutex_3[a], NULL))
			return -1;
		lock(mutex_3[a]);

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



#define get_commentator() (size_t)dequeue(queue)

int currentSize ;

void ask_question(){
	lock(mutex3);
	static int i = 0;
	log("Moderator asks question %d", i++);
	//wait for all answwers
	unlock(question_mutex);
	pthread_sleep(0.5);
	currentSize = getSize(queue);
	unlock(mutex3);
	lock(mutex1);
	unlock(mutex1);

}

void get_answers(){
		size_t idx;
		while(!isEmpty(queue)){
			idx = get_commentator();
			unlock(mutex[idx-1]);
			unlock(mutex2);
			lock(mutex_2[idx-1]);
		}
}

void* moderator(void *vargp){
	lock(question_mutex);
	lock (mutex2);
	for(int i=0; i<q; i++){
		//clear(queue);

		ask_question();
		get_answers();
	}
	_running = false;
	return NULL;
}

void wait_question(int tid){
	//wait for question to be asked
	lock(question_mutex);
	unlock(question_mutex);
}
void waitForQueueFill(){
	//wait for queue to be filled.
	lock(mutex3);
	unlock(mutex3);
}


void speak(int tid){
	float tmp = (float)(rand() % 100)/100.0f * (float)t;
	lock(mutex2);
	log("Commentator #%d's turn to speak for %f seconds", tid , tmp);
	float st = 0, dt = 0.01f;
	while(st < tmp){
		pthread_sleep(dt);
		st += dt;
		//lock(bnews_mutex);
	}
	log("Commentator #%d finished speaking",tid);
	unlock(mutex2);
}

void* commmentator(void *vargp){
	int tid;
    tid = (int)vargp;
	for(int i=0; i<q; i++){
		wait_question(tid);
		if(probabilityCheck(p)){
			int pos = enqueue(queue, tid);
			log("Commentator #%d generates answer, position in queue:%d",tid, pos);
			waitForQueueFill();
			//printf("%d,     %d \n",pos,currentSize);
			if(pos == currentSize-1){
				unlock(mutex1);
				lock(question_mutex);
			}
		
			lock(mutex1);
			unlock(mutex1);
			lock(mutex[tid-1]);
			speak(tid);
			unlock(mutex_2[tid-1]);
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
	printf("Asd\n\n\n");
	return NULL;
}


