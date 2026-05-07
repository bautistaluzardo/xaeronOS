
#include "commands.h"
#include "../drivers/vga.h"
#include "../drivers/timer.h"
#include "../lib/string.h"
#include "../mm/pmm.h"
#include "../mm/vmm.h"

void cmd_vmtest() {
    // alocamos un frame físico
    void *frame = pmm_alloc_frame();
    if (!frame) {
        print("\nvmtest: sin memoria\n");
        return;
    }

    // lo mapeamos a una dirección virtual arbitraria
    uint64_t virt = 0x400000;  // 4MB, fuera del kernel
    vmm_map_page(virt, (uint64_t)frame, PAGE_WRITE);

    // escribimos en la dirección virtual
    uint64_t *ptr = (uint64_t *)virt;
    *ptr = 0xDEADBEEF;

    // leemos y verificamos
    if (*ptr == 0xDEADBEEF) {
        print("\nvmtest: OK, mapeo funciona\n");
    } else {
        print("\nvmtest: FALLO, valor incorrecto\n");
    }

    // desmapeamos
    vmm_unmap_page(virt);
    pmm_free_frame(frame);
}

void cmd_meminfo() {
    print("\nMemoria fisica:\n");
    print("  Total : ");
    print_number(pmm_total_frames() * 4);
    print(" KB\n");
    print("  Libre : ");
    print_number(pmm_free_frames() * 4);
    print(" KB\n");
    print("  Usada : ");
    print_number((pmm_total_frames() - pmm_free_frames()) * 4);
    print(" KB\n");
}

void cmd_help() {
    print("\nComandos disponibles:\n");
    print("help   - muestra esta ayuda\n");
    print("clear  - limpia la pantalla\n");
    print("about  - info del kernel\n");
    print("uptime - tiempo desde el boot\n");
    print("sleep - manda al sistema a dormir 3 segundos\n");
    print("panic  - debug\n");
    print("meminfo - muestra la memoria fisica\n");
    print("vmtest - test de memoria virtual\n");
}
 
void cmd_clear() {
    clear_screen();
}
 
void cmd_uptime() {
    print("\nVamos corriendo: ");
    print_number(timer_ticks / 100);
    print(" segundos\n");
}
 
void cmd_about() {
    print("\nxaeronOS");
    print("\nKernel 64-bit experimental");
    print("\nDesarollado por Bautista E. Luzardo");
    print("\nCon IRQs y teclado PS/2, azar y mujerzuelas\n");
}
 
void cmd_sleep() {
    print("\nDurmiendo 10 segundos...\n");
    sleep(1000);
}
 
void cmd_panic() {
    __asm__ volatile ("int $0");
}
