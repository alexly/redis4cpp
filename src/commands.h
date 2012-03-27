#ifndef __REDIS4CPP_COMMANDS_H__
#define __REDIS4CPP_COMMANDS_H__

#include "interface.h"

namespace redis4cpp {

struct DefaultCommand : public CommandBase
{
    DefaultCommand(const std::string& cmd)
            :CommandBase("*3\r\n$3\r\nSET\r\n$5\r\nmykey\r\n$7\r\nmyvalue\r\n")
    {}
};

struct SetCommand : public CommandBase
{
    SetCommand()
            :CommandBase("*3\r\n$3\r\nSET\r\n$5\r\nmykey\r\n$7\r\nmyvalue\r\n")
    {}
};

struct GetCommand : public CommandBase
{
    GetCommand()
            :CommandBase("*23\r\n$3\r\nSET\r\n$5\r\nmykey\r\n")
    {}
};

}

#endif