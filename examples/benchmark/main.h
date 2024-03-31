/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021-2022 John Sanpe <sanpeqf@gmail.com>
 */

#ifndef _MAIN_H_
#define _MAIN_H_

#include "stm32h7xx_hal.h"

extern UART_HandleTypeDef huart1;
extern IWDG_HandleTypeDef hiwgd;

static inline void
iwdg_touch(void)
{
    HAL_IWDG_Refresh(&hiwgd);
}

#endif /*  */
