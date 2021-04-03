
#ifndef __SEASHELL_CONFIG_H__
#define __SEASHELL_CONFIG_H__

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>


const char* config_file = "/home/umur/.seashellrc";
typedef unsigned long long hash_t;

typedef struct conf_elm{
	char *type;
	char **args;
	hash_t hash;
	struct conf_elm* next;
} conf_elm_t;

void save_config(conf_elm_t **head);

hash_t hash_str(unsigned char *str) {
	// sue djb2 for hashing a string
	hash_t hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

void hash_conf_elm(conf_elm_t *elm){
	hash_t *hash = (hash_t*)malloc(sizeof(hash_t));
	*hash = hash_str(elm->type) + hash_str(elm->args[0]);
	elm->hash = *hash;
}

conf_elm_t* create_conf_elm(char *type, char **args){
	conf_elm_t *elm = (conf_elm_t*)malloc(sizeof(conf_elm_t));
	elm->type = type;
	elm->args = args;
	hash_conf_elm(elm);
	elm->next = NULL;
	return elm;
}

void add_conf_elm(conf_elm_t **head, char *type, char **args){
	/*add the conf element to the linked list
	 * */
	conf_elm_t *nconf = create_conf_elm(type, args);
	if(*head == NULL){
		*head = nconf;
		save_config(head);
		return;
	}
	conf_elm_t *next = (*head)->next, *last = *head;
	while(next != NULL && (nconf->hash != last->hash)){ last = next; next = next->next;}
	if (next == NULL && (nconf->hash != last->hash))
		last->next = nconf;
	else
		last->args = nconf->args;
	save_config(head);
}

void load_config(conf_elm_t **head){
	/*Load config elements from the file
	 *For handling race conditions lock the file flock using fcntl
	 * */
	struct flock fl = {F_RDLCK, SEEK_SET, 0, 0, getpid()};
	int fd = open(config_file, O_RDONLY | O_CREAT);
	if(fd == -1){
		perror("Cannot open config file");
		exit(1);
	}
	if (fcntl(fd, F_SETLKW, &fl) == -1) {
		perror("fcntl");
		exit(1);
	}

	FILE *fp = fdopen(fd, "r");
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	char *type, **args;
	while((read = getline(&line, &len, fp)) != -1){
		if(line == NULL || strcmp(line, "") == 0 || strcmp(line, "\n") == 0){
			continue;
		}
		if(line[strlen(line)-1] == '\n'){
			line[strlen(line)-1] = '\0';
		}
		args = (char**)malloc(sizeof(char*) * 2);
		char* t = strtok(line, ":");
		type = (char*)malloc(sizeof(char) * (strlen(t)+1));
		char *arg0 = strtok(NULL, ":");
		args[0] = (char*)malloc(sizeof(char) * (strlen(arg0)+1));
		char *arg1 = strtok(NULL, ":");
		args[1] = (char*)malloc(sizeof(char) * (strlen(arg1) + 1));
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

	close(fd);
}

void save_config(conf_elm_t **head){
	/*Save conf elements into config file
	 *To avoid race conditions first acquire the file lock using flock
	 * */
	struct flock fl = {F_WRLCK, SEEK_SET, 0, 0, getpid()};
	int fd = open(config_file, O_WRONLY | O_CREAT);
	if(fd == -1){
		perror("Cannot open config file");
		exit(1);
	}
	if (fcntl(fd, F_SETLKW, &fl) == -1) {
		perror("fcntl");
		exit(1);
	}

	FILE *fp = fdopen(fd, "w");
	fp = freopen(NULL, "w", fp);
	if(*head == NULL){
		//Nothing to save
		return;
	}
	else{
		conf_elm_t *backup = *head;
		for(conf_elm_t *elm = *head; elm != NULL; elm = elm->next){

			fprintf(fp, "%s:%s:%s\n", elm->type, elm->args[0], elm->args[1]);
		}
	}


	fl.l_type = F_UNLCK;  /* set to unlock same region */

	if (fcntl(fd, F_SETLK, &fl) == -1) {
		perror("fcntl");
		exit(1);
	}

	fclose(fp);
	close(fd);
}

char** get_conf(conf_elm_t **head, char *type, char *name){
	if(*head == NULL){
		return NULL;
	}
	if(name == NULL){
		char **rv = (char**)malloc(sizeof(char*)*512);
		int i = 0;
		for(conf_elm_t *tmp = *head; tmp != NULL; i+=2, tmp = tmp->next){
			if(strcmp(tmp->type, type) == 0){
				rv[i] = tmp->args[0];
				rv[i+1]  = tmp->args[1];
			}
		}
		rv[i] = NULL;
		return rv;
	}else{
		hash_t hash = hash_str(type) + hash_str(name);
		for(conf_elm_t *tmp = *head; tmp != NULL; tmp = tmp->next){
			if(tmp->hash == hash){
				return &(tmp->args[1]);
			}
		}
	}
	return NULL;
}

void rm_config(conf_elm_t **head, char *type, char *name){
	/*Delete element from linked list
	 * if name is Null delete all elements with type = type
	 * */
	if(*head == NULL){
		return;
	}
	conf_elm_t *prev;
	if(name == NULL){
		prev = *head;
		for(conf_elm_t *tmp = *head; tmp != NULL;){
			if(strcmp(tmp->type, type) == 0){
				if(tmp->next == NULL){
					if(tmp == prev){
						*head = NULL;
					}else{
						prev->next = NULL;
					}

				}else{
					if(tmp == prev){
						*head = tmp->next;
						tmp = *head;
						prev = tmp;
						continue;
					}else{
						prev->next = tmp->next;
					}
				}
			}
			prev = tmp;
			tmp = tmp->next;
		}
	}else{
		hash_t hash = hash_str(type) + hash_str(name);
		prev = *head;
		for(conf_elm_t *tmp = *head; tmp != NULL;){
			if(tmp->hash == hash){
				if(tmp->next == NULL){
					if(tmp == prev)
						*head = NULL;
					else
						prev->next = NULL;
				}else{
					if(tmp == prev){
						*head = tmp->next;
						tmp = *head;
						prev = tmp;
						continue;
					}else{
						prev->next = tmp->next;
					}
				}
			}
			prev = tmp;
			tmp = tmp->next;
		}
	}
	save_config(head);
}

#endif
