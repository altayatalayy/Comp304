#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "p1.h"

int main(void){
	start();
	run(part_b);
	return 0;
}

int part_b(){
	pid_t pid;
	pid = fork();
	if(pid == 0){
		char *args[]={"/bin/ps", "-f", NULL};
		execv(args[0], args);
		printf("Child finished execution\n");//This should not run
	}

	wait(NULL);
	return 0;
}
