#include "can.h"

/* ================== STATIC ================== */

static CAN_HandleTypeDef *g_hcan = NULL;

static CAN_Message_t rx_queue[CAN_RX_QUEUE_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;

/* ================== INIT ================== */

HAL_StatusTypeDef CAN_Init(CAN_HandleTypeDef *hcan)
{
    g_hcan = hcan;

    CLEAR_BIT(hcan->Instance->MCR, CAN_MCR_SLEEP); // Ensure awake

    // 1. Configure filter (accept NO messages)
    CAN_FilterTypeDef filter = {0};

    filter.FilterActivation = ENABLE;
    filter.FilterBank = 0;
    filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filter.FilterIdHigh = (0x7FF << 5); //FIXME: Assign tighter ID range. Currently accepts none
    filter.FilterIdLow = 0x0000; //FIXME: Assign tighter ID range. Currently accepts none
    filter.FilterMaskIdHigh = (0x7FF << 5); //FIXME: Assign tighter ID range. Currently accepts none
    filter.FilterMaskIdLow = 0x0000; //FIXME: Assign tighter ID range. Currently accepts none
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;

    if (HAL_CAN_ConfigFilter(g_hcan, &filter) != HAL_OK)
        return HAL_ERROR;

    // 2. Start CAN
    if (HAL_CAN_Start(g_hcan) != HAL_OK)
        return HAL_ERROR;

    // 3. Enable RX interrupt
    if (HAL_CAN_ActivateNotification(g_hcan,
        CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

/* ================== TRANSMIT ================== */

HAL_StatusTypeDef CAN_Transmit(uint32_t id, uint8_t *data, uint8_t len)
{
    CAN_TxHeaderTypeDef header;
    uint32_t mailbox;

    header.StdId = id & 0x7FF;
    header.ExtId = 0;
    header.IDE   = CAN_ID_STD;
    header.RTR   = CAN_RTR_DATA;
    header.DLC   = len;
    header.TransmitGlobalTime = DISABLE;

    return HAL_CAN_AddTxMessage(g_hcan, &header, data, &mailbox);
}

/* ================== RECEIVE ================== */

static void CAN_PushMessage(CAN_Message_t *msg)
{
    uint8_t next = (rx_head + 1) % CAN_RX_QUEUE_SIZE;

    if (next != rx_tail) // not full
    {
        rx_queue[rx_head] = *msg;
        rx_head = next;
    }
}

/* HAL callback (called automatically) */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if (hcan != g_hcan) return;

    CAN_RxHeaderTypeDef header;
    uint8_t data[8];

    if (HAL_CAN_GetRxMessage(hcan,
        CAN_RX_FIFO0,
        &header,
        data) != HAL_OK)
    {
        return;
    }

    CAN_Message_t msg;

    msg.id  = (header.IDE == CAN_ID_STD) ?
                header.StdId : header.ExtId;
    msg.dlc = header.DLC;

    for (uint8_t i = 0; i < msg.dlc; i++)
        msg.data[i] = data[i];

    CAN_PushMessage(&msg);
}

/* ================== QUEUE ================== */

uint8_t CAN_Available(void)
{
    return (rx_head != rx_tail);
}

HAL_StatusTypeDef CAN_Read(CAN_Message_t *msg)
{
    if (rx_head == rx_tail)
        return HAL_ERROR;

    *msg = rx_queue[rx_tail];
    rx_tail = (rx_tail + 1) % CAN_RX_QUEUE_SIZE;

    return HAL_OK;
}



