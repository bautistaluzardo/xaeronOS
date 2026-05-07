#ifndef SHELL_H
#define SHELL_H

extern char command_buffer[256];
extern int command_index;

void execute_command();
void prompt();

#endif