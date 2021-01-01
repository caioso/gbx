#include "CPU.h"

using namespace std;

namespace gbx
{

CPU::CPU()
    : _controlUnit(make_shared<ControlUnit>())
    , _alu(make_shared<ArithmeticLogicUnit>())
    , _memoryController(make_shared<MemoryController>())

    // Memory Resources
    , _internalROM(make_shared<ROM>(0x100))
{
    _controlUnit->Initialize(_alu);
}

void CPU::Initialize()
{
    InitializeMemorySubsystem();
    InitializeReferences();
}

void CPU::Run()
{
    while (true)
    {
        UpdateEmulation();
    }
}

void CPU::Run(uint32_t instructions)
{
    auto counter = static_cast<uint32_t>(0);
    while (counter++ != instructions)
    {
        UpdateEmulation();
    }
}

inline void CPU::UpdateEmulation()
{
    _controlUnit->RunInstructionCycle();
}


inline void CPU::InitializeMemorySubsystem()
{
    // 0x100 will change later
    AddressRange range(0x0000, 0x0100, RangeType::BeginInclusive);
    _memoryController->RegisterMemoryResource(static_pointer_cast<Memory>(_internalROM), range);
}

inline void CPU::InitializeReferences()
{
    _alu->Initialize(_memoryController);
}

}