#ifndef __DRV_USB_H
#define __DRV_USB_H

/*----------------includes----------------------*/
#include "stm32h7xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

//缓冲区字节长度
#define USB_RX_BUFFER_SIZE 48
#define USB_TX_BUFFER_SIZE 48

/**
 * @brief  USB接收回调函数
 * @param  Buffer: 接收到的数据缓冲区
 * @param  Length: 接收到的数据长度
 * @retval None
 */
typedef void (*USB_Rx_Callback)(uint8_t *Buffer, uint32_t Length);  // FIXED: removed parentheses around void

/**
 * @brief  USB管理对象
 * @note   USB管理对象结构体
 */
typedef struct 
{
    uint8_t Rx_Buffer[USB_RX_BUFFER_SIZE];
    uint8_t Tx_Buffer[USB_TX_BUFFER_SIZE];
    uint16_t Rx_Buffer_Size;
    USB_Rx_Callback Callback_Function;
} Struct_USB_Manage_Object;

extern Struct_USB_Manage_Object USB_Manage_Object;

/**
 * @brief  USB初始化函数
 * @param  Callback_Function: USB接收回调函数
 * @param  Rx_Buffer_Size: USB接收缓冲区大小
 * @retval None
 */
void USB_Init(USB_Rx_Callback Callback_Function, uint16_t Rx_Buffer_Size);  // FIXED: added missing Rx_Buffer_Size parameter

#ifdef __cplusplus
}
#endif

// typedef struct 
// {
//     //上位机控制指令
//     char info[5];
//     //底盘x轴速度，单位m/s
//     float x_velocity;
//     //底盘y轴速度，单位m/s
//     float y_velocity;
//     //底盘角速度，单位rad/s
//     float omega;
// } MiniPC_Rx_t;

#endif // __DRV_USB_H