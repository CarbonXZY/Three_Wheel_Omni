#include "dvc_minipc.h"

/**
 * @brief  MiniPC Init
 * @param  None
 * @retval None
 */
void Class_MiniPC::Init(Struct_USB_Manage_Object &__USB_Manage_Object)
{ 
    USB_Manage_Object = &__USB_Manage_Object;
}


/**
 * @brief  MiniPC USB Rx Callback
 * @param  rx_data: 接收到的数据缓冲区
 * @retval None
 */
void Class_MiniPC::USB_Rx_Callback(uint8_t *rx_data)
{
    // 滑动窗口, 判断迷你主机是否在线
		MiniPC_Flag++;
    Data_Process(); 
}

/**
 * @brief  MiniPC 数据处理
 * @param  None
 * @retval None
 */
void Class_MiniPC::TIM_100ms_Alive_PeriodElapsedCallback()
{
    if (MiniPC_Flag == Pre_MiniPC_Flag)
    {
        MiniPC_Status = MiniPC_Status_DISABLE;
    }
    else
    {
        MiniPC_Status = MiniPC_Status_ENABLE;
    }

    Pre_MiniPC_Flag = MiniPC_Flag;
}


/**
 * @brief  MiniPC 数据处理
 * @param  None
 * @retval None
 */
void Class_MiniPC::Data_Process()
{ 
    // 安全检查：防止在未调用 Init 绑定前访问导致的硬件死机 (HardFault)
    if (USB_Manage_Object == nullptr) 
    {
        return; 
    }

    Struct_USB_Rx_Data *tmp_buffer = (Struct_USB_Rx_Data *)USB_Manage_Object->Rx_Buffer;
    Rx_Data.x_velocity = Math_Int_To_Float(tmp_buffer->x_velocity, 0, 65535, -8.0f, 8.0f); // 将接收到的速度值转换为m/s
    Rx_Data.y_velocity = Math_Int_To_Float(tmp_buffer->y_velocity, 0, 65535, -8.0f,8.0f); // 将接收到的速度值转换为m/s
    Rx_Data.omega = Math_Int_To_Float(tmp_buffer->omega, 0, 65535, -8.0f, 8.0f); // 将接收到的速度值转换为rad/s
}