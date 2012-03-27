#ifndef __REDIS4CPP_INTERFACE_H__
#define __REDIS4CPP_INTERFACE_H__

#include <streambuf>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/cstdint.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/lockfree/fifo.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/atomic.hpp>
#include <boost/smart_ptr.hpp>

namespace redis4cpp {

class CommandBase;
typedef boost::shared_ptr<CommandBase> CommandBasePtr;

typedef boost::lockfree::fifo<CommandBase*> FifoCommands;

typedef const std::string& BufferRef;

typedef boost::array<char, 5*15000>  ReceiveBuffer;

typedef boost::shared_array<char> RerplyPtr;

class CommandBase : public boost::enable_shared_from_this<CommandBase>
{
public:
    typedef boost::function<void(CommandBasePtr)> CommandCompleted;
    CommandBase(const std::string& command);
    CommandBase(const std::string& command, CommandCompleted completed);
    //~CommandBase() { std::cout << "destroy command" << std::endl; }
    void AddArgument(const std::string& arg);
    void AddArgument(boost::int16_t arg);
    void AddArgument(boost::int32_t arg);
    void AddArgument(boost::int64_t arg);
    void AddArgument(float arg);
    void AddArgument(double arg);
    
    BufferRef OutputBuffer();
    void SetResult(const std::string& data) { _result = data; }
    const std::string& Result() const { return _result; }
    void Completed();
    
     virtual bool Batched() { return false; }
     
protected:
    void WriteStringArg(const std::string& arg);
    
private:
    bool _alreadyBuild;
    boost::uint16_t _arguments;
    std::string _out;
    std::string _result;
    CommandCompleted _completed;
};

}

#endif