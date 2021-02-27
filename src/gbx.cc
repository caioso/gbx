#include <boost/asio.hpp>
#include <boost/array.hpp>

#include <string>
#include <sstream>
#include <iostream>

#include "ArgumentsParser.h"
#include "ApplicationOptions.h"
#include "ArgumentsParser.h"
#include "GameBoyX.h"
#include "GBXExceptions.h"
#include "GBXCommonsExceptions.h"

using namespace boost;
using namespace std;
using namespace gbx;
using namespace gbxcore::interfaces;
using namespace gbxcore;
using namespace gbxcommons;

// Test Only
struct ApplicationConfiguration
{
    bool Verbose;
    bool IsDebug;
    string IPAddress;
    string Port;
};

class GbxDecorator : public GameBoyX
{
public:
    std::shared_ptr<Z80X> GetCPU() { return _cpu; }
    std::shared_ptr<ControlUnit> GetControlUnit() { return _controlUnit; }
    std::shared_ptr<MemoryController> GetMemoryController() { return _memoryController; }
    std::shared_ptr<ROM> GetROM() { return _userROM; }
    std::shared_ptr<RAM> GetVRAM() { return _videoRAM; }
    std::shared_ptr<RAM> GetERAM() { return _externalRAM; }
    std::shared_ptr<RAM> GetWRAM0() { return _workRAMBank0; }
    std::shared_ptr<RAM> GetWRAM1() { return _workRAMBank1; }
    std::shared_ptr<RAM> GetMRAM() { return _mirrorRAM; }
    std::shared_ptr<RAM> GetIORAM() { return _IORAM; }
    std::shared_ptr<RAM> GetHRAM() { return _HRAM; }
    std::shared_ptr<ArithmeticLogicUnit> GetALU() { return _alu; }
    std::shared_ptr<Clock> GetClock() { return _clock; }
};

ApplicationConfiguration configuration{};

ApplicationConfiguration ParseCommandLine(int argc, char** argv)
{
    auto parser = make_shared<ArgumentsParser>("gbx [-d/--debug -i/--ip <ip> -p/--port <port {-v}]");
    parser->RegisterOption("-d", "--debug", "Enable Debug Mode", OptionType::Flag, OptionRequirement::Optional);
    parser->RegisterOption("-i", "--ip", "IP Address", OptionType::Pair, OptionRequirement::Optional);
    parser->RegisterOption("-p", "--port", "Port Number", OptionType::Pair, OptionRequirement::Optional);
    parser->RegisterOption("-v", "--verbose", "Verbose mode", OptionType::Flag, OptionRequirement::Optional);

    try
    {
        parser->Parse(argv, argc);

        if (parser->HasBeenFound("-h"))
        {
            cout << parser->Help() << '\n';
            exit(0);
        }

        if (parser->HasBeenFound("-v"))
            configuration.Verbose = true;

        if (parser->HasBeenFound("-d"))
        {
            configuration.IsDebug = true;

            if (parser->HasBeenFound("-i"))
                configuration.IPAddress = parser->RetrieveOption("-i").Value.value();
            
            if (parser->HasBeenFound("-p"))
                configuration.Port = parser->RetrieveOption("-p").Value.value();
        }

        return configuration;
    }
    catch(const GBXCommonsException& e)
    {
        cout << e.what() << '\n';
        exit(1);
    }
}

void Log(string message)
{
    if (configuration.Verbose)
        cout << message << '\n';
}

void InitializeDebugServer()
{
    auto raw_ip_address = configuration.IPAddress;
    unsigned short port_num = stoi(configuration.Port);

    boost::system::error_code ec;
    asio::ip::address ip_address = asio::ip::address::from_string(raw_ip_address, ec);

    if (ec.value() != 0) {
        std::cout 
        << "Failed to parse the IP address. Error code = " << ec.value() << ". Message: " << ec.message();
    }

    asio::ip::tcp::endpoint ep(ip_address, port_num);
    asio::io_service ios;

    try 
    {
        asio::ip::tcp::acceptor acceptor(ios, ep.protocol());
        acceptor.bind(ep);
        acceptor.listen(1);
        
        cout << "Waiting for client to join..." << '\n';
        asio::ip::tcp::socket sock(ios);
        acceptor.accept(sock);
        cout << "Connection established!" << '\n';

        for (;;)
        {
            boost::array<char, 128> buf;
            boost::system::error_code error;

            size_t len = sock.read_some(boost::asio::buffer(buf), error);
            cout << "message received: " << buf.data() << '\n';

            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.
        }
    }
    catch (system::system_error &e) 
    {
        std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();
    }
}

void DebugMode()
{
    Log("Debug Mode");
    InitializeDebugServer();
    /*auto gbx = make_unique<GbxDecorator>();
    auto cycleCounter = 0;

    while (cycleCounter < 100)
    {
        gbx->Run();
        cycleCounter++;
    }*/
}

void RuntimeMode()
{
    Log("Runtime Mode");
    auto gbx = make_unique<GbxDecorator>();
    auto cycleCounter = 0;

    while (cycleCounter < 100)
    {
        gbx->Run();
        cycleCounter++;
    }
}

void LaunchEmulator(ApplicationConfiguration configuration)
{
    if (configuration.IsDebug)
        DebugMode();
    else
        RuntimeMode();
}

int main (int argc, char** argv)
{
    cout << "GAME BOY X Emulator 0.0.1-alpha" << '\n';
    configuration = ParseCommandLine(argc, argv);
    
    LaunchEmulator(configuration);
}