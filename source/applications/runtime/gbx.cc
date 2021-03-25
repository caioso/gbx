#include <iostream>
#include <chrono>
#include <limits>
#include <string>
#include <thread>
#include <sstream>

#include "Runner.h"
#include "ArgumentsParser.h"
#include "ApplicationOptions.h"
#include "GameBoyX.h"
#include "GBXEmulatorExceptions.h"
#include "GBXCommonsExceptions.h"

#include "transport/BoostAsioServerTransport.h"

using namespace std;
using namespace gbx;
using namespace gbxdb::transport;
using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace gbxcommons;

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
                exit(1);
            }
        }

        return configuration;
    }
    catch(const GBXCommonsException& e)
    {
        cout << e.what() << '\n';
        exit(2);
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
    auto transport = make_unique<BoostAsioServerTransport>(configuration.IPAddress, configuration.Port);
    runtime::CancellationToken token;

    auto runner = make_shared<runtime::Runner>(gbx, std::move(transport));
    runner->Run(token);
}

void DebugMode()
{
    Log("Debug Mode");
    InitializeDebugServer();
    Log("Execution Complete");
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
    cout << "GAME BOY X Runtime" << '\n';
    configuration = ParseCommandLine(argc, argv);
    
    LaunchEmulator(configuration);
}