#include <boost/asio.hpp>
#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <memory>
#include <thread>

#include "ArgumentsParser.h"
#include "BoostAsioClientTransport.h"
#include "ClientMessageHandler.h"
#include "CommandLineParser.h"
#include "CommandLineOutputDriver.h"

/*using namespace boost;
using namespace boost::asio;
using namespace boost::system;
using std::string;
using std::array;
using std::cout;
using std::cin;
using std::endl;
*/

using namespace gbxcommons;
using namespace gbxdb;
using namespace gbxdb::input;
using namespace gbxdb::interfaces;
using namespace gbxdb::output;
using namespace gbxdb::protocol;
using namespace gbxdb::transport;
using namespace std;

struct ApplicationConfiguration
{
    bool Verbose{};
    string IPAddress{};
    string Port{};
};

ApplicationConfiguration ParseCommandLine(int argc, char** argv)
{
    ApplicationConfiguration configuration;
    ArgumentsParser parser("gbxdb_cl -i/--ip <ip> -p/--port <port> [-v/--verbose]");
    parser.RegisterOption("-i", "--ip", "GBX Debug Server IP Address", OptionType::Pair, OptionRequirement::Required);
    parser.RegisterOption("-p", "--port", "GBX Debug Server Port Number", OptionType::Pair, OptionRequirement::Required);
    parser.RegisterOption("-v", "--verbose", "Verbose mode", OptionType::Flag, OptionRequirement::Optional);

    try
    {
        parser.Parse(argv, argc);

        if (parser.HasBeenFound("-h"))
        {
            cout << parser.Help() << '\n';
            exit(0);
        }

        if (parser.HasBeenFound("-v"))
            configuration.Verbose = true;
        
        if (parser.HasBeenFound("-i") && parser.HasBeenFound("-p"))
        {
            configuration.IPAddress = parser.RetrieveOption("-i").Value.value();
            configuration.Port = parser.RetrieveOption("-p").Value.value();
        }
        else
        {
            cout << parser.Help() << '\n';
            exit(1);
        }

    }
    catch(const GBXCommonsException& e)
    {
        cout << e.what() << '\n';
        cout << parser.Help() << '\n';
        exit(2);
    }

    return configuration;
}

void RunDebugClient(ApplicationConfiguration configuration)
{
    CommandLineParser parser;
    CommandLineOutputDriver outputDriver;
    auto command = string("");
    auto transport = make_shared<BoostAsioClientTransport>(configuration.IPAddress, configuration.Port);
    auto transportPointer = static_pointer_cast<ClientTransport>(transport) ;
    auto messageHandler = make_shared<ClientMessageHandler>(transportPointer, outputDriver);

    messageHandler->Initialize();
    parser.Initialize();
    
    while (true)
    {
        cin >> command;
        if (command.compare("exit") == 0)
        {
            auto option = string("");

            while (option.compare("y") != 0 && option.compare("yes") != 0 && 
                   option.compare("n") != 0 && option.compare("no") != 0)
            {
                cout << "Terminate current debug session? [Y/n] ";
                cin >> option;
                std::for_each(option.begin(), option.end(), [](char & c) -> auto
                {
                    c = ::tolower(c);
                });

                if (option.size() == 0 || option.compare("y") == 0 || option.compare("yes") == 0 ||
                    option.compare("n") == 0 || option.compare("no") == 0)
                    break;

                cout << "Invalid option '" << option << "'" << '\n'; 
            }
                    
            if (option.size() == 0 || option.compare("y") == 0 || option.compare("yes") == 0)
            {
                cout << "Debug session terminated" << '\n';
                transport->LeaveServer();
                break;
            }
            
            if (option.compare("n") == 0 || option.compare("no") == 0)
                continue;
        }
        else
        {
            try
            {
                auto message = parser.Parse(command);
                transport->SendMessage(make_shared<DebugMessage>(message));
            }
            catch (const CommandLineInputException& exception)
            {
                cout << exception.what() << '\n';
            }
        }
    }
}

int main (int argc, char** argv)
{
    cout << "GAME BOY X Debugger" << '\n' ;
    auto config = ParseCommandLine(argc, argv);
    RunDebugClient(config);
}