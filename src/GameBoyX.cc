#include "GameBoyX.h"

using namespace std;

namespace gbx
{

GameBoyX::GameBoyX()
{
    _cpu = make_shared<Z80X>();
    _controlUnit = make_shared<ControlUnit>();
    _memoryController = make_shared<MemoryController>();
    _systemROM = make_shared<ROM>(0x1000); // sample rom size
    _alu = make_shared<ArithmeticLogicUnit>();
    _clock = make_shared<Clock>(EngineParameters::GBCClockPeriod);

    // Initialize Memory Controller
    _memoryController->RegisterMemoryResource(_systemROM, AddressRange(0, 0x1000, RangeType::BeginInclusive));

    // Initialize Z80X CPU
    _cpu->Initialize(_controlUnit, _clock, _alu, _memoryController);
}

void GameBoyX::Run()
{
    _cpu->Run();
}

}