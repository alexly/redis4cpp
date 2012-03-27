#ifndef __REDIS4CPP_DataAccess_H__
#define __REDIS4CPP_DataAccess_H__

#include "interface.h"
#include "sender.h"
#include "reciever.h"

namespace redis4cpp {

class DataAccess
{
public:
    DataAccess(boost::asio::io_service& io_service);

    // Add to lock-free queque, or send if curretly not processing
    void AsyncCommand(CommandBase* cmd);
  
private:
    void HandleSend(CommandBase* cmd);
    void HandleReceive(ReceiveBuffer&, std::size_t bytes_recvd);
    
private:
    boost::asio::io_service& _ioservice;
    boost::asio::io_service::strand _strand;
    boost::asio::ip::tcp::socket _socket;
    // Free-lock buffer commands of to send
    FifoCommands _sendedcommands;
    Sender _sender;
    Receiver _reciever;
};

}

#endif