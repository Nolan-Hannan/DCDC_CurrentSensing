#include "i2c.h"

Sensor_t sensors[4] =
{
    { .addr = SHUNT1_ADDR, .alert_pin = ALERT1_Pin, .alert_flag = 0 },
    { .addr = SHUNT2_ADDR, .alert_pin = ALERT2_Pin, .alert_flag = 0 },
    { .addr = SHUNT3_ADDR, .alert_pin = ALERT3_Pin, .alert_flag = 0 },
    { .addr = SHUNT4_ADDR, .alert_pin = ALERT4_Pin, .alert_flag = 0 }
};

HAL_StatusTypeDef I2C_Init(I2C_HandleTypeDef *hi2c)
{
    HAL_StatusTypeDef status;

    for (uint8_t i = 0; i < NUM_SENSORS; i++)
    {
        uint8_t config_data[2] = { 0x44, 0xDF };   // Choosing 16 averages, 588 us conversion times, continuous shunt and bus readings
        uint8_t cal_data_10mO[2]    = { 0x10, 0x62 };
        uint8_t cal_data_5mO[2]    = { 0x0D, 0x1B };
        uint8_t aul_data_10mO[2] = {0x32, 0x00}; // Overcurrent alert at 3.2 A
        uint8_t aul_data_5mO[2] = {0x3E, 0x80}; // Overcurrent alert at 8 A
        uint8_t alert_setting[2] = {0x80, 0x01};

        sensors[i].alert_flag = 0;

        status = I2C_CheckReady(hi2c, i);
        if (status != HAL_OK)
            return status;

        status = I2C_WriteReg(hi2c, i, SHUNT_CONFIG_ADDR, config_data, 2);
        if (status != HAL_OK)
            return status;

        status = I2C_WriteReg(hi2c, i, SHUNT_MASK_ADDR, alert_setting, 2);
        if (status != HAL_OK)
                   return status;

        if(i >= 2){
			status = I2C_WriteReg(hi2c, i, SHUNT_CAL_ADDR, cal_data_10mO, 2);
        }
        else{
        	status = I2C_WriteReg(hi2c, i, SHUNT_CAL_ADDR, cal_data_5mO, 2);
        }
        if (status != HAL_OK)
            return status;

        if(i >= 2){
			status = I2C_WriteReg(hi2c, i, SHUNT_AUL_ADDR, aul_data_10mO, 2);
		}
		else{
			status = I2C_WriteReg(hi2c, i, SHUNT_AUL_ADDR, aul_data_5mO, 2);
		}
		if (status != HAL_OK)
			return status;
    }

    return HAL_OK;
}

HAL_StatusTypeDef I2C_CheckReady(I2C_HandleTypeDef *hi2c, uint8_t sensor_index)
{
    if (sensor_index >= NUM_SENSORS)
    {
        return HAL_ERROR;
    }

    return HAL_I2C_IsDeviceReady(hi2c, sensors[sensor_index].addr, 3, I2C_TIMEOUT);
}

HAL_StatusTypeDef I2C_ReadReg(I2C_HandleTypeDef *hi2c, uint8_t sensor_index, uint8_t reg, int16_t *value)
	{
	    uint8_t data[2];

	    if (sensor_index >= NUM_SENSORS || value == NULL)
	        return HAL_ERROR;

	    HAL_StatusTypeDef status =
	        HAL_I2C_Mem_Read(hi2c,
	                         sensors[sensor_index].addr,
	                         reg,
	                         I2C_MEMADD_SIZE_8BIT,
	                         data,
	                         2,
	                         I2C_TIMEOUT);

	    if (status != HAL_OK)
	        return status;

	    *value = (data[0] << 8) | data[1];   // INA230 is big-endian

	    return HAL_OK;
	}

HAL_StatusTypeDef I2C_WriteReg(I2C_HandleTypeDef *hi2c, uint8_t sensor_index, uint8_t reg, const uint8_t *data, uint16_t len)
{
    if (sensor_index >= NUM_SENSORS || data == NULL)
    {
        return HAL_ERROR;
    }

    return HAL_I2C_Mem_Write(hi2c, sensors[sensor_index].addr, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t *)data, len, I2C_TIMEOUT);
}

HAL_StatusTypeDef I2C_ReadCurrents(I2C_HandleTypeDef *hi2c, char *retmsg, uint16_t len) {
	int32_t shuntCurVals[NUM_SENSORS];
	int16_t cur = 0;
	HAL_StatusTypeDef status;
	for(uint8_t i = 0; i < NUM_SENSORS; ++i){
		status = I2C_ReadReg(hi2c, i, SHUNT_CUR_ADDR, &cur);
		if(status != HAL_OK) {
			return status;
		}
		if(i >= 2) {
			shuntCurVals[i] = (int32_t)(cur * 0.00012208 * 1000);
		} else {
			shuntCurVals[i] = (int32_t)(cur * 0.0003052 * 1000);
		}
	}
	snprintf(retmsg, len, "load1Cur: %d mA\r\nload2Cur: %d mA\r\nload3Cur: %d mA\r\nload4Cur: %d mA\r\n",
			(int)shuntCurVals[0],
			(int)shuntCurVals[1],
			(int)shuntCurVals[2],
			(int)shuntCurVals[3]);

	return HAL_OK;
}

HAL_StatusTypeDef I2C_HandleAlert(I2C_HandleTypeDef *hi2c, uint8_t sensor_idx) {
	int16_t cur_raw = 0;
	if (I2C_ReadReg(hi2c, sensor_idx, SHUNT_CUR_ADDR, &cur_raw) != HAL_OK)
	{
		Error_Handler();
	}

	int32_t current_mA;

	if (sensor_idx >= 2)
		current_mA = (int32_t)(cur_raw * SHUNT_SENS_10MO * 1000.0f);
	else
		current_mA = (int32_t)(cur_raw * SHUNT_SENS_5MO * 1000.0f);

	int16_t mask_raw;
	if (I2C_ReadReg(hi2c, sensor_idx, SHUNT_MASK_ADDR, &mask_raw) != HAL_OK) {
		Error_Handler();
	}

	char msg[64];

	snprintf(msg, sizeof(msg),
			 "HIGH CURRENT: Load[%d] %d mA\r\n",
			 sensor_idx + 1,
			 (int)current_mA);

	UART_SendLine(msg);

	return HAL_OK;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	for(uint8_t i = 0; i < NUM_SENSORS; ++i) {
		if(sensors[i].alert_pin == GPIO_Pin) {
			sensors[i].alert_flag = 1;
		}
	}
}
