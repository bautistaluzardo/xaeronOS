#pragma once
#include <stdint.h>

#define PAGE_PRESENT   (1ULL << 0)
#define PAGE_WRITE     (1ULL << 1)
#define PAGE_USER      (1ULL << 2)

void     vmm_init(void);
void     vmm_map_page(uint64_t virt, uint64_t phys, uint64_t flags);
void     vmm_unmap_page(uint64_t virt);
uint64_t vmm_get_phys(uint64_t virt);