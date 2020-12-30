#include "CPU.h"

using namespace std;

namespace gbx
{

CPU::CPU()
    : _controlUnit(make_unique<ControlUnit>())
    , _alu(make_unique<ArithmeticLogicUnit>())
    , _memoryController(make_unique<MemoryController>())

    // Memory Resources
    , _internalROM(make_unique<ROM>(0x100))
{

    _controlUnit->ControlUnitALUChannel->Bind(_alu->ALUControlUnitChannel);
    _alu->ALUControlUnitChannel->Bind(_controlUnit->ControlUnitALUChannel);
    
    _controlUnit->Initialize();

    _memoryController->MemoryControllerALUChannel->Bind(_alu->ALUMemoryControllerChannel);
    _alu->ALUMemoryControllerChannel->Bind(_memoryController->MemoryControllerALUChannel);
}

void CPU::Initialize()
{
    InitializeMemorySubsystem();
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

}