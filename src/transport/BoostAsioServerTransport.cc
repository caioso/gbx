#include "BoostAsioServerTransport.h"

using namespace boost::asio;
using namespace boost::system;
using namespace gbx;
using namespace gbx::interfaces;
using namespace gbx::runtime;
using namespace gbxcommons;
using namespace std;

namespace gbx::transport
{

BoostAsioServerTransport::BoostAsioServerTransport(string ip, string port)
    : _ip(ip)
    , _port(port)
{}

BoostAsioServerTransport::~BoostAsioServerTransport()
{
    if (_thread != nullptr)
        _thread->join();
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
        if (_socket != nullptr)
            _socket->close();

        stringstream ss;
        ss << "Boost ASIO error: " << e.code() << " Message: " << e.what();
        throw ServerProtocolException(ss.str());
    }
}

void BoostAsioServerTransport::ProtocolLoop()
{
    while(true)
    {
        auto debugMessage = make_shared<DebugMessage>(make_shared<array<uint8_t, MaxMessageBufferSize>>());
        boost::system::error_code error;
        size_t len = _socket->read_some(buffer(*debugMessage->Buffer()), error);

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
    
    _socket = make_unique<boost::asio::ip::tcp::socket>(ios);
    acceptor.accept(*_socket);

    // FIX THIS LATER!!!
    // Send 'Client joined message'
    auto debugMessage = make_shared<DebugMessage>(make_shared<array<uint8_t, MaxMessageBufferSize>>());
    (*debugMessage->Buffer())[0] = 0xFF;
    (*debugMessage->Buffer())[1] = 0xFF;
    NotifyObservers(debugMessage);

    cout << "Connection established!" << '\n';
}

boost::asio::ip::address BoostAsioServerTransport::ConvertIpAddress()
{
    boost::system::error_code ec;
    auto ip = ip::address::from_string(_ip, ec);

    if (ec.value() != 0) 
    {
        stringstream ss;
        ss << "Failed to parse the IP address. Error code = " << ec.value() << ". Message: " << ec.message();
        throw ServerProtocolException(ss.str());
    }

    return ip;
}

void BoostAsioServerTransport::SendMessage(shared_ptr<DebugMessage>)
{
    cout << "Send Message" << '\n';
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