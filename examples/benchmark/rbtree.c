/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021-2022 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "rbtree-benchmark"
#define bfdev_log_fmt(fmt) MODULE_NAME ": " fmt

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <bfdev.h>

#include "main.h"
#include "stm32h7xx_hal.h"

#define TEST_LEN 1000
#define TEST_LOOP 1000
static BFDEV_RB_ROOT(bench_root);

struct bench_node {
    struct bfdev_rb_node node;
    unsigned int data;
};

#define rb_to_bench(ptr) \
    bfdev_rb_entry_safe(ptr, struct bench_node, node)

static long
demo_cmp(const struct bfdev_rb_node *a,
         const struct bfdev_rb_node *b, void *pdata)
{
    struct bench_node *demo_a = rb_to_bench(a);
    struct bench_node *demo_b = rb_to_bench(b);
    return demo_a->data - demo_b->data;
}

int rbtree_benchmark(void)
{
    struct bench_node *node;
    unsigned int count, loop;
    uint32_t start, time;
    void *block;

    node = block = malloc(sizeof(*node) * TEST_LEN);
    if (!block) {
        bfdev_log_err("Insufficient Memory!\n");
        return -ENOMEM;
    }

    bfdev_log_notice("Generate Node: %u\n", TEST_LEN);
    for (count = 0; count < TEST_LEN; ++count)
        node[count].data = count;

    bfdev_log_notice("Insert Nodes: %u\n", TEST_LOOP * TEST_LEN);
    start = HAL_GetTick();

    for (loop = 0; loop < TEST_LOOP; ++loop) {
        for (count = 0; count < TEST_LEN; ++count)
            bfdev_rb_insert(&bench_root, &node[count].node, demo_cmp, NULL);
        bench_root = BFDEV_RB_INIT();
        iwdg_touch();
    }

    time = HAL_GetTick() - start;
    bfdev_log_notice("Total time: %lu.%lus\n", time / 1000, time % 1000);
    free(block);

    return 0;
}
