#include "CPU.h"

using namespace std;

namespace gbx
{

CPU::CPU()
    : _clock(make_unique<ClockSource>(EngineParameters::GBCFrequency))
    , _controlUnit(make_unique<ControlUnit>())
    , _alu(make_unique<ArithmeticLogicUnit>())
    , _memoryController(make_unique<MemoryController>())

    // Memory Resources
    , _internalROM(make_unique<ROM>(0x100))
{

    _controlUnit->ControlUnitALUChannel->Bind(_alu->ALUControlUnitChannel);
    _memoryController->MemoryControllerALUChannel->Bind(_alu->ALUMemoryControllerChannel);
}

void CPU::Initialize()
{
    InitializeClockSubsystem();
    InitializeMemorySubsystem();
}

void CPU::Run()
{
    while (true)
    {
        UpdateEmulation();
    }
}

void CPU::Run(uint32_t cycles)
{
    auto counter = static_cast<uint32_t>(0);
    while (counter++ != cycles)
    {
        UpdateEmulation();
    }
}

void CPU::UpdateEmulation()
{
    _clock->Tick();
}

void CPU::OnTick()
{
    _controlUnit->Update();
}

void CPU::InitializeClockSubsystem()
{
    weak_ptr<ClockObserver> mock = shared_from_this();
    _clock->Subscribe(mock);
}

void CPU::InitializeMemorySubsystem()
{
    // 0x100 will change later
    AddressRange range(0x0000, 0x0100, RangeType::BeginInclusive);
    _memoryController->RegisterMemoryResource(static_pointer_cast<Memory>(_internalROM), range);
}

}