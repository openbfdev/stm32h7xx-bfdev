#include "stm32h7xx_hal.h"

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

    GPIOInitType.Pin = GPIO_PIN_4;
    GPIOInitType.Mode = GPIO_MODE_OUTPUT_PP;
    GPIOInitType.Speed = GPIO_SPEED_FREQ_HIGH;

    __HAL_RCC_GPIOB_CLK_ENABLE();
    HAL_GPIO_Init(GPIOB, &GPIOInitType);

    for (;;) {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
        HAL_Delay(500);
    }

    return 0;
}
