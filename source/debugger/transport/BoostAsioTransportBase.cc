#include "BoostAsioTransportBase.h"

using namespace boost::asio;
using namespace boost::system;
using namespace gbxdb;
using namespace gbxdb::interfaces;
using namespace gbxdb::protocol;
using namespace gbxcommons;
using namespace std;

namespace gbxdb::transport
{

BoostAsioTransportBase::BoostAsioTransportBase(std::string ip, std::string port)
    : _ip(ip)
    , _port(port)
{}

void BoostAsioTransportBase::ListenerLoop(std::function<void(std::shared_ptr<gbxdb::interfaces::DebugMessage>)> NotifyObservers)
{
    std::shared_ptr<DebugMessage> debugMessage;
    boost::system::error_code error;
    
    while(!_terminated)
    {
        if (_socket->available())
            ReceiveMessageBlocking(debugMessage, error);
        else
            continue;

        NotifyObservers(debugMessage);

        if (error == error::eof)
            break;
        else if (error)
            throw boost::system::system_error(error);
    }
}

void BoostAsioTransportBase::ReceiveMessageBlocking(std::shared_ptr<DebugMessage>& debugMessage, boost::system::error_code& error)
{
    debugMessage = std::make_shared<DebugMessage>(std::make_shared<std::array<uint8_t, MaxMessageBufferSize>>());
    std::lock_guard<std::mutex> guard(_socketLock);
    _socket->read_some(buffer(*debugMessage->Buffer()), error);
}

void BoostAsioTransportBase::Terminate()
{
    if (_socket != nullptr && _socket->is_open())
        _socket->close();

    if (_thread != nullptr && _thread->joinable())
        _thread->join();
}

void BoostAsioTransportBase::InitializeAliveLine()
{
    ip::tcp::endpoint endPoint(ConvertIpAddress(), stoi(_port) + 1);
    io_service ios;

    ip::tcp::acceptor acceptor(ios, endPoint.protocol());
    acceptor.bind(endPoint);
    acceptor.listen(MaxNumberOfConnections);    
}

boost::asio::ip::address BoostAsioTransportBase::ConvertIpAddress()
{
    boost::system::error_code ec;
    auto ip = ip::address::from_string(_ip, ec);

    if (ec.value() != 0) 
    {
        stringstream ss;
        ss << "Failed to parse the IP address. Error code = " << ec.value() << ". Message: " << ec.message();
        throw ProtocolException(ss.str());
    }

    return ip;
}

}
