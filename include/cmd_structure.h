#ifndef _CMD_STRUCTURE_H_
#define _CMD_STRUCTURE_H_

#include <iostream>
#include "cmd_code.h"

// 简单指令结构体
struct CommandHead
{
    uint32_t code;           // 指令码
    uint32_t paramters_size; // 指令值
    uint32_t type;           // 指令类型：简单指令为0
};

// 复杂指令结构体
const uint32_t kDataSize = 256;
struct CommandHead
{
    uint32_t code;           // 指令码
    uint32_t paramters_size; // 数据长度
    uint32_t type;           // 指令类型：复杂指令为1
};
struct Command
{
    CommandHead head;         // 复杂指令头
    uint32_t data[kDataSize]; // 数据内容
};

// 机器人状态信息结构体
struct RobotStateUpload
{

    int robot_basic_state;              // 基本运动状态：1-趴下，4-准备起立，5-正在起立，6-力控，7-正在趴下，8-失控保护，9-姿态调整，11-翻身，17-回零，18-后空翻，20-打招呼
    int robot_gait_state;               // 当前步态：0-平地低速，2-通用越障，4-平地中速，5-平地高速，6-抓地越障，12-太空步，13-高踏步越障
    double rpy[3];                      // IMU角度：{roll, pitch, yaw}
    double rpy_vel[3];                  // IMU角速度：{roll_vel, pitch_vel, yaw_vel}
    double xyz_acc[3];                  // IMU加速度：{x_acc, y_acc, z_acc}
    double pos_world[3];                // 世界系下的位姿：{x, y, z}
    double vel_world[3];                // 世界系下的速度：{x_vel, y_vel, yaw_vel}
    double vel_body[3];                 // body系下的速度：{x_vel, y_vel, yaw_vel}
    unsigned touch_down_and_stair_trot; // 无效，占位用
    bool is_charging;                   // 无效，占位用
    unsigned error_state;               // 无效，占位用
    int robot_motion_state;             // 动作状态：0-robot_basic_state对应的状态，1-robot_gait_state对应的步态，2-扭身体，4-扭身跳，11-向前跳
    double battery_level;               // 电池电量百分比的小数形式
    int task_state;                     // 无效，占位用
    bool is_robot_need_move;            // 机器人的平衡状态：0-可保持平衡，1-无法保持平衡、踏步调整姿态
    bool zero_position_flag;            // 回零标志位：0-未完成或已退出回零状态，1-已回零
    double ultrasound[2];               // 超声波数据(有效量程[0.28, 4.50])：{forward_distance, backward_distance} (m)
};

// ！！！12个关节排列顺序：左前侧摆关节、左前髋关节、左前膝关节、右前侧摆关节、右前髋关节、右前膝关节、
// ！！！左后侧摆关节、左后髋关节、左后膝关节、右后侧摆关节、右后髋关节、右后膝关节

// 机器人关节角度信息结构体
struct RobotJointAngle
{
    double joint_angle[12]; // 单位：rad
};

// 机器人关节角速度信息结构体
struct RobotJointVel
{
    double joint_vel[12]; // 单位：rad/s
};

#endif