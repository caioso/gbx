#include <boost/asio.hpp>
#include <iostream>
#include <sstream>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

int main ()
{
    boost::asio::io_service io_service;
    cout << "GAME BOY X Debugger" << '\n' ;

    tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 1234 ));
    tcp::socket socket_(io_service);
    acceptor_.accept(socket_);

    cout << "Connection Accepted" << '\n';
}