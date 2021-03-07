
#ifndef P1_H
#define P1_H

int part_a(void);
int part_b(void);
int part_c(void);

#define start() int rv;\
	pid_t child_pid, wpid, pid = getpid();\
	int status = 0;\

#define run(func) printf("[+]Running: %s\n", (#func));\
	rv = func();while ((wpid = wait(&status)) > 0);\
	if(getpid() != pid)\
		exit(0);\
	(rv == 0)?printf("[+]%s returned succesfully\n", (#func)):printf("[-]returned: %d", rv)

#endif
