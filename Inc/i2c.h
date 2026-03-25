#ifndef I2C_H
#define I2C_H

#include "main.h"
#include "uart.h"
#include <stdint.h>
#include <stdio.h>


// Numberings correspond to voltage labelings in Altium -- i.e. SHUNT1 uses IN1_POS_12V
#define SHUNT1_ADDR   (0x45 << 1)
#define SHUNT2_ADDR   (0x41 << 1)
#define SHUNT3_ADDR   (0x44 << 1)
#define SHUNT4_ADDR   (0x40 << 1)
#define NUM_SENSORS 4

#define SHUNT_CONFIG_ADDR 0x00
#define SHUNT_CUR_ADDR 0x04
#define SHUNT_CAL_ADDR 0x05
#define SHUNT_MASK_ADDR 0x06
#define SHUNT_AUL_ADDR 0x07


#define I2C_TIMEOUT 100

typedef struct
{
    uint8_t addr;
    uint16_t alert_pin;
    volatile uint8_t alert_flag;
} Sensor_t;

extern Sensor_t sensors[NUM_SENSORS];

HAL_StatusTypeDef I2C_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef I2C_CheckReady(I2C_HandleTypeDef *hi2c, uint8_t sensor_index);

/* Read / write helpers */
HAL_StatusTypeDef I2C_ReadReg(I2C_HandleTypeDef *hi2c,
                                 uint8_t sensor_index,
                                 uint8_t reg,
                                 int16_t *data);

HAL_StatusTypeDef I2C_WriteReg(I2C_HandleTypeDef *hi2c,
                                  uint8_t sensor_index,
                                  uint8_t reg,
                                  const uint8_t *data,
                                  uint16_t len);

HAL_StatusTypeDef I2C_ReadCurrents(I2C_HandleTypeDef *hi2c, char *retmsg, uint16_t len);
/* Alert handling */
HAL_StatusTypeDef I2C_HandleAlert(I2C_HandleTypeDef *hi2c, uint8_t sensor_idx);


#endif
