#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "p1.h"

int main(void){
	start();
	run(part_a);
	return 0;
}

int part_a(){
	pid_t p;
	int level = 1;

	p = fork();
	if(p == 0)
		printf("Proccesss ID:%d, ParentID:%d, level:%d\n", (int)getpid(), (int)getppid(), level++);
	else if(p > 0)
		printf("Base Process ID: %d, level: 0\n", (int)getpid());


	p = fork();
	if(p == 0)
		printf("Proccesss ID:%d, ParentID:%d, level:%d\n", (int)getpid(), (int)getppid(), level++);

	p = fork();
	if(p == 0)
		printf("Proccesss ID:%d, ParentID:%d, level:%d\n", (int)getpid(), (int)getppid(), level++);

	p = fork();
	if(p == 0)
		printf("Proccesss ID:%d, ParentID:%d, level:%d\n", (int)getpid(), (int)getppid(), level++);

	return 0;
}
