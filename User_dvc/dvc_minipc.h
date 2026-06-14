/**
 * @file dvc_minipc.h
 * @author hzy (2370905113@qq.com)
 * @brief minipc host communication over usb
 * @version 0.2
 * @date 2026-05-26
 *
 * @copyright ROBOPIONEER (c) 2026
 *
 */
#ifndef __DVC_MINIPC_H__
#define __DVC_MINIPC_H__ 

/* Includes ------------------------------------------------------------------*/
#include "drv_usb.h"
#include "drv_math.h"
#include "stm32h7xx_hal.h"

/* Exported types ------------------------------------------------------------*/
/**
 * @brief 迷你主机状态枚举
 * @note 该枚举用于表示迷你主机的当前状态
 */
enum Enum_MiniPC_Status
{
    MiniPC_Status_DISABLE = 0,
    MiniPC_Status_ENABLE,
};

/**
 * @brief 迷你主机数据结构体
* @note 该结构体用于存储从迷你主机接收到的数据
* @param info 描述信息
* @param x_velocity 底盘x轴速度，0-65535
* @param y_velocity 底盘y轴速度，0-65535
* @param omega 底盘角速度，0-65535
 */
struct Struct_USB_Rx_Data
{
   //上位机控制指令
    char info[5];
    //底盘x轴速度，单位m/s
    uint16_t x_velocity;
    //底盘y轴速度，单位m/s
    uint16_t y_velocity;
    //底盘角速度，单位rad/s
    uint16_t omega;
} __attribute__((packed));

/**
 * @brief 处理后的MiniPC数据结构体
 * @note 该结构体用于存储处理后的MiniPC数据
 * @param info 描述信息
 * @param x_velocity 底盘x轴速度，单位m/s
 * @param y_velocity 底盘y轴速度，单位m/s
 * @param omega 底盘角速度，单位rad/s
 */
struct Struct_MiniPC_Rx_Data
{
    //下位机反馈信息
    char info[5];
    //底盘x轴速度，单位m/s
    float x_velocity;
    //底盘y轴速度，单位m/s
    float y_velocity;
    //底盘角速度，单位rad/s
    float omega;
} __attribute__((packed));

class Class_MiniPC
{
public: 

    void Init(Struct_USB_Manage_Object &USB_Manage_Object);
    void USB_Rx_Callback(uint8_t *rx_data);
    inline float Get_MiniPC_Velocity_X();
    inline float Get_MiniPC_Velocity_Y();
    inline float Get_MiniPC_Omega();
    inline Enum_MiniPC_Status Get_MiniPC_Status();
    void TIM_100ms_Alive_PeriodElapsedCallback();
private:
    //绑定的USB管理对象
    Struct_USB_Manage_Object *USB_Manage_Object;

    //处理后的MiniPC数据
    Struct_MiniPC_Rx_Data Rx_Data;
    //标志位，用于判断迷你主机是否在线
    uint32_t MiniPC_Flag = 0;
    uint32_t Pre_MiniPC_Flag = 0;
    Enum_MiniPC_Status MiniPC_Status = MiniPC_Status_DISABLE;
    //数据处理
    void Data_Process();

};
/**
 * @brief 获取当前底盘x轴速度
 * 
 */
inline float Class_MiniPC::Get_MiniPC_Velocity_X()
{
    return Rx_Data.x_velocity;
}
/**
 * @brief 获取当前底盘y轴速度
 * 
 */
inline float Class_MiniPC::Get_MiniPC_Velocity_Y()
{
    return Rx_Data.y_velocity;
}

/**
 * @brief 获取当前底盘角速度
 * 
 */
inline float Class_MiniPC::Get_MiniPC_Omega()
{
    return Rx_Data.omega;
}


inline Enum_MiniPC_Status Class_MiniPC::Get_MiniPC_Status()
{
    return MiniPC_Status;
}
#endif