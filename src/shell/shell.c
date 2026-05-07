#include "shell.h"
#include "commands.h"
#include "../drivers/vga.h"
#include "../lib/string.h"
 
char command_buffer[256];
int command_index = 0;
 
void prompt() {
    print("\nxaeronOS> ");
}
 
void execute_command() {
 
    command_buffer[command_index] = '\0';
 
    if (strcmp(command_buffer, "help")) {
 
        cmd_help();
 
    } else if (strcmp(command_buffer, "clear")) {
 
        cmd_clear();
 
    } else if (strcmp(command_buffer, "uptime")) {
 
        cmd_uptime();
 
    } else if (strcmp(command_buffer, "about")) {
 
        cmd_about();
 
    } else if (strcmp(command_buffer, "panic")) {
 
        cmd_panic();
 
    } else if (strcmp(command_buffer, "sleep")) {
 
        cmd_sleep();
        return;
 
    } else if (command_index == 0) {
 
        /* gato no hace nada */
 
    } else {
 
        print("\nNo lo tengo a ese comando pa, fijate despues\n");
    }
 
    command_index = 0;
    render_prompt();
}
