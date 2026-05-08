#ifndef SHELL_H
#define SHELL_H

extern char command_buffer[];
extern int  command_index;

void prompt(void);
void execute_command(void);
void render_prompt(void);
void cmd_help(void);

#endif