#include "adc.h"

uint16_t adc1_buf_in[ADC_BUF_LEN];
uint16_t adc2_buf_can[ADC_BUF_LEN];
volatile uint8_t adc1_ready = 0;
volatile uint8_t adc2_ready = 0;

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

static ADC_Readings_t g_adc_readings;

static float ADC_CountsToVoltage(uint16_t counts)
{
    return ((float)counts * ADC_VREF) / ADC_MAX_COUNT;
}

HAL_StatusTypeDef ADC_App_Init(void)
{
    g_adc_readings.in_raw = 0;
    g_adc_readings.can_raw = 0;
    g_adc_readings.in_volt = 0.0f;
    g_adc_readings.can_volt = 0.0f;
    HAL_StatusTypeDef status = HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc1_buf_in, ADC_BUF_LEN);
    if (status != HAL_OK) return status;
    status = HAL_ADC_Start_DMA(&hadc2, (uint32_t*)adc2_buf_can, ADC_BUF_LEN);
    return status;
}

HAL_StatusTypeDef ADC_ReadBoth(uint16_t *adc1_value, uint16_t *adc2_value)
{
    if ((adc1_value == NULL) || (adc2_value == NULL))
    {
        return HAL_ERROR;
    }

    if(adc1_ready == 1 && adc2_ready == 1) {
    	adc1_ready = 0;
    	adc2_ready = 0;

    	*adc1_value = adc_get_average(ADC1_IDX);
    	*adc2_value = adc_get_average(ADC2_IDX);

    	return HAL_OK;
    }

    return HAL_BUSY;
}

HAL_StatusTypeDef ADC_UpdateReadings(void)
{
    HAL_StatusTypeDef status;
    uint16_t raw1;
    uint16_t raw2;

    status = ADC_ReadBoth(&raw1, &raw2);
    if (status == HAL_OK)
    {
    	g_adc_readings.in_raw = raw1;
		g_adc_readings.can_raw = raw2;
		g_adc_readings.in_volt = ADC_CountsToVoltage(raw1);
		g_adc_readings.can_volt = ADC_CountsToVoltage(raw2);
    }

    return status;
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

uint16_t adc_get_average(uint8_t adc_idx) {
    uint32_t sum = 0;
    if(adc_idx == ADC1_IDX) {
    	for (int i = 0; i < ADC_BUF_LEN; i++) {
			sum += adc1_buf_in[i];
	    }
    	return sum / ADC_BUF_LEN;
    } else if (adc_idx == ADC2_IDX) {
    	for (int i = 0; i < ADC_BUF_LEN; i++) {
			sum += adc2_buf_can[i];
    	}
		return sum / ADC_BUF_LEN;
    }

    return 0xFFFF;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        adc1_ready = 1;
    }
    if (hadc->Instance == ADC2) {
        adc2_ready = 1;
    }
}
