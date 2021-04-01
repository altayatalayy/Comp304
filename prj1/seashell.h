
#ifndef __SEASHELL_H__
#define __SEASHELL_H__

const char * sysname = "seashell";
const char * config_file = "/home/umur/.seashell.rc";

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

void print_command(struct command_t * command);
int free_command(struct command_t *command);
int show_prompt();
int parse_command(char *buf, struct command_t *command);
void prompt_backspace();
int prompt(struct command_t *command);
int process_command(struct command_t *command);

void print_colored(char* str, char color){
	char *start, *end = "\033[0m";
	start = (color == 'r') ? "\033[0;31m": (color == 'g') ? "\033[0;32m" : "\033[0;34m";
	printf("%s%s%s", start, str, end);
}

#endif
