#include "vmm.h"
#include "pmm.h"
#include "../drivers/vga.h"

#define PAGE_SIZE 4096

// leer CR3 (PML4 actual)
static uint64_t read_cr3(void) {
    uint64_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

// invalidar una entrada del TLB
static void invlpg(uint64_t virt) {
    __asm__ volatile ("invlpg (%0)" :: "r"(virt) : "memory");
}

// extraer índices de la dirección virtual
#define PML4_IDX(v) (((v) >> 39) & 0x1FF)
#define PDPT_IDX(v) (((v) >> 30) & 0x1FF)
#define PDT_IDX(v)  (((v) >> 21) & 0x1FF)
#define PT_IDX(v)   (((v) >> 12) & 0x1FF)

// dirección física → puntero a tabla
#define PHYS_TO_VIRT(p) ((uint64_t*)(p))

static uint64_t* get_or_create_table(uint64_t *table, int idx) {
    if (!(table[idx] & PAGE_PRESENT)) {
        // no existe, crear nueva tabla
        void *frame = pmm_alloc_frame();
        if (!frame) {
            print("vmm: sin memoria para page table\n");
            for(;;) __asm__ volatile ("hlt");
        }
        // limpiar la tabla nueva
        uint64_t *new_table = PHYS_TO_VIRT((uint64_t)frame);
        for (int i = 0; i < 512; i++)
            new_table[i] = 0;

        table[idx] = (uint64_t)frame | PAGE_PRESENT | PAGE_WRITE;
    }
    // devolver puntero a la tabla apuntada por esta entrada
    return PHYS_TO_VIRT(table[idx] & ~0xFFFULL);
}

void vmm_map_page(uint64_t virt, uint64_t phys, uint64_t flags) {
    uint64_t *pml4 = PHYS_TO_VIRT(read_cr3() & ~0xFFFULL);

    uint64_t *pdpt = get_or_create_table(pml4, PML4_IDX(virt));
    uint64_t *pdt  = get_or_create_table(pdpt, PDPT_IDX(virt));
    uint64_t *pt   = get_or_create_table(pdt,  PDT_IDX(virt));

    pt[PT_IDX(virt)] = (phys & ~0xFFFULL) | flags | PAGE_PRESENT;
    invlpg(virt);
}

void vmm_unmap_page(uint64_t virt) {
    uint64_t *pml4 = PHYS_TO_VIRT(read_cr3() & ~0xFFFULL);

    uint64_t *pdpt = PHYS_TO_VIRT(pml4[PML4_IDX(virt)] & ~0xFFFULL);
    uint64_t *pdt  = PHYS_TO_VIRT(pdpt[PDPT_IDX(virt)] & ~0xFFFULL);
    uint64_t *pt   = PHYS_TO_VIRT(pdt[PDT_IDX(virt)]   & ~0xFFFULL);

    pt[PT_IDX(virt)] = 0;
    invlpg(virt);
}

uint64_t vmm_get_phys(uint64_t virt) {
    uint64_t *pml4 = PHYS_TO_VIRT(read_cr3() & ~0xFFFULL);

    if (!(pml4[PML4_IDX(virt)] & PAGE_PRESENT)) return 0;
    uint64_t *pdpt = PHYS_TO_VIRT(pml4[PML4_IDX(virt)] & ~0xFFFULL);

    if (!(pdpt[PDPT_IDX(virt)] & PAGE_PRESENT)) return 0;
    uint64_t *pdt = PHYS_TO_VIRT(pdpt[PDPT_IDX(virt)] & ~0xFFFULL);

    if (!(pdt[PDT_IDX(virt)] & PAGE_PRESENT)) return 0;
    uint64_t *pt = PHYS_TO_VIRT(pdt[PDT_IDX(virt)] & ~0xFFFULL);

    if (!(pt[PT_IDX(virt)] & PAGE_PRESENT)) return 0;
    return (pt[PT_IDX(virt)] & ~0xFFFULL) | (virt & 0xFFF);
}

void vmm_init(void) {
    print("vmm: inicializado\n");
}