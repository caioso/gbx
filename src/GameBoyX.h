#pragma once

#include <memory>

#include "ArithmeticLogicUnit.h"
#include "Clock.h"
#include "ControlUnit.h"
#include "EngineParameters.h"
#include "MemoryController.h"
#include "RegisterBank.h"
#include "ROM.h"
#include "Z80X.h"

namespace gbx
{

class GameBoyX
{
public:
    GameBoyX();
    virtual ~GameBoyX() = default;
    void Run();
    
protected:
    std::shared_ptr<Z80X> _cpu;
    std::shared_ptr<ControlUnit> _controlUnit;
    std::shared_ptr<MemoryController> _memoryController;
    std::shared_ptr<ROM> _systemROM;
    std::shared_ptr<ArithmeticLogicUnit> _alu;
    std::shared_ptr<Clock> _clock;
    std::shared_ptr<RegisterBank> _registers;
};

}