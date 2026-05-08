
#include "commands.h"
#include "../drivers/vga.h"
#include "../drivers/timer.h"
#include "../lib/string.h"
#include "../mm/pmm.h"
#include "../mm/vmm.h"
#include "../mm/kmalloc.h"
#include "../lib/io.h"
#include "shell.h"

void cmd_echo() {
    print("\n");
    for (int i = 1; i < argc; i++) {  // desde 1, saltando el nombre del comando
        print(argv[i]);
        if (i < argc - 1) print(" ");
    }
    print("\n");
}

void cmd_version() {
    print("\nxaeronOS v0.1.0\n");
    print("Compilado: ");
    print(__DATE__);
    print(" ");
    print(__TIME__);
    print("\n");
    print("Arch: x86_64\n");
}

void cmd_cpuinfo() {
    uint32_t eax, ebx, ecx, edx;

    // cpuid con eax=0 devuelve el vendor string en ebx, edx, ecx
    __asm__ volatile (
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(0)
    );

    // el vendor string son 12 bytes: ebx + edx + ecx
    char vendor[13];
    ((uint32_t*)vendor)[0] = ebx;
    ((uint32_t*)vendor)[1] = edx;
    ((uint32_t*)vendor)[2] = ecx;
    vendor[12] = '\0';

    // cpuid con eax=1 devuelve family/model/stepping en eax
    __asm__ volatile (
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(1)
    );

    uint32_t stepping = eax & 0xF;
    uint32_t model    = (eax >> 4)  & 0xF;
    uint32_t family   = (eax >> 8)  & 0xF;

    print("\nCPU Vendor : "); print(vendor);
    print("\nFamily     : "); print_number(family);
    print("\nModel      : "); print_number(model);
    print("\nStepping   : "); print_number(stepping);
    print("\n");
}

void cmd_reboot() {
    print("\nReiniciando...\n");
    // vaciar el buffer del keyboard controller
    uint8_t val;
    do {
        val = inb(0x64);
        if (val & 1) inb(0x60);  // limpiar output buffer
    } while (val & 2);           // esperar que el input buffer esté libre

    // mandar el pulso de reset
    outb(0x64, 0xFE);

    // si no funcionó, triple fault como fallback
    __asm__ volatile ("lidt 0; int $0");
}

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
