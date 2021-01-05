#include "Z80X.h"

using namespace std;
using namespace gbx::interfaces;

namespace gbx
{

void Z80X::Initialize(std::shared_ptr<ControlUnitInterface> controlUnit, 
                      std::shared_ptr<interfaces::ClockInterface> clock, 
                      std::shared_ptr<interfaces::ArithmeticLogicUnitInterface> alu,
                      std::shared_ptr<interfaces::MemoryControllerInterface> memoryController)
{
    _controlUnit = controlUnit;
    _clock = clock;
    _alu = alu;
    _memoryController = memoryController;

    _controlUnit->Initialize(_memoryController, _alu);
}

void Z80X::Run()
{
    _controlUnit->RunCycle();
}

}