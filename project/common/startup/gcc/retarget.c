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

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h> /* for timeofday */
#include "compiler.h"
#include "driver/chip/system_chip.h"
#include "driver/chip/hal_global.h"
#include "common/board/board.h"
#include "sys/sram_heap.h"

extern int stdout_init(void);
extern void main_task_start(void);

void hardware_init_hook(void)
{
#ifdef __CONFIG_ROM
	extern void rom_init();
	rom_init();
#endif
	HAL_BoardIoctlCbRegister(board_ioctl);
	SystemInit();
}

int __wrap_main(void)
{
	static const GPIO_GlobalInitParam gpio_param = {
		.portIRQUsed  = PRJCONF_GPIO_PORT_IRQ_USED,
		.portPmBackup = PRJCONF_GPIO_PORT_PM_BACKUP
	};

	SystemCoreClockUpdate();
	timeofday_restore();
	HAL_GlobalInit();
	HAL_GPIO_GlobalInit(&gpio_param);
#if PRJCONF_SWD_EN
	HAL_SWD_Init();
#endif
#if PRJCONF_UART_EN
	stdout_init();
#endif
	main_task_start();
	return -1;
}

#if (__CONFIG_MALLOC_MODE == 0x00)

/* Linker defined symbol used by _sbrk to indicate where heap should start. */
extern uint8_t __heap_start__[];
extern uint8_t __heap_end__[];

static uint8_t *heap = __heap_start__;

/* Dynamic memory allocation related syscall. */
void *_sbrk(int incr)
{
	uint8_t *prev_heap = heap;
	uint8_t *new_heap = heap + incr;

	/* avoid corrupting heap data by the increase of main stack (MSP) */
	if (new_heap >= __heap_end__) {
#ifndef __CONFIG_MIX_HEAP_MANAGE
		printf("heap exhausted, incr %d, %p >= %p\n",
		       incr, new_heap, __heap_end__);
#endif
		return (void *)-1;
	}

	heap = new_heap;
	return (void *)prev_heap;
}

void heap_get_space(uint8_t **start, uint8_t **end, uint8_t **current)
{
	*start = __heap_start__;
	*end = __heap_end__;
	*current = heap;
}

#else

extern uint8_t __heap_start__[];
extern uint8_t __heap_end__[];

void heap_get_space(uint8_t **start, uint8_t **end, uint8_t **current)
{
	*start = __heap_start__;
	*end = __heap_end__;
	*current = __heap_end__ - sram_minimum_ever_free_heap_size();
}

#endif

void __wrap_exit(int status)
{
	printf("ERR: exit %d\n", status);
	while (1);
}

static char *__env[] = {
	PRJCONF_ENV_TZ,
	NULL
};

char **environ = __env;
