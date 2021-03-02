#include "BoostAsioServerProtocol.h"

using namespace boost::asio;
using namespace boost::system;
using namespace std;

namespace gbx
{

void BoostAsioServerProtocol::Initialize(shared_ptr<interfaces::ServerProtocolParameters> arguments)
{
    auto asioParameters = dynamic_pointer_cast<BoostAsioServerProtocolParameters>(arguments);

    if (asioParameters == nullptr)
        throw ServerProtocolException("Invalid server protocol type");

    _ip = asioParameters->IP();
    _port = asioParameters->Port();
    _verbose = asioParameters->Verbose();
}

void BoostAsioServerProtocol::WaitForClient()
{
    _thread = make_unique<thread>([&](){ this->RunProtocol(); });
}

boost::asio::ip::address BoostAsioServerProtocol::ConvertIpAddress()
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

void BoostAsioServerProtocol::RunProtocol()
{
    try 
    {
        AcceptConnection();
        ProtocolLoop();
    }
    catch (boost::system::system_error& e) 
    {
        stringstream ss;
        ss << "Boost ASIO error: " << e.code() << " Message: " << e.what();
        throw ServerProtocolException(ss.str());
    }
}

void BoostAsioServerProtocol::ProtocolLoop()
{
    while(true)
    {
        boost::array<char, 128> buf;
        boost::system::error_code error;
        size_t len = _socket->read_some(buffer(buf), error);

        //cout << buf.data() << '\n';
        if (error == error::eof)
        {
            Log("Client disconnected");
            break;
        }
        else if (error)
            throw boost::system::system_error(error);
    }
}

void BoostAsioServerProtocol::Log(std::string message)
{
    if (_verbose)
        cout << message << '\n';
}

void BoostAsioServerProtocol::AcceptConnection()
{
    ip::tcp::endpoint endPoint(ConvertIpAddress(), _port);
    io_service ios;

    ip::tcp::acceptor acceptor(ios, endPoint.protocol());
    acceptor.bind(endPoint);
    acceptor.listen(1);
    
    Log("Waiting for client to join...");
    _socket = make_unique<boost::asio::ip::tcp::socket>(ios);
    acceptor.accept(*_socket);
    Log("Connection established!");
}

void BoostAsioServerProtocol::AddEventListener(shared_ptr<gbxcommons::Observer> oberver)
{
    _observer.push_back(oberver);
}

void BoostAsioServerProtocol::Send(shared_ptr<interfaces::DebugMessage>)
{

}

}