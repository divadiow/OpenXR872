/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _SYS_DMA_HEAP_H
#define _SYS_DMA_HEAP_H

#include <stdlib.h>
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum DMAHEAP_FLAG {
	DMAHEAP_SYSTEM_RAM      = (1 << 0),
	DMAHEAP_SRAM            = (1 << 1),
	DMAHEAP_PSRAM           = (1 << 2),
};

void *dma_malloc(size_t size, uint32_t flag);
void dma_free(void *ptr, uint32_t flag);
void *dma_realloc(void *ptr, size_t size, uint32_t flag);
void *dma_calloc(size_t nmemb, size_t size, uint32_t flag);

#if ((defined __CONFIG_PSRAM) && (__CONFIG_DMAHEAP_PSRAM_SIZE != 0))
size_t dma_psram_total_heap_size(void);
size_t dma_psram_free_heap_size(void);
size_t dma_psram_minimum_ever_free_heap_size(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
