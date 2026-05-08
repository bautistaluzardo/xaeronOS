#ifndef SHELL_H
#define SHELL_H
#define MAX_ARGS 8
#define ARG_LEN  64

extern int   argc;
extern char  argv[MAX_ARGS][ARG_LEN];
extern char command_buffer[];
extern int  command_index;

void prompt(void);
void execute_command(void);
void render_prompt(void);
void cmd_help(void);
void parse_args(void);

#endif