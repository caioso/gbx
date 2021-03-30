#include "BoostAsioServerTransport.h"

using namespace boost::asio;
using namespace boost::system;
using namespace gbxdb;
using namespace gbxdb::interfaces;
using namespace gbxdb::protocol;
using namespace gbxcommons;
using namespace std;

namespace gbxdb::transport
{

BoostAsioServerTransport::BoostAsioServerTransport(string ip, string port)
    : BoostAsioTransportBase(ip, port)
{}

BoostAsioServerTransport::~BoostAsioServerTransport()
{
    Terminate();
}

void BoostAsioServerTransport::WaitForClient()
{
    _thread = make_unique<thread>([&](){ this->RunProtocol(); });
}

void BoostAsioServerTransport::RunProtocol()
{
    try
    {
        AcceptConnection();
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

void BoostAsioServerTransport::ProtocolLoop()
{
    ListenerLoop([&](std::shared_ptr<DebugMessage> message)
    {
        NotifyObservers(message);
    });
}

void BoostAsioServerTransport::NotifyObservers(shared_ptr<DebugMessage> message)
{
    auto notificationArguments = make_shared<DebugMessageNotificationArguments>(message);
    auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);
    
    for (auto observer : _observers)
        if (!observer.expired())
            observer.lock()->Notify(argumentsPointer);
}

void BoostAsioServerTransport::AcceptConnection()
{
    ip::tcp::endpoint endPoint(ConvertIpAddress(), stoi(_port));
    io_service ios;

    ip::tcp::acceptor acceptor(ios, endPoint.protocol());
    acceptor.bind(endPoint);
    acceptor.listen(MaxNumberOfConnections);

    cout << "Waiting for client to join..." << '\n';
    
    _socketLock.lock();
        _socket = make_unique<boost::asio::ip::tcp::socket>(ios);
        acceptor.accept(*_socket);
    _socketLock.unlock();

    // FIX THIS LATER!!!
    // Send 'Client joined message'
    auto debugMessage = make_shared<DebugMessage>(make_shared<array<uint8_t, MaxMessageBufferSize>>());
    (*debugMessage->Buffer())[0] = 0xFF;
    (*debugMessage->Buffer())[1] = 0xFF;
    NotifyObservers(debugMessage);

    cout << "Connection established!" << '\n';
}

void BoostAsioServerTransport::SendMessage(shared_ptr<DebugMessage> message)
{
    std::lock_guard guard(_socketLock);
    _socket->write_some(boost::asio::buffer((*message->Buffer())));
}

void BoostAsioServerTransport::Subscribe(weak_ptr<Observer> obs)
{
    auto matcher = [&](weak_ptr<Observer> o) -> auto 
    { 
        if (o.lock().get() == obs.lock().get()) 
            return true;  
        return false;
    
    }; 

    if (find_if(_observers.begin(), _observers.end(), matcher) == _observers.end())
        _observers.push_back(obs);
}

void BoostAsioServerTransport::Unsubscribe(weak_ptr<Observer> obs)
{
    auto matcher = [&](weak_ptr<Observer> o) -> auto 
    { 
        if (o.lock().get() == obs.lock().get()) 
            return true;  
        return false;
    
    }; 

    auto location = find_if(_observers.begin(), _observers.end(), matcher);

    if (find_if(_observers.begin(), _observers.end(), matcher) != _observers.end())
        _observers.erase(location);
}

}