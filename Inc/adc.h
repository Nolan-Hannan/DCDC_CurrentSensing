#ifndef ADC_H
#define ADC_H

#include "main.h"
#include <stdint.h>

#define ADC_MAX_COUNT   4095.0f
#define ADC_VREF        3.3f
#define ADC_VQUIESCENT 0.33f // Ideally 0.33, may need calibration
#define ADC_SENS 0.264f

#define ADC1_IDX 0 // High Voltage
#define ADC2_IDX 1 // CAN Bus

#define ADC_BUF_LEN 32

#define IN_CUR_THR 2000
#define CAN_CUR_THR 5000

// For this naming convention, PA0 must attach to the IN_PWR voltage, PA2 must attach to the CAN voltage
typedef struct
{
    uint16_t in_raw;
    uint16_t can_raw;
    float in_volt;
    float can_volt;
} ADC_Readings_t;

HAL_StatusTypeDef ADC_App_Init(void);
HAL_StatusTypeDef ADC_ReadBoth(uint16_t *adc1_value, uint16_t *adc2_value);
HAL_StatusTypeDef ADC_UpdateReadings(void);
uint16_t ADC_GetRawIn(void);
uint16_t ADC_GetRawCan(void);
float ADC_GetVoltageIn(void);
float ADC_GetVoltageCan(void);
ADC_Readings_t ADC_GetReadings(void);

uint16_t adc_get_average(uint8_t adc_idx);



#endif
