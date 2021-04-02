
#ifndef __SEASHELL_CONFIG_H__
#define __SEASHELL_CONFIG_H__

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>


static const char* config_file = "/home/altay/.seashell.rc";
typedef unsigned long long hash_t;

typedef struct conf_elm{
	char *type;
	char **args;
	hash_t hash;
	struct conf_elm* next;
} conf_elm_t;

hash_t hash_str(unsigned char *str) {
	// sue djb2 for hashing a string
	hash_t hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

void hash_conf_elm(conf_elm_t *elm){
	hash_t hash = hash_str(elm->type) + hash_str(elm->args[0]);
	printf("%lld\n", hash);
	elm->hash = hash;
}

conf_elm_t* create_conf_elm(char *type, char **args){
	conf_elm_t *elm = (conf_elm_t*)malloc(sizeof(conf_elm_t));
	elm->type = type;
	elm->args = args;
	hash_conf_elm(elm);
	elm->next = NULL;
}

void add_conf_elm(conf_elm_t **head, char *type, char **args){
	conf_elm_t *nconf = create_conf_elm(type, args);
	if((*head) == NULL){
		*head = nconf;
		return;
	}
	conf_elm_t *next = (*head)->next, *last = *head;
	while(next != NULL && (nconf->hash != last->hash)){ last = next; next = next->next;}
	if (next == NULL && (nconf->hash != last->hash))
		last->next = nconf;
	else
		last->args = nconf->args;
}

void load_config(conf_elm_t **head){
	struct flock fl = {F_RDLCK, SEEK_SET, 0, 0, getpid()};
	int fd = open(config_file, O_RDONLY);
	if(fd == -1){
		perror("Cannot open config file");
		exit(1);
	}
	if (fcntl(fd, F_SETLKW, &fl) == -1) {
		perror("fcntl");
		exit(1);
	}

	printf("config file got lock\n");
	FILE *fp = fdopen(fd, "r");
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	char *type, **args;
	while((read = getline(&line, &len, fp)) != -1){
		args = (char**)malloc(sizeof(char*) * 2);
		char* t = strtok(line, ":");
		type = (char*)malloc(sizeof(char) * strlen(t));
		char *arg0 = strtok(NULL, ":");
		args[0] = (char*)malloc(sizeof(char) * strlen(arg0));
		char *arg1 = strtok(NULL, ":");
		args[1] = (char*)malloc(sizeof(char) * strlen(arg1));
		strcpy(type, t);
		strcpy(args[0], arg0);
		strcpy(args[1], arg1);
		add_conf_elm(head, type, args);
	}


	fl.l_type = F_UNLCK;  /* set to unlock same region */

	if (fcntl(fd, F_SETLK, &fl) == -1) {
		perror("fcntl");
		exit(1);
	}

	printf("config file Unlocked.\n");

	close(fd);
}

void save_config(conf_elm_t **head){
	struct flock fl = {F_WRLCK, SEEK_SET, 0, 0, getpid()};
	int fd = open(config_file, O_WRONLY);
	if(fd == -1){
		perror("Cannot open config file");
		exit(1);
	}
	if (fcntl(fd, F_SETLKW, &fl) == -1) {
		perror("fcntl");
		exit(1);
	}

	printf("config file got lock\n");
	FILE *fp = fdopen(fd, "w");
	if(*head == NULL)
		fprintf(fp, "\n");
	else{
		for(conf_elm_t elm = *head; elm != NULL, elm = elm->next){
			if(elm->args[1][strlen(elm->args[1])-2] == '\n')
				elm->args[1][strlen(elm->args[1])-2] = '\0'
			fprintf(fp, "%s:%s:%s\n", elm->type, elm->args[0], elm->args[1]);
		}
	}

	fl.l_type = F_UNLCK;  /* set to unlock same region */

	if (fcntl(fd, F_SETLK, &fl) == -1) {
		perror("fcntl");
		exit(1);
	}

	printf("config file Unlocked.\n");

	close(fd);

}

void get_conf(conf_elm_t **head){

}
#endif
