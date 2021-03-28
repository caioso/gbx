#include "BoostAsioTransportBase.h"

using namespace boost::asio;
using namespace boost::system;
using namespace gbxdb;
using namespace gbxdb::interfaces;
using namespace gbxdb::protocol;
using namespace gbxcommons;
using namespace std;
using namespace std::literals::chrono_literals;

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
        // Lock scope
        {
            std::lock_guard<std::recursive_mutex> guard(_socketLock);
            if (_socket->available())
            {
                ReceiveMessageBlocking(debugMessage, error);
                NotifyObservers(debugMessage);

                if (error == error::eof)
                    break;
                else if (error)
                    throw boost::system::system_error(error);
            }
        }

        this_thread::sleep_for(std::chrono::milliseconds(MainLoopWindDownIntervalInMilliseconds));
    }
}

void BoostAsioTransportBase::ReceiveMessageBlocking(std::shared_ptr<DebugMessage>& debugMessage, boost::system::error_code& error)
{
    debugMessage = std::make_shared<DebugMessage>(std::make_shared<std::array<uint8_t, MaxMessageBufferSize>>());
    _socket->read_some(buffer(*debugMessage->Buffer()), error);
    cout << hex << static_cast<size_t>((*debugMessage->Buffer())[0]) << " " << static_cast<size_t>((*debugMessage->Buffer())[1]) << '\n';
}

void BoostAsioTransportBase::Terminate()
{
    _terminated = true;
    if (_socket != nullptr && _socket->is_open())
        _socket->close();

    if (_mainChannelThread != nullptr && _mainChannelThread->joinable())
        _mainChannelThread->join();
    
    if (_statusChannelThread != nullptr && _statusChannelThread->joinable())
        _statusChannelThread->join();
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
