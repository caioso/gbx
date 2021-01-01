#include "ControlUnit.h"

using namespace std;

namespace gbx
{

ControlUnit::ControlUnit()
    : _clock(make_unique<ClockSource>(EngineParameters::GBCClockPeriod))
    , _state(ControlUnitState::Fetch)
{
}

void ControlUnit::Initialize(std::shared_ptr<ArithmeticLogicUnitInterface> alu)
{
    _alu = alu;
}

void ControlUnit::RunInstructionCycle()
{
    _alu->RunCycle();
    _clock->Tick(4, 0);
}

}