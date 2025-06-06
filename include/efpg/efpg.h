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

#ifndef _EFPG_EFPG_H_
#define _EFPG_EFPG_H_

#include <stdint.h>
#include "driver/chip/hal_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief EFPG field definition
 */
typedef enum efpg_field {
	EFPG_FIELD_HOSC = 0,    /* data buffer size: 1  byte  */
	EFPG_FIELD_BOOT,        /* data buffer size: 32 bytes */
	EFPG_FIELD_MAC,         /* data buffer size: 6  bytes */
	EFPG_FIELD_DCXO,        /* data buffer size: 1  byte  */
	EFPG_FIELD_POUT,        /* data buffer size: 3  bytes */
	EFPG_FIELD_CHIPID,      /* data buffer size: 16 bytes */
	EFPG_FIELD_UA,          /* data buffer size: V1(1447~2047) : V2(765~1023 bit)*/
	EFPG_FIELD_ALL,         /* data buffer size: 128 bytes */
	EFPG_FIELD_NUM,
} efpg_field_t;

/**
 * @brief EFPG HOSC value definition
 */
#define    EFPG_HOSC_24M    (0x06)
#define    EFPG_HOSC_26M    (0x03)
#define    EFPG_HOSC_40M    (0x0C)
#define    EFPG_HOSC_52M    (0x09)

/** @brief Type define of EFPG callback function */
typedef void (*efpg_cb_t)(void);

int efpg_start(uint8_t *key, uint8_t key_len, UART_ID uart_id, efpg_cb_t start_cb, efpg_cb_t stop_cb);
int efpg_read(efpg_field_t field, uint8_t *data);
int efpg_read_ua(uint32_t start, uint32_t num, uint8_t *data);
int efpg_read_all(uint32_t start, uint32_t num, uint8_t *data);

/**
 * @brief EFPG return value definition
 */
#define EFPG_ACK_OK             (200)
#define EFPG_ACK_CS_ERR         (400)
#define EFPG_ACK_MD_ERR         (401)
#define EFPG_ACK_PARSE_ERR      (402)
#define EFPG_ACK_RW_ERR         (403)
#define EFPG_ACK_DI_ERR         (404)
#define EFPG_ACK_NODATA_ERR     (405)

uint16_t efpg_read_mac(uint8_t *r_data);
uint16_t efpg_read_dcxo(uint8_t *r_data);
uint16_t efpg_read_pout(uint8_t *r_data);
uint16_t efpg_read_chipid(uint8_t *r_data);
uint16_t efpg_read_boot(uint8_t *r_data);
uint16_t efpg_read_hosc(uint8_t *r_data);
uint16_t efpg_read_user_area(uint16_t start, uint16_t num, uint8_t *r_data);

#ifdef __cplusplus
}
#endif

#endif /* _EFPG_EFPG_H_ */
