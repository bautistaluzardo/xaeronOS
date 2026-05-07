#include "pmm.h"
#include "../drivers/vga.h"  // para debug prints

// Multiboot2 tags
#define MB2_TAG_END     0
#define MB2_TAG_MMAP    6

#define MMAP_AVAILABLE  1

#define FRAME_SIZE      4096

// símbolos del linker
extern uint64_t kernel_start;
extern uint64_t kernel_end;

// bitmap estático: soporta hasta 4GB (131072 frames de 4KB = 16KB de bitmap)
#define MAX_FRAMES (1024 * 1024)
static uint8_t bitmap[MAX_FRAMES / 8];
static uint64_t total_frames = 0;
static uint64_t free_frames  = 0;

// ---- helpers bitmap ----

static void bitmap_set(uint64_t frame) {
    bitmap[frame / 8] |= (1 << (frame % 8));
}

static void bitmap_clear(uint64_t frame) {
    bitmap[frame / 8] &= ~(1 << (frame % 8));
}

static int bitmap_test(uint64_t frame) {
    return bitmap[frame / 8] & (1 << (frame % 8));
}

// ---- Multiboot2 structs ----

typedef struct {
    uint32_t total_size;
    uint32_t reserved;
} __attribute__((packed)) mb2_info_t;

typedef struct {
    uint32_t type;
    uint32_t size;
} __attribute__((packed)) mb2_tag_t;

typedef struct {
    uint32_t type;
    uint32_t size;
    uint64_t entry_size;   // en realidad son dos campos de 32 bits
    // pero los leemos manualmente abajo
} __attribute__((packed)) mb2_tag_mmap_t;

typedef struct {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} __attribute__((packed)) mb2_mmap_entry_t;

// ---- init ----

void pmm_init(uint64_t mb2_addr) {
    // todo ocupado por defecto
    for (int i = 0; i < MAX_FRAMES / 8; i++)
        bitmap[i] = 0xFF;

    mb2_info_t *info = (mb2_info_t *)mb2_addr;
    mb2_tag_t  *tag  = (mb2_tag_t *)(mb2_addr + 8);

    while (tag->type != MB2_TAG_END) {
        if (tag->type == MB2_TAG_MMAP) {
            uint8_t *mmap_tag = (uint8_t *)tag;
            uint32_t entry_size = *(uint32_t *)(mmap_tag + 8);
            uint8_t *entry_ptr  = mmap_tag + 16;
            uint8_t *end_ptr    = mmap_tag + tag->size;

            while (entry_ptr < end_ptr) {
                mb2_mmap_entry_t *entry = (mb2_mmap_entry_t *)entry_ptr;

                if (entry->type == MMAP_AVAILABLE) {
                    uint64_t base   = entry->base_addr;
                    uint64_t length = entry->length;

                    // alinear base hacia arriba
                    if (base % FRAME_SIZE) {
                        uint64_t align = FRAME_SIZE - (base % FRAME_SIZE);
                        base   += align;
                        length -= align;
                    }

                    uint64_t frames = length / FRAME_SIZE;
                    for (uint64_t i = 0; i < frames; i++) {
                        uint64_t frame = (base / FRAME_SIZE) + i;
                        if (frame < MAX_FRAMES) {
                            bitmap_clear(frame);
                            free_frames++;
                            total_frames++;
                        }
                    }
                }

                entry_ptr += entry_size;
            }
        }

        // siguiente tag (alineado a 8 bytes)
        uint64_t next = (uint64_t)tag + tag->size;
        if (next % 8) next += 8 - (next % 8);
        tag = (mb2_tag_t *)next;
    }

    // marcar el kernel como ocupado
    uint64_t k_start = (uint64_t)&kernel_start;
    uint64_t k_end   = (uint64_t)&kernel_end;
    uint64_t k_first = k_start / FRAME_SIZE;
    uint64_t k_last  = (k_end + FRAME_SIZE - 1) / FRAME_SIZE;

    for (uint64_t i = k_first; i < k_last; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            free_frames--;
        }
    }
}

// ---- alloc / free ----

void *pmm_alloc_frame(void) {
    for (uint64_t i = 0; i < MAX_FRAMES; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            free_frames--;
            return (void *)(i * FRAME_SIZE);
        }
    }
    return 0;  // out of memory
}

void pmm_free_frame(void *addr) {
    uint64_t frame = (uint64_t)addr / FRAME_SIZE;
    if (!bitmap_test(frame)) return;  // ya estaba libre
    bitmap_clear(frame);
    free_frames++;
}

uint64_t pmm_free_frames(void)  { return free_frames; }
uint64_t pmm_total_frames(void) { return total_frames; }