#ifndef CAN_H
#define CAN_H

#include "main.h"

/* ================== CONFIG ================== */
#define CAN_RX_QUEUE_SIZE  8
#define CAN_ALERT_ID 0x400
#define CAN_POWER_DATA_ID 0x500
#define CAN_MESSAGE_LENGTH 12
#define CAN_ALERT_LENGTH 6 + CAN_MESSAGE_LENGTH


/* ================== TYPES ================== */
typedef struct
{
    uint32_t id;
    uint8_t  data[8];
    uint8_t  dlc;
} CAN_Message_t;

/* ================== API ================== */

// Init (filter + start + interrupts)
HAL_StatusTypeDef CAN_Init(CAN_HandleTypeDef *hcan);

// Transmit
HAL_StatusTypeDef CAN_Transmit(uint32_t id, uint8_t *data, uint8_t len);

// Receive queue interface
uint8_t CAN_Available(void);
HAL_StatusTypeDef CAN_Read(CAN_Message_t *msg);



#endif
