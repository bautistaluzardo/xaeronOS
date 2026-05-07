#include "idt.h"

extern void print(const char* str);
extern void isr0();
extern void idt_load(struct idtr*);

static struct idt_entry idt[256];
static struct idtr idtr_desc;

static void idt_set_gate(int n, void* handler) {
    uint64_t addr = (uint64_t)handler;

    idt[n].offset_low  = addr & 0xFFFF;
    idt[n].selector    = 0x08;
    idt[n].ist         = 0;
    idt[n].type_attr   = 0x8E;
    idt[n].offset_mid  = (addr >> 16) & 0xFFFF;
    idt[n].offset_high = (addr >> 32) & 0xFFFFFFFF;
    idt[n].zero        = 0;
}

void idt_init() {
    idt_set_gate(0, isr0);

    idtr_desc.limit = sizeof(idt) - 1;
    idtr_desc.base  = (uint64_t)&idt;

    idt_load(&idtr_desc);
}

const char* exception_messages[] = {
    "NO DIVIDAS POR CERO BURRO",
    "DEBUG",
    "INTERRUPCION? NI AHI, NMI",
    "SE ROMPIO EL PUNTO, BREAKPOINT",
    "FLUJO",
    "BOUND RANGE",
    "MAL OPCODE",
    "NO EXISTE EL DISPOSITIVO",
    "DOOUUUBLE FAULT",
};

void exception_handler(int isr) {
    print("SE ROMPIO ALGO: ");

    if (isr < 9)
        print(exception_messages[isr]);
    else
        print("ME MATASTE HERMANO, NO SOY DE POR ACA, UNKNOWN EXCEPTION");

    print("\n");
}