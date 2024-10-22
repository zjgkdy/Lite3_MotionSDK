#ifndef _ROBOT_CMD_H_
#define _ROBOT_CMD_H_

#include <iostream>

struct CommandHead
{
    uint32_t code;
    uint32_t paramters_size;
    uint32_t type;
};

#endif