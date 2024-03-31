/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021-2022 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "crc-benchmark"
#define bfdev_log_fmt(fmt) MODULE_NAME ": " fmt

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <bfdev.h>

#include "main.h"
#include "stm32h7xx_hal.h"

#define TEST_SIZE BFDEV_SZ_1KiB
#define TEST_LOOP 3

#define GENERIC_CRC_BANDWIDTH(func, name, size) \
for (count = 0; count < TEST_LOOP; ++count) {   \
    uint32_t start = HAL_GetTick();             \
    loop = 0;                                   \
    do {                                        \
        func(buff, size, 0);                    \
        loop++;                                 \
    } while (start + 1000 > HAL_GetTick());     \
    iwdg_touch();                               \
    bfdev_log_notice(                           \
        name " bandwidth %u: %uKiB/s\n",        \
        count, loop                             \
    );                                          \
}

int crc_benchmark(int argc, char const *argv[])
{
    bfdev_prandom_t pstate;
    unsigned int count, loop;
    uint8_t *buff;
    size_t index;

    buff = malloc(TEST_SIZE);
    if (!buff)
        return -ENOMEM;

    bfdev_prandom_seed(&pstate, HAL_GetTick());
    for (count = 0; count < TEST_SIZE; ++count)
        buff[count] = bfdev_prandom_value(&pstate);

    GENERIC_CRC_BANDWIDTH(bfdev_crc8, "crc8", TEST_SIZE)
    GENERIC_CRC_BANDWIDTH(bfdev_crc16, "crc16", TEST_SIZE)
    GENERIC_CRC_BANDWIDTH(bfdev_crc32, "crc32", TEST_SIZE)
    free(buff);

    return 0;
}
