#include "sender.h"
#include <boost/lexical_cast.hpp>
#include <boost/memory_order.hpp>

namespace redis4cpp {
    
Sender::Sender(boost::asio::io_service& io_service
                ,boost::asio::ip::tcp::socket& socket
                ,QuerySended sended
                ,boost::asio::io_service::strand& strand)

    : _socket(socket)
    ,QuerySendedCallback(sended)
    ,_strand(strand)
    ,_inprocess(false)
{
}    

void Sender::AsyncCommand(CommandBase* cmd)
{
    DoSend(cmd);
}

void Sender::HandleSended(const boost::system::error_code& error, CommandBase* cmd)
{
    _inprocess.store(false, boost::memory_order_acquire);
    
    if (!error)
    {
        QuerySendedCallback(cmd);
        DoSend();
    }
    else
    {
        std::cout << "Sender::HandleSended error: " << error.message() << std::endl;
        delete cmd;
    }
}

void Sender::DoSend(CommandBase* cmd)
{                
    if(cmd != NULL)
        _tosendcommands.enqueue(cmd);
        
    bool expected = false;
    
    // Compare current value with expected, change it to desired if matches. 
    // Returns true if an exchange has been performed, and always writes 
    // the previous value back in expected.
    
    if(_inprocess.compare_exchange_strong(expected, true, boost::memory_order_acquire))
    {
        CommandBase* tosend;
        if(_tosendcommands.dequeue(tosend)) 
        {
            boost::asio::async_write(_socket
                                ,boost::asio::buffer(tosend->OutputBuffer())
                                ,boost::bind(&Sender::HandleSended, this
                                ,boost::asio::placeholders::error, tosend));
        }
    }
}

}

