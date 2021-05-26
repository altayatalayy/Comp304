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
mutex_t question_mutex;

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

	queue = createQueue(n+1);
   	//pthread_mutex_init(&mutex1, NULL);

	create_new_thread(moderator);
	for(threadID = 0; threadID<n; threadID++){
			create_new_thread(commmentator);
	}
	create_new_thread(bnews);

	join(0);
	_running = false;
  	for(int i = 1; i < n + 2; i++){
		join(i);
  	}

	freeQueue(queue);
	return 0;
}



size_t get_commentator(){
	return (size_t)dequeue(queue);
}

void* moderator(void *vargp){
	for(int i=0; i<q; i++){
		log("Moderator asks question %d", i);
		lock(question_mutex);
		//lock(answer_mutex);
		//questionAsked = true;
		//unlock(question_mutex);
		idx = get_commentator();
		if(idx != -1){
		//	unlock(answer_mutex);
		}
		log("idx = %zu", idx);
	}

	return NULL;
}

void* commmentator(void *vargp){
	size_t tid;
    tid = (size_t)vargp;
	float tmp = rand() % (size_t) t;
	if(probabilityCheck(p)){
		enqueue(queue, (int)tid);
		log("Commentator #%zu generates answer, position in queue:%d",tid, queue->size)
		//if(questionAsked){
		//	lock(answer_mutex);
		//}
		if(idx == tid){
			log("Commentator #%zu's turn to speak for %f seconds", tid , tmp);
			pthread_sleep(t);
			unlock(question_mutex);
			log("Commentator #%zu finished speaking",tid);

		}
		log("Commentator #%zu is cut short due to a breaking news", tid);
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


