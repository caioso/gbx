#include "BoostAsioClientTransport.h"

using namespace boost;
using namespace boost::asio;
using namespace boost::system;
using namespace gbxcommons;
using namespace gbxdb::interfaces;
using namespace std;

namespace gbxdb::transport
{

BoostAsioClientTransport::BoostAsioClientTransport(string ip, string port)
    : _ip(ip)
    , _port(port)
{}

void BoostAsioClientTransport::JoinServer()
{
    asio::ip::tcp::endpoint ep(ConvertIpAddress(), stoi(_port));
    asio::io_service ios;

    asio::ip::tcp::socket sock(ios, ep.protocol());

    cout << "Ready to connect to server..." << '\n';
    sock.connect(ep);
    cout << "Connection established!" << '\n';
}

void BoostAsioClientTransport::SendMessage(std::shared_ptr<DebugMessage>)
{}

void BoostAsioClientTransport::Subscribe(std::weak_ptr<Observer>)
{}

void BoostAsioClientTransport::Unsubscribe(std::weak_ptr<Observer>)
{}

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