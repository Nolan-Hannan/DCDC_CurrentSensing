#ifndef CAN_H
#define CAN_H

#include "main.h"

/* ================== CONFIG ================== */
#define CAN_RX_QUEUE_SIZE  8

/* This board sends everything on one ID. 0x301 and 0x302 are the Pi board's GPS and wind.
 * Classic CAN carries at most 8 data bytes, so telemetry goes out as two frames and
 * data[0] is the frame index:
 *   frame 0: [0, in, CAN, load 1 (2 bytes each), alarm flags]
 *   frame 1: [1, load 2, load 3, load 4 (2 bytes each), 0]
 * Currents are signed 16-bit mA, big endian. Alarm flag bits: 0 in, 1 CAN, 2 to 5 load 1 to 4.
 * The flags are nonzero only in frames sent right after an alarm. */
#define CAN_DCDC_ID 0x300
#define CAN_FRAME_LENGTH 8
#define CAN_TELEMETRY_LENGTH 12


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
