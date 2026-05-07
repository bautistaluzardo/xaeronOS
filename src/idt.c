#include "idt.h"

extern void print(const char* str);
extern void isr0();
extern void idt_load(struct idtr*);
extern void irq1();
extern void irq0();
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1"
                      :
                      : "a"(value), "Nd"(port));
}

static struct idt_entry idt[256];
static struct idtr idtr_desc;

void pic_remap(void);

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
    idt_set_gate(32, irq0);
    idt_set_gate(33, irq1);

    idtr_desc.limit = sizeof(idt) - 1;
    idtr_desc.base  = (uint64_t)&idt;
    pic_remap();
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

void pic_remap() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0xFC);
    outb(0xA1, 0xFF);
}