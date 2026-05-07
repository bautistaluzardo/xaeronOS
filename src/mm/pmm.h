#pragma once
#include <stdint.h>
#include <stddef.h>

void pmm_init(uint64_t mb2_info_addr);
void *pmm_alloc_frame(void);
void pmm_free_frame(void *addr);
uint64_t pmm_free_frames(void);
uint64_t pmm_total_frames(void);