#ifndef __REDIS4CPP_SENDER_H__
#define __REDIS4CPP_SENDER_H__

#include "interface.h"

namespace redis4cpp {

const short multicast_port = 6379;

class Sender
{
friend class DataAccess;
public:
    typedef boost::function<void(CommandBase*)> QuerySended;
    Sender(boost::asio::io_service& io_service
            ,boost::asio::ip::tcp::socket& socket
            ,QuerySended sended
            ,boost::asio::io_service::strand& strand);

    // Add to lock-free queque, or send if curretly not processing
    void AsyncCommand(CommandBase* cmd);
  
private:
    void DoSend(CommandBase* cmd = NULL);
    void HandleSended(const boost::system::error_code& error, CommandBase* cmd);
    
private:
    boost::asio::ip::tcp::socket& _socket;
    boost::asio::io_service::strand& _strand;
    // Free-lock buffer commands of to send
    FifoCommands _tosendcommands;
    boost::atomic<bool> _inprocess;
    QuerySended QuerySendedCallback;
};

}

#endif