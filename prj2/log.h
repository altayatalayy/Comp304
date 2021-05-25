
#include <stdarg.h>

#ifndef __LOG_H__
#define __LOG_H__

#include <sys/time.h>

#define VA_ARGS(...) , ##__VA_ARGS__

char* get_time(){
	char *rv = (char*)malloc(sizeof(char) * 512);
	struct timeval te;
	gettimeofday(&te, NULL); // get current time
	long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
	sprintf(rv, "[%lld:%lld.%lld]", (milliseconds/1000/60)%60, (milliseconds/1000)%60, milliseconds%1000);
	return rv;
}

#define log(fmt, ...) {\
	char* time = get_time();\
	printf("%s " fmt "\n", time, __VA_ARGS__);\
	free(time);}

#endif
