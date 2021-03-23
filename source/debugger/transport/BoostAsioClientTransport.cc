#include "BoostAsioClientTransport.h"

using namespace boost;
using namespace boost::asio;
using namespace boost::system;
using namespace gbxcommons;
using namespace gbxdb::interfaces;
using namespace gbxdb::protocol;
using namespace std;

namespace gbxdb::transport
{

BoostAsioClientTransport::BoostAsioClientTransport(string ip, string port)
    : _ip(ip)
    , _port(port)
{}

BoostAsioClientTransport::~BoostAsioClientTransport()
{
    if (_thread != nullptr)
        _thread->join();
}

void BoostAsioClientTransport::JoinServer()
{
    _thread = make_unique<thread>([&](){ this->RunProtocol(); });
}

void BoostAsioClientTransport::LeaveServer()
{
    _terminated = true;

    if (_socket != nullptr && _socket->is_open())
        _socket->close();

    if (_thread != nullptr)
        _thread->join();
}

void BoostAsioClientTransport::RunProtocol()
{
    try
    {
        TryToJoinServer();
        ProtocolLoop();
        _socket->close();
    }
    catch (boost::system::system_error& e) 
    {
        if (_socket != nullptr && !_socket->is_open())
            _socket->close();

        stringstream ss;
        ss << "Boost ASIO error: " << e.code() << " Message: " << e.what();
        throw ProtocolException(ss.str());
    }
}

void BoostAsioClientTransport::TryToJoinServer()
{
    using namespace std::literals::chrono_literals;
    bool connected{};

    while (!connected && !_terminated)
    {
        try
        {
            asio::ip::tcp::endpoint ep(ConvertIpAddress(), stoi(_port));
            asio::io_service ios;

            _socket = make_unique<boost::asio::ip::tcp::socket>(ios, ep.protocol());

            cout << "Ready to connect to server..." << '\n';
            _socket->connect(ep);
            cout << "Connection established!" << '\n';
            connected = true;
        }
        catch (boost::system::system_error& e) 
        {
            cout << "Unable to join server" << '\n';
            cout << "Retrying in 3 seconds..." << '\n';
            std::this_thread::sleep_for(3s);
        }
    }
}

void BoostAsioClientTransport::ProtocolLoop()
{
    std::shared_ptr<DebugMessage> debugMessage;
    while(!_terminated)
    {
        
        boost::system::error_code error;
        auto len = 0llu;
        if (_socket->available())
        {
            debugMessage = std::make_shared<DebugMessage>(std::make_shared<std::array<uint8_t, MaxMessageBufferSize>>());
            // Use lock guard
            _socketLock.lock();
                len = _socket->read_some(buffer(*debugMessage->Buffer()), error);
            _socketLock.unlock();
        }
        else
        {
            std::this_thread::sleep_for(400ms);
            continue;
        }

        cout << "Received: " << len << '\n';
        cout << (*debugMessage->Buffer()).data() << '\n';

        NotifyObservers(debugMessage);

        if (error == error::eof)
        {
            cout << ("Client disconnected") << '\n';
            break;
        }
        else if (error)
            throw boost::system::system_error(error);
    }
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
    cout << "Message Received. Ready to disparch" << '\n';
    cout << "Send Message" << '\n';
    std::lock_guard guard(_socketLock);
        _socket->write_some(boost::asio::buffer((*message->Buffer())));
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

boost::asio::ip::address BoostAsioClientTransport::ConvertIpAddress()
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