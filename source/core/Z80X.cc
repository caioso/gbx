#include "Z80X.h"

using namespace std;
using namespace gbxcore::interfaces;

namespace gbxcore
{

void Z80X::Initialize(unique_ptr<ControlUnitInterface> controlUnit, 
                      unique_ptr<ClockInterface> clock, 
                      unique_ptr<ArithmeticLogicUnitInterface> alu,
                      unique_ptr<MemoryControllerInterface> memoryController,
                      unique_ptr<RegisterBankInterface> registers)
{
    _controlUnit = std::move(controlUnit);
    _clock = std::move(clock);
    _alu = std::move(alu);
    _memoryController = std::move(memoryController);
    _registers = std::move(registers);

    _alu->Initialize(_registers.get());
    _controlUnit->Initialize(_memoryController.get(), _alu.get());
}

void Z80X::Run()
{
    _controlUnit->RunCycle();
}

}