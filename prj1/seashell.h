
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
void binaryConversion(int a);


void print_colored(char* str, char color){
	char *start, *end = "\033[0m";
	start = (color == 'r') ? "\033[0;31m": (color == 'g') ? "\033[0;32m" : "\033[0;34m";
	printf("%s%s%s", start, str, end);
}

typedef struct builtin_cmd{
	char* name;
	int (*handler)(command_t*, conf_elm_t**);
	struct builtin_cmd *nxt;
}builtin_cmd_t;

builtin_cmd_t* create_builtin_cmd(const char* name, int (*handler)(command_t*, conf_elm_t**)){
	builtin_cmd_t *cmd = (builtin_cmd_t*)malloc(sizeof(builtin_cmd_t));
	cmd->name = (char*)malloc(sizeof(char)*strlen(name));
	strcpy(cmd->name, name);
	cmd->handler = handler;
	cmd->nxt = NULL;
	return cmd;
}

void add_cmd(builtin_cmd_t **cmd, const char* name, int (*handler)(command_t*, conf_elm_t**)){
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

int handle_cmd(builtin_cmd_t *head, command_t *cmd, conf_elm_t **elms){
	builtin_cmd_t *tmp = head;
	for(;(tmp != NULL) && (strcmp(tmp->name, cmd->name) != 0); tmp = tmp->nxt);
	if(tmp != NULL){
		return (*(tmp->handler))(cmd, elms);
	}
	return UNKNOWN;
}


int kdiff_handler(command_t * command, conf_elm_t **conf_elms){
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

int shortdir_handler(command_t* command, conf_elm_t **conf_elms){
	if (command->arg_count > 0 ){
		if (strcmp(command->args[0], "set") == 0){
			if(!(command->arg_count > 0))
				return SUCCESS;
			char cwd[1024];
			getcwd(cwd, sizeof(cwd));
			char **args = (char**)malloc(sizeof(char*) * 2);
			args[0] = (char*)malloc(sizeof(char)*(strlen(command->args[1]) + 1));
			args[1] = (char*)malloc(sizeof(char)*(strlen(cwd)+1));
			strcpy(args[0], command->args[1]);
			strcpy(args[1], cwd);
			add_conf_elm(conf_elms, "alias", args);
			printf("Alias set %s -> %s\n", args[0], args[1]);
			return SUCCESS;
		}else if(strcmp(command->args[0], "jump") == 0){
			if(!(command->arg_count > 1))
				return SUCCESS;

			char *path = get_conf(conf_elms, "alias", command->args[1])[0];
			if(path != NULL){
				int r = chdir(path);
				printf("chdir to %s : %d\n", path, r);
			}
			return SUCCESS;
		}else if(strcmp(command->args[0], "del") == 0){
			if(!(command->arg_count > 1))
				return EXIT;
			rm_config(conf_elms, "alias", command->args[1]);
			return SUCCESS;
		}else if(strcmp(command->args[0], "clear") == 0){
			rm_config(conf_elms, "alias", NULL);
			return SUCCESS;
		}else if(strcmp(command->args[0], "list") == 0){
			char **ls = get_conf(conf_elms, "alias", NULL);
			if(ls == NULL)
				return SUCCESS;
			char *name, *path;
			for(int i = 0;; i+=2){
				name = ls[i];
				if(name == NULL)
					break;
				path = ls[i+1];
				printf("Alias %s -> %s\n", ls[i], ls[i+1]);
			}
			return SUCCESS;
		}else{
			printf("usage: $shortdir (set|jump|del|clear|list) [options] \n");
			return SUCCESS;
		}
	}else{
		printf("wrong arg count\n");
		return SUCCESS;
	}
}
int highlight_handler(command_t* command, conf_elm_t **conf_elms){
	if(strcmp(command->name, "highlight") == 0){
		if(!(command->arg_count > 2)){
			printf("required 3 got %d args\n", command->arg_count);
			return SUCCESS;
		}
		char* word = command->args[0];
		char color = command->args[1][0];
		char* f_name = command->args[2];
		FILE* fp;
		fp = fopen(f_name, "r");
		char * line = NULL;
		size_t len = 0;
		ssize_t read;
		char * token;
		while ((read = getline(&line, &len, fp)) != -1){
			token = strtok(line, " ");
			while(token != NULL){
				(strcmp(word, token) == 0) ? print_colored(token, color) : printf("%s", token);
				int llen = strlen(token);
				if(llen > 0 && token[llen-1] != '\n')
					printf(" ");
				token = strtok(NULL, " ");
			}
		}
		return SUCCESS;
	} 
	}
int goodMorning_handler(command_t* command, conf_elm_t **conf_elms){
	if(strcmp(command->name, "goodMorning") == 0){
		int h, m;
		h = atoi(strtok(command->args[0], "."));
		m = atoi(strtok(NULL, "."));
		char cronjob[256];
		sprintf(cronjob, "%d %d * * * rhythmbox %s", m, h, command->args[1]);
		printf("%s\n", cronjob);
		return SUCCESS;
	}
	}
int tobin_handler(command_t* command, conf_elm_t **conf_elms){
	if(strcmp(command->name, "tobin")==0) {
		if (command->arg_count > 0) {
			if(strcmp(command->args[0], "conv") == 0){
				int a = atol(command->args[1]);
				binaryConversion(a);
				return SUCCESS;

		}
	}
	}
}

void binaryConversion(int a){
				int tmp = 0;
				int remainderArray[512];
				for(int i = 0; a > 0; i++ ){
					remainderArray[i] = a % 2 ;
					a = a/2;
					tmp = i ;

			}
				for(; tmp >= 0; tmp-- ){
					printf("%d", remainderArray[tmp]);
}
				printf("\n");
}


#endif
