#include "drv_usb.h"

// 实例化全局管理对象
Struct_USB_Manage_Object USB_Manage_Object;

/**
 * @brief  USB初始化函数
 * @param  Callback_Function: USB接收回调函数
 * @param  Rx_Buffer_Size: USB接收缓冲区大小
 * @retval None
 */
void USB_Init(USB_Rx_Callback Callback_Function, uint16_t Rx_Buffer_Size) 
{
    USB_Manage_Object.Callback_Function = Callback_Function;
    USB_Manage_Object.Rx_Buffer_Size = Rx_Buffer_Size;                   
}