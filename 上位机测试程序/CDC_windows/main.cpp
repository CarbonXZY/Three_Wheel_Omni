// 串口通信库头文件
#include "CSerialPort/SerialPort.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <cstdint>
#include <cstdio>

#pragma pack(push, 1)  // 强制1字节对齐，确保结构体与下位机一致
/**
 * @brief 迷你主机数据结构体
 * @note 发送给迷你主机的二进制数据帧（13字节）
 * @param info 描述信息，5字节（前4字节为命令字，末尾隐含0）
 * @param x_velocity 底盘x轴速度，uint16范围0-65535，对应-8.0~8.0 m/s
 * @param y_velocity 底盘y轴速度，uint16范围0-65535，对应-8.0~8.0 m/s
 * @param omega 底盘角速度，uint16范围0-65535，对应-8.0~8.0 rad/s
 */
struct Struct_USB_Rx_Data
{
    char info[5];
    uint16_t x_velocity;
    uint16_t y_velocity;
    uint16_t omega;
};
#pragma pack(pop)  // 恢复默认对齐

/**
 * @brief 将float值从[min_val, max_val]线性映射到[0, 65535]
 * @param val 待转换的浮点数值
 * @param min_val 最小边界值
 * @param max_val 最大边界值
 * @return 映射后的uint16值
 */
static uint16_t Math_Float_To_Int(float val, float min_val, float max_val)
{
    // 归一化到[0, 1]区间
    float normalized = (val - min_val) / (max_val - min_val);
    // 限幅保护
    if (normalized < 0.0f) normalized = 0.0f;
    if (normalized > 1.0f) normalized = 1.0f;
    // 缩放到0-65535并四舍五入
    return (uint16_t)(normalized * 65535.0f + 0.5f);
}

int main() {
    std::string str;
    std::cout << "serial number?: ";
    std::cin >> str;

    // 拼接串口名，如"COM3"
    std::string portName = "COM" + str;
    std::cout << "Opening serial: " << portName << std::endl;

    // 创建串口对象
    itas109::CSerialPort cdc;

    // 初始化串口参数：115200波特率、无校验、8数据位、1停止位、无流控
    cdc.init(portName.c_str(),
             itas109::BaudRate115200,
             itas109::ParityNone,
             itas109::DataBits8,
             itas109::StopOne,
             itas109::FlowNone);

    // 使用同步模式，读写操作阻塞等待完成
    cdc.setOperateMode(itas109::SynchronousOperate);

    // 打开串口，失败则退出
    if (!cdc.open()) {
        std::cout << "Failed to open " << portName << std::endl;
        std::cout << "Error: " << cdc.getLastErrorMsg() << std::endl;
        system("pause");
        return 1;
    }

    std::cout << "Port opened successfully!" << std::endl;
    std::cout << "Sending Struct_USB_Rx_Data binary frames..." << std::endl;

    // 构造发送数据
    Struct_USB_Rx_Data tx_data;
    memcpy(tx_data.info, "CMD", 4);  // 前4字节设为"CMD"，留一个字节给末尾0

    int frame_count = 0;
    int vx = -8, vy = 0, vo = 0;  // 速度从-8开始递增到8

    // 循环发送数据帧，共10000帧
    while (frame_count < 10000) {
        // 步进式扫描：vx变化最快，vy次之，vo最慢
        vx++;
        if (vx > 8) { vx = -8; vy++; }
        if (vy > 8) { vy = -8; vo++; }
        if (vo > 8) break;

        // 将速度值转换为uint16范围
        tx_data.x_velocity = Math_Float_To_Int((float)vx, -8.0f, 8.0f);
        tx_data.y_velocity = Math_Float_To_Int((float)vy, -8.0f, 8.0f);
        tx_data.omega      = Math_Float_To_Int((float)vo, -8.0f, 8.0f);

        // 发送结构体（13字节二进制数据）
        int bytesWritten = cdc.writeData((char*)&tx_data, sizeof(tx_data));
        std::cout << "[" << frame_count << "] vx=" << vx
                  << " vy=" << vy << " vo=" << vo
                  << " | raw: x=" << tx_data.x_velocity
                  << " y=" << tx_data.y_velocity
                  << " o=" << tx_data.omega
                  << " | sent " << bytesWritten << " bytes" << std::endl;

        frame_count++;
        Sleep(10);  // 每10ms发送一帧（100Hz）
    }

    std::cout << "Done. Sent " << frame_count << " frames." << std::endl;

    // 尝试读取下位机响应
    char readBuffer[256] = {0};
    int bytesRead = cdc.readData(readBuffer, sizeof(readBuffer) - 1);
    if (bytesRead > 0) {
        readBuffer[bytesRead] = '\0';
        std::cout << "Received: " << readBuffer << std::endl;
    }

    // 关闭串口
    cdc.close();
    std::cout << "Serial port closed" << std::endl;

    system("pause");
    return 0;
}
