#include "adc.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

static ADC_Readings_t g_adc_readings;

static float ADC_CountsToVoltage(uint16_t counts)
{
    return ((float)counts * ADC_VREF) / ADC_MAX_COUNT;
}

void ADC_App_Init(void)
{
    g_adc_readings.in_raw = 0;
    g_adc_readings.can_raw = 0;
    g_adc_readings.in_volt = 0.0f;
    g_adc_readings.can_volt = 0.0f;
}

HAL_StatusTypeDef ADC_ReadBoth(uint16_t *adc1_value, uint16_t *adc2_value)
{
    HAL_StatusTypeDef status;

    if ((adc1_value == NULL) || (adc2_value == NULL))
    {
        return HAL_ERROR;
    }

    /* Start both ADCs first so samples happen close together */
    status = HAL_ADC_Start(&hadc1);
    if (status != HAL_OK)
    {
        return status;
    }

    status = HAL_ADC_Start(&hadc2);
    if (status != HAL_OK)
    {
        HAL_ADC_Stop(&hadc1);
        return status;
    }

    /* Wait for ADC1 conversion */
    status = HAL_ADC_PollForConversion(&hadc1, 50);
    if (status != HAL_OK)
    {
        HAL_ADC_Stop(&hadc1);
        HAL_ADC_Stop(&hadc2);
        return status;
    }

    /* Wait for ADC2 conversion */
    status = HAL_ADC_PollForConversion(&hadc2, 50);
    if (status != HAL_OK)
    {
        HAL_ADC_Stop(&hadc1);
        HAL_ADC_Stop(&hadc2);
        return status;
    }

    *adc1_value = (uint16_t)HAL_ADC_GetValue(&hadc1);
    *adc2_value = (uint16_t)HAL_ADC_GetValue(&hadc2);

    HAL_ADC_Stop(&hadc1);
    HAL_ADC_Stop(&hadc2);

    return HAL_OK;
}

HAL_StatusTypeDef ADC_UpdateReadings(void)
{
    HAL_StatusTypeDef status;
    uint16_t raw1;
    uint16_t raw2;

    status = ADC_ReadBoth(&raw1, &raw2);
    if (status != HAL_OK)
    {
        return status;
    }

    g_adc_readings.in_raw = raw1;
    g_adc_readings.can_raw = raw2;
    g_adc_readings.in_volt = ADC_CountsToVoltage(raw1);
    g_adc_readings.can_volt = ADC_CountsToVoltage(raw2);

    return HAL_OK;
}

uint16_t ADC_GetRawIn(void)
{
    return g_adc_readings.in_raw;
}

uint16_t ADC_GetRawCan(void)
{
    return g_adc_readings.can_raw;
}

float ADC_GetVoltageIn(void)
{
    return g_adc_readings.in_volt;
}

float ADC_GetVoltageCan(void)
{
    return g_adc_readings.can_volt;
}

ADC_Readings_t ADC_GetReadings(void)
{
    return g_adc_readings;
}
