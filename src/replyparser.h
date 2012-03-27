#ifndef __REDIS4CPP_REPLY_PARSERH__
#define __REDIS4CPP_REPLY_PARSERH__

#include "interface.h"

namespace redis4cpp {

enum ReplyType
{
    BAD_REPLY           = 0,
    STATUS_REPLY        = 1,
    ERROR_REPLY         = 2,
    INTEGER_REPLY       = 3,
    BULK_REPLY          = 4,
    MULTI_BULK_REPLY    = 5
};

class Reply
{
};

class ReplyParser
{
public:
    ReplyParser(const char* reply, std::size_t bytesRecvd);
    
    bool NextReply(std::string& data);
    
private:
    bool ParseNext(std::string& data);
    bool ParseLine(std::string& data);
    bool ParseBulk(std::string& data);
    bool ParseMultiBulk(std::string& data);
    
    bool ParseLongLong(long long& ll);
private:
    ReplyType _currState;
    std::size_t _cursor;
    const char* _reply;
    std::size_t _length;
};

}

#endif