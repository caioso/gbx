#include "Z80X.h"

using namespace std;
using namespace gbx::interfaces;

namespace gbx
{

void Z80X::Initialize(shared_ptr<ControlUnitInterface> controlUnit, 
                      shared_ptr<ClockInterface> clock, 
                      shared_ptr<ArithmeticLogicUnitInterface> alu,
                      shared_ptr<MemoryControllerInterface> memoryController,
                      shared_ptr<RegisterBankInterface> registers)
{
    _controlUnit = controlUnit;
    _clock = clock;
    _alu = alu;
    _memoryController = memoryController;
    _registers = registers;

    _controlUnit->Initialize(_memoryController, _alu);
    _alu->Initialize(_registers);
}

void Z80X::Run()
{
    _controlUnit->RunCycle();
}

}