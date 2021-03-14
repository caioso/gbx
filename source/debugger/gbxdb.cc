#include <boost/asio.hpp>
#include <array>
#include <iostream>
#include <sstream>
#include <memory>
#include <thread>

using namespace boost;
using namespace boost::asio;
using namespace boost::system;
using namespace std;
using std::string;
using std::array;
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

        auto _thread = make_unique<thread>([&]()
        { 
            while(true)
            {
                std::array<uint8_t, 256> debugMessage;
                boost::system::error_code error;
                size_t len = sock.read_some(boost::asio::buffer(debugMessage), error);

                cout << "Received: " << len << '\n';
                cout << hex << static_cast<size_t>(debugMessage[0]) << static_cast<size_t>(debugMessage[1]) << dec << '\n';

                if (debugMessage[0] == 0xFD && debugMessage[1] == 0xFF)
                {
                    uint16_t registerB = debugMessage[2];
                    uint16_t registerC = debugMessage[3];
                    uint16_t registerD = debugMessage[4];
                    uint16_t registerE = debugMessage[5];
                    uint16_t registerH = debugMessage[6];
                    uint16_t registerL = debugMessage[7];
                    uint16_t registerA = debugMessage[8];
                    uint16_t registerF = debugMessage[9];
                    uint16_t registerIR = debugMessage[10];
                    uint16_t registerPIR= debugMessage[11];
                    uint16_t registerPC = debugMessage[12] | debugMessage[13] << 0x08;
                    uint16_t registerSP = debugMessage[14] | debugMessage[15] << 0x08;

                    cout << "Register::B = " << registerB << '\n';
                    cout << "Register::C = " << registerC << '\n';
                    cout << "Register::D = " << registerD << '\n';
                    cout << "Register::E = " << registerE << '\n';
                    cout << "Register::H = " << registerH << '\n';
                    cout << "Register::L = " << registerL << '\n';
                    cout << "Register::A = " << registerA << '\n';
                    cout << "Register::F = " << registerF << '\n';
                    cout << "Register::IR = " << registerIR << '\n';
                    cout << "Register::PIR = " << registerPIR << '\n';
                    cout << "Register::PC = " << registerPC << '\n';
                    cout << "Register::SP = " << registerSP << '\n';
                }

                if (error == error::eof)
                {
                    cout << ("Client disconnected") << '\n';
                    break;
                }
                else if (error)
                    throw boost::system::system_error(error);
            }
            
        });

        for (;;)
        {
            std::string message;
            cin >> message;

            if (message.compare("register-bank") == 0)
            {
                std::array<uint8_t, 256> debugMessage;
                debugMessage[0] = 0xFD;
                debugMessage[1] = 0xFF;
                boost::system::error_code ignored_error;
                boost::asio::write(sock, boost::asio::buffer(debugMessage), ignored_error);
            }
            else if (message.compare("read-register") == 0)
            {
                std::array<uint8_t, 256> debugMessage;
                debugMessage[0] = 0xFE;
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