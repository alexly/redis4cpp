#include "interface.h"
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>

namespace redis4cpp {
    
const char CRLF[] = "\r\n";

int redisFormatCommandArgv(char **target, int argc, const char **argv, const size_t *argvlen) {
    char *cmd = NULL; /* final command */
    int pos; /* position in final command */
    size_t len;
    int totlen, j;

    /* Calculate number of bytes needed for the command */
    //totlen = 1+intlen(argc)+2;
    for (j = 0; j < argc; j++) {
        len = argvlen ? argvlen[j] : strlen(argv[j]);
        //totlen += 1+intlen(len)+2+len+2;
    }

    /* Build the command at protocol level */
    //cmd = malloc(totlen+1);
    //if (!cmd) redisOOM();
    pos = sprintf(cmd,"*%d\r\n",argc);
    for (j = 0; j < argc; j++) {
        len = argvlen ? argvlen[j] : strlen(argv[j]);
        pos += sprintf(cmd+pos,"$%zu\r\n",len);
        memcpy(cmd+pos,argv[j],len);
        pos += len;
        cmd[pos++] = '\r';
        cmd[pos++] = '\n';
    }
    assert(pos == totlen);
    cmd[totlen] = '\0';
    *target = cmd;
    return totlen;
}

CommandBase::CommandBase(const std::string& command)
    :_arguments(0)
    ,_alreadyBuild(false)
{            
    WriteStringArg(command);
}

CommandBase::CommandBase(const std::string& command, CommandCompleted completed)
    :_arguments(0)
    ,_completed(completed)
    ,_alreadyBuild(false)
{
    WriteStringArg(command);
}

void CommandBase::AddArgument(const std::string& arg)
{
    WriteStringArg(boost::lexical_cast<std::string>(arg));
}

void CommandBase::AddArgument(boost::int16_t arg)
{
    WriteStringArg(boost::lexical_cast<std::string>(arg));
}

void CommandBase::AddArgument(boost::int32_t arg)
{
    WriteStringArg(boost::lexical_cast<std::string>(arg));
}

void CommandBase::AddArgument(boost::int64_t arg)
{
    WriteStringArg(boost::lexical_cast<std::string>(arg));
}

void CommandBase::AddArgument(float arg)
{
    WriteStringArg(boost::lexical_cast<std::string>(arg));
}

void CommandBase::AddArgument(double arg)
{
    WriteStringArg(boost::lexical_cast<std::string>(arg));
}

void CommandBase::WriteStringArg(const std::string& arg)
{
    // Every Redis command or data transmitted by the client 
    // and the server is terminated by \r\n (CRLF).
    // Example: 
    // "*3\r\n$3\r\nSET\r\n$5\r\nmykey\r\n$7\r\nmyvalue\r\n"
    
    _arguments++;
    _out += "$" + boost::lexical_cast<std::string>(arg.size()) + CRLF + arg + CRLF;
}

BufferRef CommandBase::OutputBuffer()
{
    if(!_alreadyBuild) {
        _alreadyBuild = true;
        _out.insert(0, "*" + boost::lexical_cast<std::string>(_arguments) + CRLF);
    }
    
    return _out;
}
    
void CommandBase::Completed()
{
    if(!_completed.empty())
        _completed(shared_from_this());
}

}

