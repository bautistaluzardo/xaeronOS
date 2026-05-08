#include "shell.h"
#include "commands.h"
#include "../drivers/vga.h"
#include "../lib/string.h"
#include <stddef.h>

char command_buffer[256];
int  command_index = 0;
int  argc = 0;
char argv[MAX_ARGS][ARG_LEN];


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
    { "reboot",     cmd_reboot,     "reinicia el sistema"         },
    { "cpuinfo",    cmd_cpuinfo,    "muestra informacion del CPU" },
    { "version",    cmd_version,    "version del kernel"          },
    { "echo",       cmd_echo,       "imprime valores en pantalla" },
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

    parse_args();  // <-- nuevo

    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(argv[0], commands[i].name)) {  // <-- argv[0]
            commands[i].func();
            command_index = 0;
            render_prompt();
            return;
        }
    }

    print("\ncomando no encontrado: ");
    print(argv[0]);  // <-- argv[0]
    print("\n");
    command_index = 0;
    render_prompt();
}

void parse_args(void) {
    argc = 0;
    int i = 0;
    int len = strlen(command_buffer);

    while (i < len && argc < MAX_ARGS) {
        // saltear espacios
        while (i < len && command_buffer[i] == ' ') i++;
        if (i >= len) break;

        // copiar palabra
        int j = 0;
        while (i < len && command_buffer[i] != ' ' && j < ARG_LEN - 1) {
            argv[argc][j++] = command_buffer[i++];
        }
        argv[argc][j] = '\0';
        argc++;
    }
}