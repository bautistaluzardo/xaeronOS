
#include "commands.h"
#include "../drivers/vga.h"
#include "../drivers/timer.h"
#include "../lib/string.h"
#include "../mm/pmm.h"
#include "../mm/vmm.h"
#include "../mm/kmalloc.h"

void cmd_xaeron() {
    print("X   X  AA  EEEE RRRR   OOO  N   N \n");
    print(" X X  A  A E    R   R O   O NN  N \n");
    print("  X   AAAA EEE  RRRR  O   O N N N \n");
    print(" X X  A  A E    R R   O   O N  NN \n");
    print("X   X A  A EEEE R  RR  OOO  N   N \n");

}
void cmd_malloctest() {
    uint64_t *a = kmalloc(sizeof(uint64_t));
    uint64_t *b = kmalloc(sizeof(uint64_t));
    uint64_t *c = kmalloc(128);

    if (!a || !b || !c) {
        print("\nmalloctest: fallo la alocacion\n");
        return;
    }

    *a = 0x1234;
    *b = 0x5678;

    print("\na: "); print_hex((uint64_t)a); print(" = "); print_hex(*a);
    print("\nb: "); print_hex((uint64_t)b); print(" = "); print_hex(*b);
    print("\nc: "); print_hex((uint64_t)c);
    print("\n");

    if (*a == 0x1234 && *b == 0x5678)
        print("malloctest: OK\n");
    else
        print("malloctest: FALLO\n");
}

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
