#include "shell.h"
#include "commands.h"
#include "../drivers/vga.h"
#include "../lib/string.h"
#include <stddef.h>

char command_buffer[256];
int  command_index = 0;

typedef struct {
    const char *name;
    void (*func)(void);
    const char *description;
} command_t;

static command_t commands[] = {
    { "help",       cmd_help,       "muestra esta ayuda"          },
    { "clear",      cmd_clear,      "limpia la pantalla"          },
    { "uptime",     cmd_uptime,     "tiempo desde el boot"        },
    { "about",      cmd_about,      "info del kernel"             },
    { "sleep",      cmd_sleep,      "duerme 10 segundos"          },
    { "panic",      cmd_panic,      "debug"                       },
    { "meminfo",    cmd_meminfo,    "estado de la memoria fisica" },
    { "vmtest",     cmd_vmtest,     "testea el VMM"               },
    { "malloctest", cmd_malloctest, "testea kmalloc"              },
    { "xaeron",     cmd_xaeron,     "arte ASCII"                  },
    { NULL, NULL, NULL }
};

void prompt(void) {
    print("\nxaeronOS> ");
}

void cmd_help(void) {
    print("\nComandos disponibles:\n");
    for (int i = 0; commands[i].name != NULL; i++) {
        print("  ");
        print(commands[i].name);
        print(" - ");
        print(commands[i].description);
        print("\n");
    }
}

void execute_command(void) {
    command_buffer[command_index] = '\0';

    if (command_index == 0) {
        render_prompt();
        return;
    }

    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(command_buffer, commands[i].name)) {
            commands[i].func();
            command_index = 0;
            render_prompt();
            return;
        }
    }

    print("\ncomando no encontrado: ");
    print(command_buffer);
    print("\n");
    command_index = 0;
    render_prompt();
}