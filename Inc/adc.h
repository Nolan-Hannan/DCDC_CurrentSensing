#ifndef ADC_H
#define ADC_H

#include "main.h"
#include <stdint.h>

#define ADC_MAX_COUNT   4095.0f
#define ADC_VREF        3.3f
#define ADC_VQUIESCENT 1.65f
#define ADC_SENS 0.044f

// For this naming convention, PA0 must attach to the IN_PWR voltage, PA2 must attach to the CAN voltage
typedef struct
{
    uint16_t in_raw;
    uint16_t can_raw;
    float in_volt;
    float can_volt;
} ADC_Readings_t;

void ADC_App_Init(void);
HAL_StatusTypeDef ADC_ReadBoth(uint16_t *adc1_value, uint16_t *adc2_value);
HAL_StatusTypeDef ADC_UpdateReadings(void);
uint16_t ADC_GetRawIn(void);
uint16_t ADC_GetRawCan(void);
float ADC_GetVoltageIn(void);
float ADC_GetVoltageCan(void);
ADC_Readings_t ADC_GetReadings(void);


#endif
