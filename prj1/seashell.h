
#ifndef __SEASHELL_H__
#define __SEASHELL_H__

#include "config.h"

const char * sysname = "seashell";

enum return_codes {
	SUCCESS = 0,
	EXIT = 1,
	UNKNOWN = 2,
};

typedef struct command_t {
	char *name;
	bool background;
	bool auto_complete;
	int arg_count;
	char **args;
	char *redirects[3]; // in/out redirection
	struct command_t *next; // for piping
}command_t;

void print_command(struct command_t * command);
int free_command(struct command_t *command);
int show_prompt(); int parse_command(char *buf, struct command_t *command);
void prompt_backspace();
int prompt(struct command_t *command);
int process_command(struct command_t *command);

void print_colored(char* str, char color){
	char *start, *end = "\033[0m";
	start = (color == 'r') ? "\033[0;31m": (color == 'g') ? "\033[0;32m" : "\033[0;34m";
	printf("%s%s%s", start, str, end);
}

typedef struct builtin_cmd{
	char* name;
	int (*handler)(command_t*);
	struct builtin_cmd *nxt;
}builtin_cmd_t;

builtin_cmd_t* create_builtin_cmd(const char* name, int (*handler)(command_t*)){
	builtin_cmd_t *cmd = (builtin_cmd_t*)malloc(sizeof(builtin_cmd_t));
	cmd->name = (char*)malloc(sizeof(char)*strlen(name));
	strcpy(cmd->name, name);
	cmd->handler = handler;
	cmd->nxt = NULL;
	return cmd;
}

void add_cmd(builtin_cmd_t **cmd, const char* name, int (*handler)(command_t*)){
	builtin_cmd_t *new_cmd = create_builtin_cmd(name, handler);
	if((*cmd) == NULL){
		*cmd = new_cmd;
		return;
	}
	builtin_cmd_t *nxt = (*cmd)->nxt, *last = *cmd;
	while(nxt != NULL){ last = nxt; nxt = nxt->nxt;}
	last->nxt = new_cmd;
	return;
}

int handle_cmd(builtin_cmd_t *head, command_t *cmd){
	builtin_cmd_t *tmp = head;
	for(;(tmp != NULL) && (strcmp(tmp->name, cmd->name) != 0); tmp = tmp->nxt);
	if(tmp != NULL){
		return (*(tmp->handler))(cmd);
	}
	return UNKNOWN;
}


int kdiff_handler(command_t * command){
	char* str = "-a";
	bool cleanup_flag = false;
	if(command->arg_count == 2){
		command->args[2] = command->args[1];
		command->args[1] = command->args[0];
		command->args[0] = str;
		command->arg_count++;
		cleanup_flag = true;
	}else if(command->arg_count != 3){
		printf("Wrong arg count\n");
		return SUCCESS;
	}
	if(strcmp(command->args[0], "-a") == 0){
		FILE *fp1, *fp2;
		char *fn1 = command->args[1], *fn2 = command->args[2];
		char * ext = strrchr(fn1, '.');
		if(strcmp(ext, ".txt") != 0){
			printf("file1 : required .txt got:%s\n", ext);
			return SUCCESS;
		}
		ext = strrchr(fn2, '.');
		if(strcmp(ext, ".txt")){
			printf("file2 : required .txt got:%s\n", ext);
			return SUCCESS;
		}
		fp1 = fopen(fn1, "r");
		fp2 = fopen(fn2, "r");
		char *line1 = NULL, *line2 = NULL;
		size_t len = 0;
		ssize_t read;
		char * token;
		int line_num = 0, count = 0;
		while ((read = getline(&line1, &len, fp1)) != -1 && (read = getline(&line2, &len, fp2)) != -1){
			if(strcmp(line1, line2) != 0){
				count++;
				printf("%s:Line %d: %s", fn1, line_num, line1);
				printf("%s:Line %d: %s", fn2, line_num, line2);
			}
			line_num++;
		}
		fclose(fp1);
		fclose(fp2);
		printf("%d different lines found\n", count);
		if(cleanup_flag){
			command->args[0] = command->args[1];
			command->args[1] = command->args[2];
			command->arg_count--;
		}
		return SUCCESS;
	}else if(strcmp(command->args[0], "-b") == 0){
		FILE *fp1, *fp2;
		char *fn1 = command->args[1], *fn2 = command->args[2];
		char * ext = strrchr(fn1, '.');
		if(strcmp(ext, ".bin") != 0){
			printf("file1 : required .txt got:%s\n", ext);
			return SUCCESS;
		}
		ext = strrchr(fn2, '.');
		if(strcmp(ext, ".bin")){
			printf("file2 : required .txt got:%s\n", ext);
			return SUCCESS;
		}fp1 = fopen(fn1, "rb");
		fp2 = fopen(fn2, "rb");
		unsigned char buf1[1], buf2[1];
		int count = 0;
		while (!feof(fp1) && !feof(fp2)){
			fread(buf1, sizeof(buf1), 1, fp1);
			fread(buf2, sizeof(buf2), 1, fp2);
			if(*buf1 != *buf2)
				count++;
		}
		fclose(fp1); fclose(fp2);
		if(count > 0)
			printf("The two files are different in %d bytes\n", count);
		else
			printf("The two files are identical\n");
		return SUCCESS;
	}else{
		printf("Usage $kdiff [-a|-b] file1 file2");
		return SUCCESS;
	}
}

int shortdir_handler(command_t* command){
	if (command->arg_count > 0 ){
		if (strcmp(command->args[0], "set") == 0){
			if(!(command->arg_count > 1))
				return SUCCESS;
			char cwd[1024];
			getcwd(cwd, sizeof(cwd));
			char **args = (char**)malloc(sizeof(char*) * 2);
			args[0] = (char*)malloc(sizeof(char)*strlen(command->args[1]));
			args[1] = (char*)malloc(sizeof(char)*strlen(cwd));
			strcpy(args[0], command->args[0]);
			strcpy(args[1], cwd);
			add_conf_elm(&conf_elms, "alias", args);
			printf("Alias set %s -> %s\n", command->args[1], cwd);
			return SUCCESS;
		}else if(strcmp(command->args[0], "jump") == 0){
			if(!(command->arg_count > 1))
				return SUCCESS;

			FILE* fp;
			fp = fopen(config_file, "r");
			char * line = NULL;
			size_t len = 0;
			ssize_t read;
			while ((read = getline(&line, &len, fp)) != -1){
				char* token = strtok(line, ":");
				if(strcmp(token, "alias") == 0){
					token = strtok(NULL, ":");
					if(strcmp(token, command->args[1]) == 0){
						token = strtok(NULL, ":");
						token = strtok(token, "\n");
						int r = chdir(token);
						printf("chdir to %s : %d\n", token, r);
						fclose(fp);
						return SUCCESS;
					}
				}
			}
		}else if(strcmp(command->args[0], "del") == 0){
			if(!(command->arg_count > 1))
				return EXIT;
			char* name = command->args[1];
			FILE* fp;
			fp = fopen(config_file, "rw");
			char * line = NULL;
			size_t len = 0;
			ssize_t read;
			char cpy[512];
			while ((read = getline(&line, &len, fp)) != -1){
				strcpy(cpy, line);
				char* token = strtok(cpy, ":");
				if(strcmp(token, "alias") == 0 && !strcmp(strtok(NULL, ":"), name))
					fprintf(fp, "%s", line);

			}
			return SUCCESS;
		}else if(strcmp(command->args[0], "clear") == 0){
			FILE* fp;
			fp = fopen(config_file, "w");
			fprintf(fp, "\n");
			return SUCCESS;
		}else if(strcmp(command->args[0], "list") == 0){
			FILE* fp;
			fp = fopen(config_file, "r");
			char * line = NULL;
			size_t len = 0;
			ssize_t read;
			char cpy[512];
			while ((read = getline(&line, &len, fp)) != -1){
				strcpy(cpy, line);
				char* token = strtok(cpy, ":");
				if(strcmp(token, "alias") == 0)
					printf("%s", line);
			}
			return SUCCESS;
		}else{
			printf("usage: $shortdir (set|jump|del|clear|list) [options]");
			return SUCCESS;
		}
	}else{
		printf("wrong arg count\n");
		return SUCCESS;
	}
}


#endif
