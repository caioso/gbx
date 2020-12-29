#include "ControlUnit.h"

using namespace std;

namespace gbx
{

ControlUnit::ControlUnit()
    : ControlUnitALUChannel(make_shared<Channel<ALUMessage>>(ChannelType::InOut))
    , _clock(make_unique<ClockSource>(EngineParameters::GBCClockPeriod))
    , _state(ControlUnitState::Fetch)
{
    ControlUnitALUChannel->OnReceived([this](ALUMessage message) -> void { this->OnALUMessage(message); });
}

void ControlUnit::Initialize()
{}

void ControlUnit::RunInstructionCycle()
{
    switch (_state)
    {
        case ControlUnitState::Fetch: Fetch(); break;
        case ControlUnitState::Decode: Decode(); break;
        case ControlUnitState::Execute: Execute(); break;
        case ControlUnitState::WriteBack: WriteBack(); break;
        case ControlUnitState::Acquire: Acquire(); break;
        case ControlUnitState::Wait: Wait(); break;
        case ControlUnitState::Interrupt: break;
    }

    // Adjust Clock Wait time.
    // This calculates an Approximate timing for each instruction.
    // _clock.Tick(#CyclesOfCurrentInstruction, approximate cycle execition time);
    // Only Run this when a cycle is complete!!!!
    _clock->Tick(4, 0);
}

void ControlUnit::OnALUMessage(ALUMessage message)
{
    switch(message)
    {
        case ALUMessage::ReadyToDecode: _state = ControlUnitState::Decode; break;
        case ALUMessage::ReadyToExecute: _state = ControlUnitState::Execute; break;
        case ALUMessage::ReadyToWriteBack: _state = ControlUnitState::WriteBack; break;
        case ALUMessage::ReadyToFetch: _state = ControlUnitState::Fetch; break;
        case ALUMessage::ReadyToAcquire: _state = ControlUnitState::Acquire; break;
        default: return;
    }
}

inline void ControlUnit::Fetch()
{
    ControlUnitALUChannel->Send(ALUMessage::Fetch);
}

inline void ControlUnit::Decode()
{
    ControlUnitALUChannel->Send(ALUMessage::Decode);
}

inline void ControlUnit::Execute()
{
    ControlUnitALUChannel->Send(ALUMessage::Execute);
}

inline void ControlUnit::WriteBack()
{
    ControlUnitALUChannel->Send(ALUMessage::WriteBack);
}

inline void ControlUnit::Acquire()
{
    ControlUnitALUChannel->Send(ALUMessage::Acquire);
}

inline void ControlUnit::Wait()
{}

inline void ControlUnit::DecideNextState()
{
    _state = ControlUnitState::Fetch;
}

}