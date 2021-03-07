#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "p1.h"

int main(void){
	start();
	run(part_c);
	return 0;
}

int part_c(){
	pid_t pid;
	if((pid = fork()) > 0){
		sleep(5);
	}
	exit(0);
	return 0;
}
