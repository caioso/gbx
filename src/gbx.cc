#include <string>
#include <sstream>
#include <iostream>

#include "ApplicationOptions.h"
#include "ArgumentsParser.h"
#include "GameBoyX.h"
#include "GBXExceptions.h"

using namespace std;
using namespace gbx;
using namespace gbxcore::interfaces;
using namespace gbxcore;

// Test Only
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

ApplicationConfiguration configuration;

ApplicationConfiguration ParseCommandLine(int argc, char** argv)
{
    auto argsParser = make_shared<ArgumentsParser>();
    try
    {
        argsParser->Parse(argv, argc);
        return argsParser->Configuration();
    }
    catch(const GBXException& e)
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

void DebugMode()
{
    Log("Debug Mode");
    auto gbx = make_unique<GbxDecorator>();
    auto cycleCounter = 0;

    while (cycleCounter < 100)
    {
        gbx->Run();
        cycleCounter++;
    }
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