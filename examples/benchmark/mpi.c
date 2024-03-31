/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 John Sanpe <sanpeqf@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <bfdev.h>

#include "main.h"
#include "stm32h7xx_hal.h"

#define TEST_LEN 10000
#define TEST_SIZE (TEST_LEN / 4 + 1)
#define TEST_LOOP (TEST_LEN / 1.39793 + 1)

int mpi_benchmark(void)
{
    uint32_t start, time;
    bfdev_mpi_t *vw, *vs, *vv, *vq;
    unsigned int k;
    int retval;

    if (!((vw = bfdev_mpi_create(NULL)) &&
          (vs = bfdev_mpi_create(NULL)) &&
          (vv = bfdev_mpi_create(NULL)) &&
          (vq = bfdev_mpi_create(NULL))))
        return -ENOMEM;

    if ((retval = bfdev_mpi_seti(vw, 16 * 5)) ||
        (retval = bfdev_mpi_seti(vv, 4 * 239)) ||
        (retval = bfdev_mpi_seti(vq, 10000)))
        return retval;

    for (k = 0; k < TEST_SIZE; ++k) {
        if ((retval = bfdev_mpi_mul(vw, vw, vq)) ||
            (retval = bfdev_mpi_mul(vv, vv, vq)))
            return retval;
    }

    bfdev_log_info("Convergence Machin %d:\n", TEST_LEN);
    start = HAL_GetTick();
    for (k = 1; k <= TEST_LOOP; ++k) {
        if ((retval = bfdev_mpi_divi(vw, vw, vw, 5 * 5)) ||
            (retval = bfdev_mpi_divi(vv, vv, vv, 239 * 239)) ||
            (retval = bfdev_mpi_sub(vq, vw, vv)) ||
            (retval = bfdev_mpi_divi(vq, vq, vq, 2 * k - 1)))
            return retval;

        if (k & 1)
            retval = bfdev_mpi_add(vs, vs, vq);
        else
            retval = bfdev_mpi_sub(vs, vs, vq);

        if (retval)
            return retval;

        iwdg_touch();
    }

    bfdev_mpi_destory(vw);
    bfdev_mpi_destory(vs);
    bfdev_mpi_destory(vv);
    bfdev_mpi_destory(vq);
    iwdg_touch();

    time = HAL_GetTick() - start;
    bfdev_log_notice("Total time: %lu.%lus\n", time / 1000, time % 1000);

    return 0;
}
