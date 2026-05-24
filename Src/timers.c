#include "main.h"
#include "timers.h"

extern TIM_HandleTypeDef htim2;

uint32_t microsec(void)
{
    return __HAL_TIM_GET_COUNTER(&htim2);
}

uint32_t millisec(void)
{
    return microsec() / 1000;
}


uint8_t SysTimer_250ms(void)
{
    static uint32_t last = 0;
    uint32_t now = HAL_GetTick();

    if ((now - last) >= 250)
    {
        last += 250;
        return TRUE;
    }

    return FALSE;
}

uint8_t SysTimer_50ms(void)
{
    static uint32_t last = 0;
    uint32_t now = HAL_GetTick();

    if ((now - last) >= 50)
    {
        last += 50;
        return TRUE;
    }

    return FALSE;
}

uint8_t SysTimer_10ms(void)
{
    static uint32_t last = 0;
    uint32_t now = HAL_GetTick();

    if ((now - last) >= 10)
    {
        last += 10;
        return TRUE;
    }

    return FALSE;
}
