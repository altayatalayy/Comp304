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


mutex_t mutex1,mutex2,mutex3;
mutex_t question_mutex, bnews_mutex;
mutex_t mutex[4], mutex_2[4];

Queue *queue;

bool _running = false;
int n, q, t;
float p, b;


int currentSize ;
int curr_com_id = -1;

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
	queue = createQueue(n);
	mutex_init(mutex1);
	mutex_init(mutex2);
	mutex_init(mutex3);
	mutex_init(question_mutex);
	mutex_init(bnews_mutex);

	for(int a = 0; a<4; a++){
		mutex_init(mutex[a]);
		mutex_init(mutex_2[a]);
		lock(mutex[a]);
		lock(mutex_2[a]);
	}

	create_new_thread(moderator);
	pthread_sleep(0.1);
	for(int threadID = 0; threadID<n; threadID++){
			create_new_thread(commmentator);

	}
	create_new_thread(bnews);

	join_all();
	freeQueue(queue);
	return 0;
}


#define get_commentator() (size_t)dequeue(queue)


void ask_question(){
	lock(mutex1);
	lock(bnews_mutex);
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
	unlock(bnews_mutex);
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
	float st = 0, dt = 0.1f;
	while(st < tmp){
		lock(bnews_mutex);
		if(st == 0.0f)
			log("Commentator #%d's turn to speak for %f seconds", tid , tmp);
		curr_com_id = tid;
		pthread_sleep(dt);
		st += dt;
		unlock(bnews_mutex);
	}
	curr_com_id = -1;
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
		}else{
			lock(mutex1);
			unlock(mutex1);
		}
	}
	//log("Commentator #%d is cut short due to a breaking news", tid);
	return NULL;
}


void* bnews(void *vargs){
	while(_running){
		if(probabilityCheck(b)){
			lock(bnews_mutex);
			log("Breaking News!");
			if(curr_com_id != -1)
				log("Commentator #%1d is cut short due to a breaking news", curr_com_id);
			pthread_sleep(5);
			log("Breaking news ends");
			unlock(bnews_mutex);
		}else{
			pthread_sleep(1);
		}
	}
	return NULL;
}


