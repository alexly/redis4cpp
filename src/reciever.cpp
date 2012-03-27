#include "reciever.h"
#include <boost/lexical_cast.hpp>

const short multicast_port = 30001;

namespace redis4cpp {

Receiver::Receiver(boost::asio::io_service& io_service
        ,boost::asio::ip::tcp::socket& socket
        ,ReplyReceived received
        ,boost::asio::io_service::strand& strand)
    : _socket(socket)
    ,ReceivedCallback(received)
    ,_strand(strand)
{
}
  
void Receiver::HandleReceive(const boost::system::error_code& error, std::size_t bytesRecvd)
{
    if(!error)
    {
        _strand.dispatch(boost::bind(ReceivedCallback, _buffer, bytesRecvd));
        
        DoReceieve();
    }
    else {
        std::cout << "Receiver::HandleReceive error: " << error.message() << std::endl;
    }
}
  
void Receiver::DoReceieve()
{    
    _socket.async_read_some(
        boost::asio::buffer(_buffer),//"\r\n"
                    boost::bind(&Receiver::HandleReceive, this, 
                        boost::asio::placeholders::error, 
                        boost::asio::placeholders::bytes_transferred));
}

}

