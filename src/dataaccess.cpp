#include "dataaccess.h"
#include "replyparser.h"
#include <boost/lexical_cast.hpp>
#include <boost/memory_order.hpp>

namespace redis4cpp {
    
DataAccess::DataAccess(boost::asio::io_service& io_service)
    :_ioservice(io_service)
    ,_socket(io_service)
    ,_strand(io_service)
    ,_sender(_ioservice, _socket, 
                boost::bind(&DataAccess::HandleSend, this, _1), _strand)
    ,_reciever(_ioservice, _socket,
                boost::bind(&DataAccess::HandleReceive, this, _1, _2), _strand)
{
    using boost::asio::ip::tcp;
    
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(tcp::v4(), "127.0.0.1", "6379");
    tcp::resolver::iterator iterator = resolver.resolve(query);
    
    _socket.connect(iterator->endpoint());

    // Put the socket into non-blocking mode.
    tcp::socket::non_blocking_io non_blocking_io(true);
    _socket.io_control(non_blocking_io);
    
    _reciever.DoReceieve();
}

void DataAccess::AsyncCommand(CommandBase* cmd)
{
    _sender.AsyncCommand(cmd);
}

void DataAccess::HandleSend(CommandBase* cmd)
{
    // Add to sended commands queue, this command is next to respond result
    _sendedcommands.enqueue(cmd);
}

void DataAccess::HandleReceive(ReceiveBuffer& buffer, std::size_t bytesRecvd)
{
    std::string data;
    
    ReplyParser parser(buffer.data(), bytesRecvd);

    CommandBasePtr cmd;
    while(parser.NextReply(data) && _sendedcommands.dequeue(cmd))
    {
        cmd->SetResult(data);
        cmd->Completed();
        //_strand.dispatch(boost::bind(&CommandBase::Completed, cmd));
    }
}

}

