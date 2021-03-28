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
        InitializeStatusThread();
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
    _socket = make_unique<ip::tcp::socket>(ios);
    acceptor.accept(*_socket);

    ClientJoinedCommand command;
    NotifyObservers(command.EncodeCommandMessage());
}

void BoostAsioServerTransport::InitializeStatusThread()
{
    _statusChannelThread = make_unique<thread>([&]() 
    {
        this->InitializeStatusSocket(); 
        this->ServerStatusLoop(); 
    });  
}

void BoostAsioServerTransport::InitializeStatusSocket()
{
    auto portFound = false;
    auto attempts = static_cast<uint8_t>(NumberOfStatusPortDefinitionRetries);
    _statusPort = ChooseStatusPort(stoi(_port));

    while (!portFound)
    {
        try
        {
            ip::tcp::endpoint endPoint(ConvertIpAddress(), _statusPort);
            io_service ios;

            ip::tcp::acceptor acceptor(ios, endPoint.protocol());
            acceptor.bind(endPoint);
            acceptor.listen(MaxNumberOfConnections);

            auto debugMessage = make_shared<DebugMessage>(GenerateInitializationMessage());
            NotifyObservers(debugMessage);

            _statusSocket = make_unique<ip::tcp::socket>(ios);
            acceptor.accept(*_statusSocket);
            portFound = true;
        }
        catch (boost::system::system_error& e) 
        {
            ChooseStatusPort(_statusPort);
            attempts--;

            if (attempts == 0)
                throw ProtocolException("Unable to initialize debug server: unable to open status socket");
        }
    }
}

std::shared_ptr<std::array<uint8_t, MaxMessageBufferSize>> BoostAsioServerTransport::GenerateInitializationMessage()
{
    auto messageBody = make_shared<array<uint8_t, MaxMessageBufferSize>>();
    (*messageBody)[0] = MessageID::MessageProtocolInitializer & 0xFF;
    (*messageBody)[1] = (MessageID::MessageProtocolInitializer >> 0x08) & 0xFF;
    (*messageBody)[2] = (_statusPort) & 0xFF;
    (*messageBody)[3] = (_statusPort >> 0x08) & 0xFF;
    (*messageBody)[4] = (_statusPort >> 0x10) & 0xFF;
    (*messageBody)[5] = (_statusPort >> 0x18) & 0xFF;

    return messageBody;
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

void BoostAsioServerTransport::ServerStatusLoop()
{
    uint8_t currentMessage = 0x00;
    bool pending = false;

    boost::system::error_code error;
    std::array<uint8_t, 1> aliveMessageBuffer;

    while(!_terminated)
    {
        if (!pending)
        {
            aliveMessageBuffer[0] = currentMessage++;

            // Lock Scope
            { 
                std::lock_guard<std::recursive_mutex> guard(_statusSocketLock);
                _statusSocket->write_some(buffer(aliveMessageBuffer));
            }
        }

        this_thread::sleep_for(std::chrono::milliseconds(ServerStatusPingIntervalInMilliseconds));
        
        // Lock Scope
        { 
            std::lock_guard<std::recursive_mutex> guard(_statusSocketLock);
            if (_statusSocket->available())
            {
                pending = false;
                _statusSocket->read_some(buffer(aliveMessageBuffer), error);

                if (error == error::eof)
                    break;
                else if (error)
                    throw boost::system::system_error(error);
            }    
            else
                pending = true;
        }
    }
}

int BoostAsioServerTransport::ChooseStatusPort(int base)
{
    return base + 1;
}

void BoostAsioServerTransport::InitializeProtocol()
{}

}