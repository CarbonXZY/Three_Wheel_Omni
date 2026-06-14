/**
 * @file dvc_sbus.h
 */

#ifndef DVC_SBUS_H
#define DVC_SBUS_H

#include "drv_uart.h"

#ifdef __cplusplus

#define SWITCH_UP (1)
#define SWITCH_DOWN (2)
#define SWITCH_MIDDLE (3)

#define SBUS_FRAME_LENGTH (25U)
#define SBUS_MAX_CHANNELS (18U)

#define SBUS_CH_RIGHT_X (1U)
#define SBUS_CH_RIGHT_Y (2U)
#define SBUS_CH_LEFT_Y (3U)
#define SBUS_CH_LEFT_X (4U)
#define SBUS_CH_VRA (5U)
#define SBUS_CH_VRB (6U)
#define SBUS_CH_SWITCH_A (7U)
#define SBUS_CH_SWITCH_B (8U)
#define SBUS_CH_SWITCH_C (9U)
#define SBUS_CH_SWITCH_D (10U)

#define SBUS_CH_YAW (SBUS_CH_RIGHT_X)
#define SBUS_CH_AUX_KNOB_1 (SBUS_CH_VRA)
#define SBUS_CH_AUX_KNOB_2 (SBUS_CH_VRB)

enum Enum_SBUS_Status//遥控器是否在线
{
    SBUS_Status_DISABLE = 0,
    SBUS_Status_ENABLE,
};

enum Enum_SBUS_Update_Status//遥控器数据是否更新
{
    SBUS_Status_DisUpdate = 0,
    SBUS_Status_Update,
};

enum Enum_SBUS_Switch_Status//拨杆开关状态 稳态和边沿态
{
    SBUS_Switch_Status_UP = 0,
    SBUS_Switch_Status_TRIG_UP_MIDDLE,
    SBUS_Switch_Status_TRIG_MIDDLE_UP,
    SBUS_Switch_Status_MIDDLE,
    SBUS_Switch_Status_TRIG_MIDDLE_DOWN,
    SBUS_Switch_Status_TRIG_DOWN_MIDDLE,
    SBUS_Switch_Status_DOWN,
};

struct Struct_SBUS_UART_Rx_Data
{
    // SBUS 原始串口帧，一帧固定 25 字节。
    // 其中 CH1~CH16 不是按“每个通道2字节”顺排，而是按 11bit 连续打包在 byte1~byte22 中。
    uint8_t Header;  // byte 0：帧头，正常应为 0x0F
    uint8_t Data_01; // byte 1：CH1~CH16 的打包数据起始字节
    uint8_t Data_02;
    uint8_t Data_03;
    uint8_t Data_04;
    uint8_t Data_05;
    uint8_t Data_06;
    uint8_t Data_07;
    uint8_t Data_08;
    uint8_t Data_09;
    uint8_t Data_10;
    uint8_t Data_11;
    uint8_t Data_12;
    uint8_t Data_13;
    uint8_t Data_14;
    uint8_t Data_15;
    uint8_t Data_16;
    uint8_t Data_17;
    uint8_t Data_18;
    uint8_t Data_19;
    uint8_t Data_20;
    uint8_t Data_21;
    uint8_t Data_22; // byte 22：CH1~CH16 的打包数据结束字节
    uint8_t Flags;   // byte 23：状态字，包含 CH17、CH18、frame lost、failsafe
    uint8_t Footer;  // byte 24：帧尾，常见为 0x00；部分 SBUS2 场景会带 slot footer
} __attribute__((packed));//packed保证结构体不被编译器插入填充字节

struct Struct_SBUS_Rx_Data
{
    float Right_X;
    float Right_Y;
    float Left_X;
    float Left_Y;
    float VRA;
    float VRB;
    float Yaw;
    Enum_SBUS_Switch_Status Switch_A;
    Enum_SBUS_Switch_Status Switch_B;
    Enum_SBUS_Switch_Status Switch_C;
    Enum_SBUS_Switch_Status Switch_D;
    uint16_t Channel[SBUS_MAX_CHANNELS];
    uint8_t Flags;
    bool Frame_Lost;
    bool Failsafe;
};

class Class_SBUS
{
public:
    void Init(UART_HandleTypeDef *huart_1);

    inline Enum_SBUS_Status Get_SBUS_Status();
    inline Enum_SBUS_Update_Status Get_SBUS_Update_Status();
    inline float Get_Right_X();
    inline float Get_Right_Y();
    inline float Get_Left_X();
    inline float Get_Left_Y();
    inline float Get_VRA();
    inline float Get_VRB();
    inline float Get_Yaw();
    inline Enum_SBUS_Switch_Status Get_Switch_A();
    inline Enum_SBUS_Switch_Status Get_Switch_B();
    inline Enum_SBUS_Switch_Status Get_Switch_C();
    inline Enum_SBUS_Switch_Status Get_Switch_D();

    void SBUS_UART_RxCpltCallback(uint8_t *Rx_Buffer, uint16_t Length);
/* 串口接收主入口：
   每当 UART5 收到一批数据，上层就会把 Buffer/Length 交给这里，
   这里负责找合法帧、解包、做 failsafe 判定。 */

    void TIM1msMod100_Alive_PeriodElapsedCallback();
/* 在线检测回调：
   周期性检查最近一段时间里成功帧计数有没有增长，
   决定 SBUS_Status 是 ENABLE 还是 DISABLE。 */
protected:
    Struct_UART_Manage_Object *UART_Manage_Object = nullptr;

    float Rocker_Offset = 1500.0f;//摇杆归一化参数
    float Rocker_Num = 500.0f;

    Struct_SBUS_UART_Rx_Data Now_UART_Rx_Data;//原始数据
    Struct_SBUS_UART_Rx_Data Pre_UART_Rx_Data;
    Struct_SBUS_Rx_Data Rx_Data;//解析数据
    Struct_SBUS_Rx_Data Pre_Rx_Data;

    uint32_t SBUS_Flag = 0U;//在线检测 每处理一帧合法数据加1
		uint32_t Pre_SBUS_Flag = 0U;//上一轮处理合法数据数量
    uint16_t Offline_Cnt = 0U;//离线累计次数
    uint16_t Error_Cnt = 0U;//解析失败次数
    bool SBUS_Failsafe_Active = false;//当前是否处在 failsafe

    uint8_t Switch_A_Status = SWITCH_UP;
    uint8_t Switch_B_Status = SWITCH_UP;
    uint8_t Switch_C_Status = SWITCH_UP;
    uint8_t Switch_D_Status = SWITCH_UP;
    uint8_t Pre_Switch_A_Status = SWITCH_UP;
    uint8_t Pre_Switch_B_Status = SWITCH_UP;
    uint8_t Pre_Switch_C_Status = SWITCH_UP;
    uint8_t Pre_Switch_D_Status = SWITCH_UP;

    Enum_SBUS_Status SBUS_Status = SBUS_Status_DISABLE;
    Enum_SBUS_Update_Status SBUS_Update_Status = SBUS_Status_DisUpdate;

		void Reset_Control_Data();//冷启动复位 缓存和状态清到初始值
    void Invalidate_Control_Data();//安全失效复位 丢帧时当前控制量失效
    void Judge_Three_Position_Switch(Enum_SBUS_Switch_Status *Switch, uint8_t Status, uint8_t Pre_Status);//三档开关解释器 输出稳态或触发态枚举
    void Judge_Update(const Struct_SBUS_Rx_Data &Pre_Data, const Struct_SBUS_Rx_Data &Now_Data);//比较是否变化
    void SBUS_Data_Process();//原始帧解释的主函数
    bool Decode_SBUS_Frame(uint8_t *Rx_Buffer, uint16_t Length);//在原始 UART 缓冲中找到最新合法 SBUS 帧
    uint16_t Get_Channel_Value(uint8_t channel_index) const;//安全读取某个通道值，越界或非法时回退到中值
    bool Is_Channel_Valid(uint16_t value) const;//判断某个通道值是否还落在 11bit 协议允许范围内
    float Normalize_Channel(uint16_t value, float sign) const;//原始通道值 -> [-1,1] 浮点值
    uint8_t Decode_Two_Position_Switch_Channel(uint16_t value, uint8_t fallback_status) const;//两档开关译码
    uint8_t Decode_Three_Position_Switch_Channel(uint16_t value, uint8_t fallback_status) const;//三档开关译码
};

inline Enum_SBUS_Status Class_SBUS::Get_SBUS_Status()
{
    return SBUS_Status;
}

inline Enum_SBUS_Update_Status Class_SBUS::Get_SBUS_Update_Status()
{
    return SBUS_Update_Status;
}

inline float Class_SBUS::Get_Right_X()
{
    return Rx_Data.Right_X;
}

inline float Class_SBUS::Get_Right_Y()
{
    return Rx_Data.Right_Y;
}

inline float Class_SBUS::Get_Left_X()
{
    return Rx_Data.Left_X;
}

inline float Class_SBUS::Get_Left_Y()
{
    return Rx_Data.Left_Y;
}

inline float Class_SBUS::Get_VRA()
{
    return Rx_Data.VRA;
}

inline float Class_SBUS::Get_VRB()
{
    return Rx_Data.VRB;
}

inline float Class_SBUS::Get_Yaw()
{
    return Rx_Data.Yaw;
}

inline Enum_SBUS_Switch_Status Class_SBUS::Get_Switch_A()
{
    return Rx_Data.Switch_A;
}

inline Enum_SBUS_Switch_Status Class_SBUS::Get_Switch_B()
{
    return Rx_Data.Switch_B;
}

inline Enum_SBUS_Switch_Status Class_SBUS::Get_Switch_C()
{
    return Rx_Data.Switch_C;
}

inline Enum_SBUS_Switch_Status Class_SBUS::Get_Switch_D()
{
    return Rx_Data.Switch_D;
}

#endif
#endif
