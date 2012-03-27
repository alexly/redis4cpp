#include "replyparser.h"

namespace redis4cpp {
    
ReplyParser::ReplyParser(const char* reply, std::size_t bytesRecvd)
    :_reply(reply)
    ,_length(bytesRecvd)
    ,_cursor(0)
{
}

bool ReplyParser::NextReply(std::string& data)
{
    data.clear();
    
    if(_cursor >= _length)
        return false;
    
    return ParseNext(data);
}

bool ReplyParser::ParseNext(std::string& data)
{
    char type = _reply[_cursor];
    _cursor++;
    switch(type)
    {
        // Single line reply
        case '+':
            _currState = STATUS_REPLY;
            return ParseLine(data);
            
        // Error message
        case '-':
            _currState = ERROR_REPLY;
            return ParseLine(data);

        // Integer number    
        case ':':
            _currState = INTEGER_REPLY;
            return ParseLine(data);
            
        // Bulk reply
        case '$':
            _currState = BULK_REPLY;
            return ParseBulk(data);
            
        // Multi-bulk reply
        case '*':
            _currState = MULTI_BULK_REPLY;
            return ParseMultiBulk(data);
            
        default:
            //"Protocol error, got %s as reply type byte"
            std::cout << "Unknown reply type: '" << type << "'" << std::endl;
    };
    
    return false;
}


bool ReplyParser::ParseLine(std::string& data)
{    
    const char *p = _reply + _cursor;
    std::cout << "ParseLine: " << p << std::endl;
    const char* endline = strchr(p, '\r');
    if(endline && *(endline+1) == '\n') {
        std::size_t linelen = endline - p;
        data.append(p, linelen);
        _cursor += linelen + 2;
        return true;
    }

    // CRLF not found at all
    return false;
}

bool ReplyParser::ParseBulk(std::string& data)
{
    long long len;
    if(ParseLongLong(len)) {        
        if(len < 0) {
            data.append("NULL");
            return true;
        }
        else {

            const char *s = _reply + _cursor;
            std::cout << "ParseBulk: " << s << std::endl;
            data.append(s, len);
            _cursor += len + 2; // Skip "\r\n"
            return true;
        } 
    }
            
    return false;
}

bool ReplyParser::ParseMultiBulk(std::string& data)
{
    return false;
}
    
bool ReplyParser::ParseLongLong(long long& ll)
{
    const char *s = _reply + _cursor;
    
    long long v = 0;
    int dec, mult = 1;
    char c;

    if (*s == '-') {
        mult = -1;
        s++;
        _cursor++;
    } else if (*s == '+') {
        mult = 1;
        s++;
        _cursor++;
    }

    while ((c = *(s++)) != '\r') {
        _cursor++;
        dec = c - '0';
        if (dec >= 0 && dec < 10) {
            v *= 10;
            v += dec;
        } else {
            /* Should not happen... */
            throw std::runtime_error("ReplyParser::ParseLongLong: invalid digit");
            return false;
        }
    }

    _cursor += 2; // Skip "\r\n"
    ll= mult*v;
    return true;
}

}