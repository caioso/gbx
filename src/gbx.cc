#include <iostream>
#include <sstream>

#include "Version.h"
#include "Logger.h"
#include "GameBoyX.h"

using namespace std;
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

int main ()
{
    stringstream message;
    message << "GAME BOY X Emulator " << SystemVersion();
    Log::LogLn(message.str().c_str());

    auto gbx = make_unique<GbxDecorator>();

    int cycleCounter = 0;
    while (cycleCounter < 100)
    {
        gbx->Run();
        cycleCounter++;
    }
}