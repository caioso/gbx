#include <iostream>
#include <chrono>
#include <limits>
#include <string>
#include <sstream>
#include <thread>

#include "FileLoader.h"
#include "Runner.h"
#include "ArgumentsParser.h"
#include "ApplicationOptions.h"
#include "GameBoyX.h"
#include "GBXEmulatorExceptions.h"
#include "GBXCommonsExceptions.h"

using namespace std;
using namespace gbxruntime::runner;
using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace gbxcommons;

struct ApplicationConfiguration
{
    bool Verbose;
    string BIOSName;
    string ROMName;
};

ApplicationConfiguration configuration{};

ApplicationConfiguration ParseCommandLine(int argc, char** argv)
{
    auto parser = make_shared<ArgumentsParser>("gbx -r <ROM> [-d/--debug -i/--ip <ip> -p/--port <port> | -v/--verbose]");
    parser->RegisterOption("-r", "--rom", "Target ROM to load", OptionType::Pair, OptionRequirement::Required);
    parser->RegisterOption("-b", "--bios", "Target BIOS to load", OptionType::Pair, OptionRequirement::Required);

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
        
        if (parser->HasBeenFound("-r"))
            configuration.ROMName = parser->RetrieveOption("-r").Value.value();
        
        if (parser->HasBeenFound("-b"))
            configuration.BIOSName = parser->RetrieveOption("-b").Value.value();

        return configuration;
    }
    catch(const GBXCommonsException& e)
    {
        cout << e.what() << '\n';
        cout << parser->Help() << '\n';
        exit(2);
    }
}

void Log(string message)
{
    if (configuration.Verbose)
        cout << message << '\n';
}

void InitializeDebugServer()
{}

void DebugMode()
{
    Log("Debug Mode");
    InitializeDebugServer();
    Log("Execution Complete");
}

void RuntimeMode(ApplicationConfiguration configuration)
{
    auto gbx = make_unique<GameBoyX>();
    auto cycleCounter = 0llu;

    gbx->LoadBIOS(configuration.BIOSName);
    cout << "System BIOS: " << configuration.BIOSName << '\n';
    gbx->LoadGame(configuration.ROMName);
    cout << "User ROM: " << configuration.ROMName << '\n';

    while (cycleCounter < std::numeric_limits<size_t>::max())
    {
        gbx->Run();
        cycleCounter++;
    }
}

void LaunchEmulator(ApplicationConfiguration configuration)
{
    RuntimeMode(configuration);
}

int main (int argc, char** argv)
{
    cout << "GAME BOY X Runtime" << '\n';
    configuration = ParseCommandLine(argc, argv);
    
    LaunchEmulator(configuration);
}