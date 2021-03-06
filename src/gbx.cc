#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include <sstream>

#include "ArgumentsParser.h"
#include "ApplicationOptions.h"
#include "BoostAsioServerProtocol.h"
#include "BoostAsioServerProtocolParameters.h"
#include "GameBoyX.h"
#include "GBXExceptions.h"
#include "GBXCommonsExceptions.h"

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

ApplicationConfiguration configuration{};

ApplicationConfiguration ParseCommandLine(int argc, char** argv)
{
    auto parser = make_shared<ArgumentsParser>("gbx [-d/--debug -i/--ip <ip> -p/--port <port> | -v/--verbose]");
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

            if (parser->HasBeenFound("-i") && parser->HasBeenFound("-p"))
            {
                configuration.IPAddress = parser->RetrieveOption("-i").Value.value();
                configuration.Port = parser->RetrieveOption("-p").Value.value();
            }
            else
            {
                cout << parser->Help() << '\n';
                exit(0);
            }
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
    auto gbx = make_shared<GameBoyX>();
    //auto protocol = make_shared<BoostAsioServerProtocol>();
    //auto protocolParameters = make_shared<BoostAsioServerProtocolParameters>(configuration.IPAddress, stoi(configuration.Port), configuration.Verbose);
    //auto debugServer = make_shared<DebugServer>(gbx, static_pointer_cast<gbx::interfaces::ServerProtocol>(protocol));
         
    //debugServer->Initialize(protocolParameters);
    //debugServer->WaitForClient();
    //debugServer->Run();
}

void DebugMode()
{
    Log("Debug Mode");
    InitializeDebugServer();
}

void RuntimeMode()
{
    Log("Runtime Mode");
    auto gbx = make_unique<GameBoyX>();
    auto cycleCounter = 0;

    while (cycleCounter < 100)
    {
        gbx->Run();
        cycleCounter++;
    }

    Log("Execution Complete");
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