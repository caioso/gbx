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
    _mainChannelThread = make_unique<thread>([&](){ this->RunProtocol(); });
}

void BoostAsioServerTransport::RunProtocol()
{
    try
    {
        AcceptConnection();

        _statusChannelThread = make_unique<thread>([&]() 
        {
            this->InitializeAliveSocket(); 
            this->ServerAliveLoop(); 
        });  

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
    
    _socket = make_unique<ip::tcp::socket>(ios);
    acceptor.accept(*_socket);

    // FIX THIS LATER!!!
    // Send 'Client joined message'
    auto debugMessage = make_shared<DebugMessage>(make_shared<array<uint8_t, MaxMessageBufferSize>>());
    (*debugMessage->Buffer())[0] = 0xFF;
    (*debugMessage->Buffer())[1] = 0xFF;
    NotifyObservers(debugMessage);

    cout << "Connection established!" << '\n';
}

void BoostAsioServerTransport::InitializeAliveSocket()
{
    uint8_t attempts = 0xFF;
    _alivePort = stoi(_port) + 1;

    while (true)
    {
        try
        {
            ip::tcp::endpoint endPoint(ConvertIpAddress(), _alivePort);
            io_service ios;

            ip::tcp::acceptor acceptor(ios, endPoint.protocol());
            acceptor.bind(endPoint);
            acceptor.listen(MaxNumberOfConnections);


            // FIX THIS LATER!!!
            // Ask client to join to join to the 'Alive socked'
            auto debugMessage = make_shared<DebugMessage>(make_shared<array<uint8_t, MaxMessageBufferSize>>());
            (*debugMessage->Buffer())[0] = MessageID::MessageProtocolInitializer & 0xFF;
            (*debugMessage->Buffer())[1] = (MessageID::MessageProtocolInitializer >> 0x08) & 0xFF;
            // Port number
            (*debugMessage->Buffer())[2] = (_alivePort) & 0xFF;
            (*debugMessage->Buffer())[3] = (_alivePort >> 0x08) & 0xFF;
            (*debugMessage->Buffer())[4] = (_alivePort >> 0x10) & 0xFF;
            (*debugMessage->Buffer())[5] = (_alivePort >> 0x18) & 0xFF;

            NotifyObservers(debugMessage);

            cout << "Server alive port:'" << _alivePort << "'\n";
            _aliveSocket = make_unique<ip::tcp::socket>(ios);
            acceptor.accept(*_aliveSocket);
            break;
        }
        catch (boost::system::system_error& e) 
        {
            cout << "Unable to open port '" << _alivePort << "'\n";
            _alivePort++; 
            attempts--;

            if (attempts == 0)
                throw ProtocolException("Unable to initialize debug server: unable to open status socket");
        }
    }
}

void BoostAsioServerTransport::SendMessage(shared_ptr<DebugMessage> message)
{
    std::lock_guard<std::recursive_mutex> guard(_socketLock);
    _socket->write_some(buffer((*message->Buffer())));
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

void BoostAsioServerTransport::InitializeProtocol()
{}

}