#include "ControlUnit.h"

using namespace std;

namespace gbx
{

ControlUnit::ControlUnit()
    : ControlUnitALUChannel(make_shared<Channel<ALUMessage>>(ChannelType::InOut))
    , _state(ControlUnitState::Fetch)
    , _fetchSubstate(FetchSubState::FetchT1)
{
    ControlUnitALUChannel->OnReceived([this](ALUMessage message) -> void { this->OnALUMessage(message); });
}

void ControlUnit::OnALUMessage(ALUMessage message)
{
}

void ControlUnit::Update()
{
    switch (_state)
    {
        case ControlUnitState::Fetch: UpdateFetch(); break;
    }
}

void ControlUnit::UpdateFetch()
{
    switch (_fetchSubstate)
    {
        case FetchSubState::FetchT1: ControlUnitALUChannel->Send(ALUMessage::FetchPC);
                                     _fetchSubstate = FetchSubState::FetchT2; 
                                     break;
        case FetchSubState::FetchT2: _fetchSubstate = FetchSubState::FetchT3;
                                     break;
        case FetchSubState::FetchT3: _fetchSubstate = FetchSubState::FetchT4; 
                                     break;
        case FetchSubState::FetchT4: ControlUnitALUChannel->Send(ALUMessage::Decode);
                                     _fetchSubstate = FetchSubState::FetchT1; 
                                     DecideNextState();
                                     break;
    }
}

void ControlUnit::DecideNextState()
{
    _state = ControlUnitState::Fetch;
}

}