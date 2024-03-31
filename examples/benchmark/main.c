/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021-2022 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "benchmark"
#define bfdev_log_fmt(fmt) MODULE_NAME ": " fmt

#include <stdio.h>
#include <errno.h>
#include <bfdev.h>

#include "main.h"
#include "stm32h7xx_hal.h"

UART_HandleTypeDef huart1;
IWDG_HandleTypeDef hiwgd;

extern int crc_benchmark(void);
extern int rbtree_benchmark(void);
extern int mpi_benchmark(void);

int __io_putchar(int ch)
{
    if (ch == '\n')
        HAL_UART_Transmit(&huart1, (void *)"\r", 1, -1);
    HAL_UART_Transmit(&huart1, (void *)&ch, 1, -1);
    return ch;
}

void _exit(int status)
{
    printf("Benchmark panic: (%d)\n", errno);
    for (;;)
        iwdg_touch();
}

int main(void)
{
    RCC_OscInitTypeDef OscInitType = {};
    RCC_ClkInitTypeDef ClkInitType = {};
    MPU_Region_InitTypeDef MpuInitType = {};
    GPIO_InitTypeDef GPIOInitType = {};
    const char *errinfo;
    int retval;

    HAL_Init();

    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY));

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
    while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY));

    OscInitType.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    OscInitType.HSEState = RCC_HSE_ON;
    OscInitType.LSIState = RCC_LSI_ON;
    OscInitType.PLL.PLLState = RCC_PLL_ON;
    OscInitType.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    OscInitType.PLL.PLLM = 5;
    OscInitType.PLL.PLLN = 192;
    OscInitType.PLL.PLLP = 2;
    OscInitType.PLL.PLLQ = 2;
    OscInitType.PLL.PLLR = 2;
    OscInitType.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    OscInitType.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    OscInitType.PLL.PLLFRACN = 0;
    HAL_RCC_OscConfig(&OscInitType);

    ClkInitType.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK |
                            RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2 |
                            RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
    ClkInitType.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    ClkInitType.SYSCLKDivider = RCC_SYSCLK_DIV1;
    ClkInitType.AHBCLKDivider = RCC_HCLK_DIV2;
    ClkInitType.APB3CLKDivider = RCC_APB3_DIV2;
    ClkInitType.APB1CLKDivider = RCC_APB1_DIV2;
    ClkInitType.APB2CLKDivider = RCC_APB2_DIV2;
    ClkInitType.APB4CLKDivider = RCC_APB4_DIV2;
    HAL_RCC_ClockConfig(&ClkInitType, FLASH_LATENCY_4);

    HAL_MPU_Disable();
    MpuInitType.Enable = MPU_REGION_ENABLE;
    MpuInitType.Number = MPU_REGION_NUMBER0;
    MpuInitType.BaseAddress = 0x0;
    MpuInitType.Size = MPU_REGION_SIZE_4GB;
    MpuInitType.SubRegionDisable = 0x87;
    MpuInitType.TypeExtField = MPU_TEX_LEVEL0;
    MpuInitType.AccessPermission = MPU_REGION_NO_ACCESS;
    MpuInitType.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MpuInitType.IsShareable = MPU_ACCESS_SHAREABLE;
    MpuInitType.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MpuInitType.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

    HAL_MPU_ConfigRegion(&MpuInitType);
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

    SCB_EnableICache();
    SCB_EnableDCache();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();

    GPIOInitType.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIOInitType.Mode = GPIO_MODE_AF_PP;
    GPIOInitType.Pull = GPIO_PULLUP;
    GPIOInitType.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIOInitType.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIOInitType);

    GPIOInitType.Pin = GPIO_PIN_4;
    GPIOInitType.Mode = GPIO_MODE_OUTPUT_PP;
    GPIOInitType.Pull = GPIO_PULLUP;
    GPIOInitType.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIOInitType);

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&huart1);

    hiwgd.Instance = IWDG1;
    hiwgd.Init.Prescaler = IWDG_PRESCALER_32;
    hiwgd.Init.Window = IWDG_WINDOW_DISABLE;
    hiwgd.Init.Reload = 1500;
    HAL_IWDG_Init(&hiwgd);

    bfdev_log_info("Benchmark for STM32H7xx.\n");
    bfdev_log_info("Bfdev version: %s\n", __bfdev_stringify(BFDEV_VERSION));
    bfdev_log_info("SYSCLK freq: %u\n", HAL_RCC_GetSysClockFreq());
    bfdev_log_info("AHB freq: %u\n", HAL_RCC_GetHCLKFreq());
    bfdev_log_info("APB1 freq: %u\n", HAL_RCC_GetPCLK1Freq());
    bfdev_log_info("APB2 freq: %u\n", HAL_RCC_GetPCLK2Freq());
    bfdev_log_info("This may take a few minutes...\n");
    puts(""); /* '\n' */

    for (;;) {
        iwdg_touch();
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);

        retval = crc_benchmark();
        if (retval) {
            bfdev_errname(retval, &errinfo);
            printf("error %d: %s\n", retval, errinfo);
            abort();
        }
        puts(""); /* '\n' */

        iwdg_touch();
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);

        retval = rbtree_benchmark();
        if (retval) {
            bfdev_errname(retval, &errinfo);
            printf("error %d: %s\n", retval, errinfo);
            abort();
        }
        puts(""); /* '\n' */

        iwdg_touch();
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);

        retval = mpi_benchmark();
        if (retval) {
            bfdev_errname(retval, &errinfo);
            printf("error %d: %s\n", retval, errinfo);
            abort();
        }
        puts(""); /* '\n' */
    }

    return 0;
}
