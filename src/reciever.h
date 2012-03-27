#ifndef __REDIS4CPP_RECIEVER_H__
#define __REDIS4CPP_RECIEVER_H__

#include "interface.h"

namespace redis4cpp {

class Receiver
{
friend class DataAccess;

public:
    typedef boost::function<void(ReceiveBuffer&, std::size_t bytes_recvd)> ReplyReceived;
    Receiver(boost::asio::io_service& io_service
            ,boost::asio::ip::tcp::socket& socket
            ,ReplyReceived received
            ,boost::asio::io_service::strand& strand);

private:
    void DoReceieve();
    void HandleReceive(const boost::system::error_code& error, std::size_t bytes_recvd);
    
private:
    boost::asio::ip::tcp::socket& _socket;
    // Strand to ensure the connection's handlers are not called concurrently.
    boost::asio::io_service::strand& _strand;
    ReceiveBuffer _buffer;
    ReplyReceived ReceivedCallback;
};

}

#endif