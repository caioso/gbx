#include <boost/asio.hpp>
#include <array>
#include <iostream>
#include <sstream>
#include <memory>

using namespace boost;
using namespace std;
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

            if (message.compare("a") == 0)
            {
                std::array<uint8_t, 256> debugMessage;
                debugMessage[0] = 0xFD;
                debugMessage[1] = 0xFF;
                boost::system::error_code ignored_error;
                boost::asio::write(sock, boost::asio::buffer(debugMessage), ignored_error);
            }
            else
            {
                cout << "message to send: " << message << '\n'; 

                if (message.compare("end") == 0)
                {
                    sock.close();
                }
                else
                {
                    boost::system::error_code ignored_error;
                    boost::asio::write(sock, boost::asio::buffer(message), ignored_error);
                }
            }
        }
    }
    catch (system::system_error &e) 
    {
        std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();
    }

}