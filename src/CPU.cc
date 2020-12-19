#include "CPU.h"

using namespace std;

namespace gbx
{

CPU::CPU()
    : _clock(make_unique<ClockSource>(EngineParameters::GBCFrequency))
    , _registerBank(make_unique<RegisterBank>())
    , _internalROM(make_unique<ROM>(0x100))
    , _controlUnit(make_unique<ControlUnit>())
{}

void CPU::Initialize()
{
    InitializeRegisterBank();
    InitializeClockSubsystem();
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

void CPU::InitializeRegisterBank()
{
    _registerBank->Write(Register::IR, 0x00);
    _registerBank->WritePair(Register::PC, 0x0000);
    _registerBank->Write(Register::F, 0x00);
}

}