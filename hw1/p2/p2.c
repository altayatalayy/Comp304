
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

#include "p2.h"

int main(void){

	pid_t pid;
	int fd[2];
	int status;
	pipe(fd);
	pid = fork();
	if(pid == 0){
		get_time(fd[0], 'B');
		sleep(3);
		pid = fork();
		if(pid == 0){
			get_time(fd[0], 'C');
			sleep(3);
			send_time(fd[1]);
			printf("Process C: blocking indefinetly\n");
			sleep(100);
		}else{
			send_time(fd[1]);
			sleep(1);//race condition
			get_time(fd[0], 'B');
			sleep(3);
			send_time(fd[1]);
			sleep(0.01);
			printf("Process B: Killing Proccess C\n");
			kill(pid, 9);
			printf("Process B: blocking indefinetly\n");
			sleep(100);
		}

	}else{
		send_time(fd[1]);
		sleep(8);
		get_time(fd[0], 'A');
		printf("Process A: Killing Proccess B\n");
		kill(pid, 9);
	}

	return 0;
}

void send_time(int fd){
	time_t t;
	char cur_time[50];
	time(&t);
	strcpy(cur_time, ctime(&t));
	write(fd, cur_time, strlen(cur_time)+1);
	return;
}

void get_time(int fd, char p_name){
	char cur_time[50];
	read(fd, cur_time, sizeof(cur_time));
	printf("Process %c :%s", p_name, cur_time);
}

