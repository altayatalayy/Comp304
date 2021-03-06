#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>            //termios, TCSANOW, ECHO, ICANON
#include <string.h>
#include <stdbool.h>
#include <errno.h>

const char * sysname = "seashell";

enum return_codes {
	SUCCESS = 0,
	EXIT = 1,
	UNKNOWN = 2,
};
struct command_t {
	char *name;
	bool background;
	bool auto_complete;
	int arg_count;
	char **args;
	char *redirects[3]; // in/out redirection
	struct command_t *next; // for piping
};
/**
 * Prints a command struct
 * @param struct command_t *
 */
void print_command(struct command_t * command) {

#include "config.h"
#include "seashell.h"

builtin_cmd_t *builtin_cmds = NULL;
conf_elm_t *conf_elms = NULL;

int main() {
	load_config(&conf_elms);

	add_cmd(&builtin_cmds, "kdiff", kdiff_handler);
	add_cmd(&builtin_cmds, "shortdir", shortdir_handler);
	add_cmd(&builtin_cmds, "highlight", highlight_handler);
	add_cmd(&builtin_cmds, "goodMorning", goodMorning_handler);
	add_cmd(&builtin_cmds, "tobin", tobin_handler);
	while (1) {
		load_config(&conf_elms);
		struct command_t *command=malloc(sizeof(struct command_t));
		memset(command, 0, sizeof(struct command_t)); // set all bytes to 0

		int code;
		code = prompt(command);
		if (code==EXIT) break;

		code = process_command(command);
		if (code==EXIT) break;

		free_command(command);
	}
	printf("\n");
	return 0;
}

int process_command(struct command_t *command) {
	int r;
	if (strcmp(command->name, "")==0) return SUCCESS;

	if (strcmp(command->name, "exit")==0)
		return EXIT;

	if (strcmp(command->name, "cd")==0) {
		if (command->arg_count > 0) {
			r=chdir(command->args[0]);
			if (r==-1)
				printf("-%s: %s: %s\n", sysname, command->name, strerror(errno));
			return SUCCESS;
		}
	}

	int rv = handle_cmd(builtin_cmds, command, &conf_elms);
	//printf("seashell c conelms == NULL %d\n", conf_elms == NULL);
	if(rv != UNKNOWN){
		return rv;
	}

	/* if(strcmp(command->name, "highlight") == 0){
		if(!(command->arg_count > 3)){
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
	if(strcmp(command->name, "goodMorning") == 0){
		int h, m;
		h = atoi(strtok(command->args[0], "."));
		m = atoi(strtok(NULL, "."));
		char cronjob[256];
		sprintf(cronjob, "%d %d * * * rhythmbox %s", m, h, command->args[1]);
		printf("%s\n", cronjob);
		return SUCCESS;
	}
	*/


	pid_t pid=fork();
	if (pid==0){
		/// This shows how to do exec with environ (but is not available on MacOs)
		// extern char** environ; // environment variables
		// execvpe(command->name, command->args, environ); // exec+args+path+environ

		/// This shows how to do exec with auto-path resolve
		// add a NULL argument to the end of args, and the name to the beginning
		// as required by exec

		// increase args size by 2
		command->args=(char **)realloc( command->args, sizeof(char *)*(command->arg_count+=2));

		// shift everything forward by 1
		for (int i=command->arg_count-2;i>0;--i)
			command->args[i]=command->args[i-1];

		// set args[0] as a copy of name
		command->args[0]=strdup(command->name);
		// set args[arg_count-1] (last) to NULL
		command->args[command->arg_count-1]=NULL;

		//execvp(command->name, command->args); // exec+args+path
		//exit(0);
		/// TODO: do your own exec with path resolving using execv()
		char *path2;
		char *token2;
		char **pathArray;

		path2 = getenv("PATH");
		int abc = 0;
		pathArray = (char **)malloc(sizeof(char *));
		token2 = strtok (path2, ":" );
		while (token2 != NULL){
			pathArray[abc] = token2;
			abc = abc + 1;
			token2 = strtok (NULL, ":");
		}
		for(int i = 0 ; i < abc; ++i){
			char buf[512];
			sprintf(buf, "%s/%s", pathArray[i], command->name);
			int tmp1 = access(buf,X_OK);
				if(tmp1==0){
					execv(buf, command->args);
					exit(0);
				}
		}




	}
	else {
		if (!command->background)
			wait(0); // wait for child process to finish
		return SUCCESS;
	}

	// TODO: your implementation here

	printf("-%s: %s: command not found\n", sysname, command->name);
	return UNKNOWN;
}

void print_command(struct command_t * command) {
	/**
	 * Prints a command struct
	 * @param struct command_t *
	 */
>>>>>>> 564ca239560e91074eabdd1e84b9693b0ae33634
	int i=0;
	printf("Command: <%s>\n", command->name);
	printf("\tIs Background: %s\n", command->background?"yes":"no");
	printf("\tNeeds Auto-complete: %s\n", command->auto_complete?"yes":"no");
	printf("\tRedirects:\n");
	for (i=0;i<3;i++)
		printf("\t\t%d: %s\n", i, command->redirects[i]?command->redirects[i]:"N/A");
	printf("\tArguments (%d):\n", command->arg_count);
	for (i=0;i<command->arg_count;++i)
		printf("\t\tArg %d: %s\n", i, command->args[i]);
	if (command->next)
	{
		printf("\tPiped to:\n");
		print_command(command->next);
	}

<<<<<<< HEAD

}
/**
 * Release allocated memory of a command
 * @param  command [description]
 * @return         [description]
 */
int free_command(struct command_t *command) {
=======
}

int free_command(struct command_t *command) {
	/**
	 * Release allocated memory of a command
	 * @param  command [description]
	 * @return         [description]
	 */
>>>>>>> 564ca239560e91074eabdd1e84b9693b0ae33634
	if (command->arg_count) {
		for (int i=0; i<command->arg_count; ++i)
			free(command->args[i]);
		free(command->args);
	}
	for (int i=0;i<3;++i)
		if (command->redirects[i])
			free(command->redirects[i]);
	if (command->next) {
		free_command(command->next);
		command->next=NULL;
	}
	free(command->name);
	free(command);
	return 0;
}
<<<<<<< HEAD
/**
 * Show the command prompt
 * @return [description]
 */
int show_prompt() {
	char cwd[1024], hostname[1024];
    gethostname(hostname, sizeof(hostname));
=======

int show_prompt() {
	/**
	 * Show the command prompt
	 * @return [description]
	 */
	char cwd[1024], hostname[1024];
	gethostname(hostname, sizeof(hostname));
>>>>>>> 564ca239560e91074eabdd1e84b9693b0ae33634
	getcwd(cwd, sizeof(cwd));
	printf("%s@%s:%s %s$ ", getenv("USER"), hostname, cwd, sysname);
	return 0;
}
<<<<<<< HEAD
/**
 * Parse a command string into a command struct
 * @param  buf     [description]
 * @param  command [description]
 * @return         0
 */
int parse_command(char *buf, struct command_t *command) {
	const char *splitters=" \t"; // split at whitespace
	int index, len;
	len=strlen(buf);
=======

int parse_command(char *buf, struct command_t *command) {
	/**
	 * Parse a command string into a command struct
	 * @param  buf     [description]
	 * @param  command [description]
	 * @return         0
	 */
	const char *splitters = " \t"; // split at whitespace
	int index, len;
	len = strlen(buf);
>>>>>>> 564ca239560e91074eabdd1e84b9693b0ae33634
	while (len>0 && strchr(splitters, buf[0])!=NULL){ // trim left whitespace
		buf++;
		len--;
	}
	while (len>0 && strchr(splitters, buf[len-1])!=NULL)
		buf[--len]=0; // trim right whitespace

	if (len>0 && buf[len-1]=='?') // auto-complete
		command->auto_complete=true;
	if (len>0 && buf[len-1]=='&') // background
		command->background=true;

	char *pch = strtok(buf, splitters);
<<<<<<< HEAD
	command->name=(char *)malloc(strlen(pch)+1);
=======
	command->name = (char *)malloc(strlen(pch)+1);
>>>>>>> 564ca239560e91074eabdd1e84b9693b0ae33634
	if (pch==NULL)
		command->name[0]=0;
	else
		strcpy(command->name, pch);

<<<<<<< HEAD
	command->args=(char **)malloc(sizeof(char *));
=======
	command->args = (char **)malloc(sizeof(char *));
>>>>>>> 564ca239560e91074eabdd1e84b9693b0ae33634

	int redirect_index;
	int arg_index=0;
	char temp_buf[1024], *arg;
	while (1) {
		// tokenize input on splitters
		pch = strtok(NULL, splitters);
		if (!pch) break;
<<<<<<< HEAD
		arg=temp_buf;
		strcpy(arg, pch);
		len=strlen(arg);
=======
		arg = temp_buf;
		strcpy(arg, pch);
		len = strlen(arg);
>>>>>>> 564ca239560e91074eabdd1e84b9693b0ae33634

		if (len==0) continue; // empty arg, go for next
		while (len>0 && strchr(splitters, arg[0])!=NULL){ // trim left whitespace
			arg++;
			len--;
		}
		while (len>0 && strchr(splitters, arg[len-1])!=NULL) arg[--len]=0; // trim right whitespace
		if (len==0) continue; // empty arg, go for next

		// piping to another command
		if (strcmp(arg, "|")==0) {
			struct command_t *c=malloc(sizeof(struct command_t));
			int l=strlen(pch);
			pch[l]=splitters[0]; // restore strtok termination
			index=1;
			while (pch[index]==' ' || pch[index]=='\t') index++; // skip whitespaces

			parse_command(pch+index, c);
			pch[l]=0; // put back strtok termination
			command->next=c;
			continue;
		}

		// background process
		if (strcmp(arg, "&")==0)
			continue; // handled before

		// handle input redirection
		redirect_index=-1;
		if (arg[0]=='<')
			redirect_index=0;
		if (arg[0]=='>') {
			if (len>1 && arg[1]=='>') {
				redirect_index=2;
				arg++;
				len--;
			}
			else redirect_index=1;
		}
		if (redirect_index != -1) {
<<<<<<< HEAD
			command->redirects[redirect_index]=malloc(len);
=======
			command->redirects[redirect_index] = malloc(len);
>>>>>>> 564ca239560e91074eabdd1e84b9693b0ae33634
			strcpy(command->redirects[redirect_index], arg+1);
			continue;
		}

		// normal arguments
		if (len>2 && ((arg[0]=='"' && arg[len-1]=='"') || (arg[0]=='\'' && arg[len-1]=='\''))){ // quote wrapped arg
			arg[--len]=0;
			arg++;
		}
		command->args=(char **)realloc(command->args, sizeof(char *)*(arg_index+1));
		command->args[arg_index]=(char *)malloc(len+1);
		strcpy(command->args[arg_index++], arg);
	}
	command->arg_count=arg_index;
	return 0;
}
<<<<<<< HEAD
=======

>>>>>>> 564ca239560e91074eabdd1e84b9693b0ae33634
void prompt_backspace() {
	putchar(8); // go back 1
	putchar(' '); // write empty over
	putchar(8); // go back 1 again
}
<<<<<<< HEAD
/**
 * Prompt a command from the user
 * @param  buf      [description]
 * @param  buf_size [description]
 * @return          [description]
 */
int prompt(struct command_t *command) {
=======

int prompt(struct command_t *command) {
	/**
	 * Prompt a command from the user
	 * @param  buf      [description]
	 * @param  buf_size [description]
	 * @return          [description]
	 */
>>>>>>> 564ca239560e91074eabdd1e84b9693b0ae33634
	int index=0;
	char c;
	char buf[4096];
	static char oldbuf[4096];

<<<<<<< HEAD
    // tcgetattr gets the parameters of the current terminal
    // STDIN_FILENO will tell tcgetattr that it should write the settings
    // of stdin to oldt
    static struct termios backup_termios, new_termios;
    tcgetattr(STDIN_FILENO, &backup_termios);
    new_termios = backup_termios;
    // ICANON normally takes care that one line at a time will be processed
    // that means it will return if it sees a "\n" or an EOF or an EOL
    new_termios.c_lflag &= ~(ICANON | ECHO); // Also disable automatic echo. We manually echo each char.
    // Those new settings will be set to STDIN
    // TCSANOW tells tcsetattr to change attributes immediately.
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);


    //FIXME: backspace is applied before printing chars
	show_prompt();
	int multicode_state=0;
	buf[0]=0;
  	while (1) {
		c=getchar();
		// printf("Keycode: %u\n", c); // DEBUG: uncomment for debugging

		if (c==9){ // handle tab
			buf[index++]='?'; // autocomplete
			break;
		}

		if (c==127){ // handle backspace
=======
	// tcgetattr gets the parameters of the current terminal
	// STDIN_FILENO will tell tcgetattr that it should write the settings
	// of stdin to oldt
	static struct termios backup_termios, new_termios;
	tcgetattr(STDIN_FILENO, &backup_termios);
	new_termios = backup_termios;
	// ICANON normally takes care that one line at a time will be processed
	// that means it will return if it sees a "\n" or an EOF or an EOL
	new_termios.c_lflag &= ~(ICANON | ECHO); // Also disable automatic echo. We manually echo each char.
	// Those new settings will be set to STDIN
	// TCSANOW tells tcsetattr to change attributes immediately.
	tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);


	//FIXME: backspace is applied before printing chars
	show_prompt();
	int multicode_state = 0;
	buf[0] = 0;
  	while (1) {
		c = getchar();
		if(c == 10 && buf[0] == 0){//Empty line
			printf("\n");
			show_prompt();
			continue;
		}

		if (c == 9){ // handle tab
			buf[index++]='?'; // autocomplete
			break;
		}
		if (c == 127){ // handle backspace
>>>>>>> 564ca239560e91074eabdd1e84b9693b0ae33634
			if (index>0) {
				prompt_backspace();
				index--;
			}
			continue;
		}
<<<<<<< HEAD
		if (c==27 && multicode_state==0){ // handle multi-code keys
			multicode_state=1;
			continue;
		}
		if (c==91 && multicode_state==1){
			multicode_state=2;
			continue;
		}
		if (c==65 && multicode_state==2){ // up arrow
			int i;
			while (index>0) {
				prompt_backspace();
				index--;
			}
			for (i=0;oldbuf[i];++i) {
				putchar(oldbuf[i]);
				buf[i]=oldbuf[i];
			}
			index=i;
			continue;
		}
		else
			multicode_state=0;

		putchar(c); // echo the character
		buf[index++]=c;
		if (index>=sizeof(buf)-1) break;
=======
		if (c == 27 && multicode_state == 0){ // handle multi-code keys
			multicode_state = 1;
			continue;
		}
		if (c == 91 && multicode_state == 1){
			multicode_state = 2;
			continue;
		}
		if (c == 65 && multicode_state == 2){ // up arrow
			int i;
			while (index > 0){
				prompt_backspace();
				index--;
			}
			for (i = 0; oldbuf[i]; ++i){
				putchar(oldbuf[i]);
				buf[i] = oldbuf[i];
			}
			index = i;
			continue;
		}
		else
			multicode_state = 0;

		putchar(c); // echo the character
		buf[index++]=c;
		if (index >= sizeof(buf)-1) break;
>>>>>>> 564ca239560e91074eabdd1e84b9693b0ae33634
		if (c=='\n') // enter key
			break;
		if (c==4) // Ctrl+D
			return EXIT;
  	}
<<<<<<< HEAD
  	if (index>0 && buf[index-1]=='\n') // trim newline from the end
  		index--;
  	buf[index++]=0; // null terminate string

  	strcpy(oldbuf, buf);

  	parse_command(buf, command);

  	// print_command(command); // DEBUG: uncomment for debugging

    // restore the old settings
    tcsetattr(STDIN_FILENO, TCSANOW, &backup_termios);
  	return SUCCESS;
}

int process_command(struct command_t *command);

int main() {
	while (1) {
		struct command_t *command=malloc(sizeof(struct command_t));
		memset(command, 0, sizeof(struct command_t)); // set all bytes to 0

		int code;
		code = prompt(command);
		if (code==EXIT) break;

		code = process_command(command);
		if (code==EXIT) break;

		free_command(command);
	}

	printf("\n");
	return 0;
}

int process_command(struct command_t *command) {
	int r;
	if (strcmp(command->name, "")==0) return SUCCESS;

	if (strcmp(command->name, "exit")==0)
		return EXIT;

	if (strcmp(command->name, "cd")==0) {
		if (command->arg_count > 0) {
			r=chdir(command->args[0]);
			if (r==-1)
				printf("-%s: %s: %s\n", sysname, command->name, strerror(errno));
			return SUCCESS;
		}
	}

	pid_t pid=fork();
	if (pid==0){
		/// This shows how to do exec with environ (but is not available on MacOs)
	    // extern char** environ; // environment variables
		// execvpe(command->name, command->args, environ); // exec+args+path+environ

		/// This shows how to do exec with auto-path resolve
		// add a NULL argument to the end of args, and the name to the beginning
		// as required by exec

		// increase args size by 2
		command->args=(char **)realloc( command->args, sizeof(char *)*(command->arg_count+=2));

		// shift everything forward by 1
		for (int i=command->arg_count-2;i>0;--i)
			command->args[i]=command->args[i-1];

		// set args[0] as a copy of name
		command->args[0]=strdup(command->name);
		// set args[arg_count-1] (last) to NULL
		command->args[command->arg_count-1]=NULL;

		execvp(command->name, command->args); // exec+args+path
		exit(0);
		/// TODO: do your own exec with path resolving using execv()
	}
	else {
		if (!command->background)
			wait(0); // wait for child process to finish
		return SUCCESS;
	}

	// TODO: your implementation here

	printf("-%s: %s: command not found\n", sysname, command->name);
	return UNKNOWN;
}
