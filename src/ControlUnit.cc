#include "ControlUnit.h"

using namespace std;

namespace gbx
{

ControlUnit::ControlUnit()
    : _state(ControlUnitState::Fetch)
    , _fetchSubstate(FetchSubState::FetchT1)
{}

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
        case FetchSubState::FetchT1: _fetchSubstate = FetchSubState::FetchT2; 
                                     cout << "\tExecuted Fetch F1 state" << '\n';
                                     break;
        case FetchSubState::FetchT2: _fetchSubstate = FetchSubState::FetchT3; break;
        case FetchSubState::FetchT3: _fetchSubstate = FetchSubState::FetchT4; break;
        case FetchSubState::FetchT4: _fetchSubstate = FetchSubState::FetchT1; 
                                     DecideNextState();
                                     break;
    }
}

void ControlUnit::DecideNextState()
{
    cout << "Current State: Fetch" << '\n';
    _state = ControlUnitState::Fetch;
}

}