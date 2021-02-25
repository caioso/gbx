#include <boost/asio.hpp>
#include <iostream>
#include <sstream>

using namespace boost;
using std::string;
using std::cout;
using std::cin;
using std::endl;

int main ()
{
    boost::asio::io_service io_service;
    cout << "GAME BOY X Debugger" << '\n' ;

    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 5678;

    try 
    {
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
        asio::io_service ios;

        asio::ip::tcp::socket sock(ios, ep.protocol());


        cout << "Ready to connect to server..." << '\n';
        sock.connect(ep);
        cout << "Connection established!" << '\n';

        for (;;)
        {
            std::string message;
            cin >> message;

            cout << "message to send: " << message << '\n'; 

            boost::system::error_code ignored_error;
            boost::asio::write(sock, boost::asio::buffer(message), ignored_error);
        }
    }
    catch (system::system_error &e) 
    {
        std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();
    }

}