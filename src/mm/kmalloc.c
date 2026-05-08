#include "kmalloc.h"
#include "vmm.h"
#include "pmm.h"
#include "../drivers/vga.h"

#define HEAP_START  0x800000ULL   // 8MB, bien lejos del kernel
#define HEAP_MAX    0x1000000ULL  // crece hasta 16MB máximo

static uint64_t heap_top = HEAP_START;

// alinear hacia arriba a 'align' bytes
static uint64_t align_up(uint64_t val, uint64_t align) {
    return (val + align - 1) & ~(align - 1);
}

void kmalloc_init(void) {
    // mapear la primera página del heap
    void *frame = pmm_alloc_frame();
    vmm_map_page(HEAP_START, (uint64_t)frame, PAGE_WRITE);
    print("kmalloc: inicializado\n");
}

void *kmalloc(size_t size) {
    if (size == 0) return 0;

    // alinear a 16 bytes (requerimiento ABI)
    size = align_up(size, 16);

    // verificar que no nos pasamos del heap
    if (heap_top + size > HEAP_MAX) {
        print("kmalloc: heap lleno\n");
        return 0;
    }

    uint64_t addr = heap_top;

    // si la alocacion cruza el limite de una pagina, mapear la siguiente
    uint64_t page_end = align_up(heap_top, 4096);
    if (heap_top + size > page_end) {
        void *frame = pmm_alloc_frame();
        if (!frame) {
            print("kmalloc: sin frames\n");
            return 0;
        }
        vmm_map_page(page_end, (uint64_t)frame, PAGE_WRITE);
    }

    heap_top += size;
    return (void *)addr;
}

void kfree(void *ptr) {
    // bump allocator: no libera memoria individualmente
    (void)ptr;
}