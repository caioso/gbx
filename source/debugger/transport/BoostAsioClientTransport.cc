#include "BoostAsioClientTransport.h"

using namespace boost;
using namespace boost::asio;
using namespace boost::system;
using namespace gbxcommons;
using namespace gbxdb::algorithms;
using namespace gbxdb::interfaces;
using namespace gbxdb::protocol;
using namespace std;

namespace gbxdb::transport
{

BoostAsioClientTransport::BoostAsioClientTransport(string ip, string port)
    : BoostAsioTransportBase(ip, port)
{}

BoostAsioClientTransport::~BoostAsioClientTransport()
{
    Terminate();
}

void BoostAsioClientTransport::JoinServer()
{
    _mainChannelThread = make_unique<thread>([&](){ this->RunProtocol(); });
}

void BoostAsioClientTransport::LeaveServer()
{
    _terminated = true;
    Terminate();
}

void BoostAsioClientTransport::RunProtocol()
{
    try
    {
        TryToJoinServer();
        ProtocolLoop();
        Terminate();
    }
    catch (boost::system::system_error& e) 
    {
        Terminate();

        stringstream ss;
        ss << "Boost ASIO error: " << e.code() << " Message: " << e.what();
        throw ProtocolException(ss.str());
    }
}

void BoostAsioClientTransport::TryToJoinServer()
{
    using namespace std::literals::chrono_literals;
    bool connected{};

    while (connected != true && _terminated != true)
    {
        try
        {
            asio::ip::tcp::endpoint ep(ConvertIpAddress(), stoi(_port));
            asio::io_service ios;

            _socket = make_unique<boost::asio::ip::tcp::socket>(ios, ep.protocol());
            _socket->connect(ep);
            connected = true;
        }
        catch (boost::system::system_error& e) 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(SocketRetryIntervalInMilliseconds));
        }
    }
}

void BoostAsioClientTransport::ProtocolLoop()
{
    ListenerLoop([&](std::shared_ptr<DebugMessage> message)
    {
        NotifyObservers(message);
    });
}

void BoostAsioClientTransport::NotifyObservers(std::shared_ptr<DebugMessage> message)
{
    auto notificationArguments = std::make_shared<DebugMessageNotificationArguments>(message);
    auto argumentsPointer = std::static_pointer_cast<NotificationArguments>(notificationArguments);
    
    for (auto observer : _observers)
        if (!observer.expired())
            observer.lock()->Notify(argumentsPointer);
}

void BoostAsioClientTransport::SendMessage(std::shared_ptr<DebugMessage> message)
{
    std::lock_guard<std::recursive_mutex> guard(_socketLock);
    _socket->write_some(buffer((*message->Buffer())));
}

void BoostAsioClientTransport::Subscribe(std::weak_ptr<Observer> obs)
{
    auto matcher = [&](std::weak_ptr<Observer> o) -> auto 
    { 
        if (o.lock().get() == obs.lock().get()) 
            return true;  
        return false;
    
    }; 

    if (find_if(_observers.begin(), _observers.end(), matcher) == _observers.end())
        _observers.push_back(obs);
}

void BoostAsioClientTransport::Unsubscribe(std::weak_ptr<Observer> obs)
{
    auto matcher = [&](std::weak_ptr<Observer> o) -> auto 
    { 
        if (o.lock().get() == obs.lock().get()) 
            return true;  
        return false;
    
    }; 

    auto location = find_if(_observers.begin(), _observers.end(), matcher);

    if (find_if(_observers.begin(), _observers.end(), matcher) != _observers.end())
        _observers.erase(location);
}

void BoostAsioClientTransport::InitializeProtocol(std::shared_ptr<std::array<uint8_t, MaxMessageBufferSize>> message)
{
    auto statusSocketConnnected = false;
    ExtractStatusPort(message);

    while (statusSocketConnnected  != true &&_terminated != true)
    {
        try
        {
            asio::ip::tcp::endpoint ep(ConvertIpAddress(), _statusPort);
            asio::io_service ios;

            _statusSocket = make_unique<boost::asio::ip::tcp::socket>(ios, ep.protocol());
            _statusSocket->connect(ep);
            
            statusSocketConnnected  = true;
        }
        catch (boost::system::system_error& e) 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(SocketRetryIntervalInMilliseconds));
        }
    }

    InitializeClientAliveLine();
}

void BoostAsioClientTransport::InitializeClientAliveLine()
{
    _statusChannelThread = make_unique<thread>([&](){ this->ClientStatusLoop(); });    
}

void BoostAsioClientTransport::ClientStatusLoop()
{
    boost::system::error_code error;
    std::array<uint8_t, 1> aliveMessageBuffer;
    
    _connectionAvailability.EstablishConnection();

    while(!_terminated)
    {
        bool received{};

        // Lock scope
        {
            std::lock_guard<std::recursive_mutex> guard(_statusSocketLock);
            if (_statusSocket->available())
            {
                _statusSocket->read_some(buffer(aliveMessageBuffer), error);
                _statusSocket->write_some(buffer(aliveMessageBuffer));
                received = true;

                if (error == error::eof)
                    break;
                else if (error)
                    throw boost::system::system_error(error);
            }    
        }

        this_thread::sleep_for(std::chrono::milliseconds(ClientStatusPingPollingIntervalInMilliseconds));

        if (received)
            _connectionAvailability.Tick(BeaconState::BeaconReceived);
        else
            _connectionAvailability.Tick(BeaconState::NoBeaconReceived);

        cout << "Connection State: " << (_connectionAvailability.State() == ConnectionState::NotConnected? "Not Connected" : "Connected") << '\n';
    }
}

void BoostAsioClientTransport::ExtractStatusPort(std::shared_ptr<std::array<uint8_t, gbxdb::interfaces::MaxMessageBufferSize>> message)
{
    _statusPort = (*message)[2] | (*message)[3] << 0x08 | (*message)[4] << 0x10 << (*message)[5] << 0x18;
}

}