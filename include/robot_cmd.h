#ifndef _ROBOT_CMD_H_
#define _ROBOT_CMD_H_

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

#endif