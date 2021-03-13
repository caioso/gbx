#pragma once

#include <memory>

#include "EngineParameters.h"
#include "interfaces/ArithmeticLogicUnitInterface.h"
#include "interfaces/ClockInterface.h"
#include "interfaces/ControlUnitInterface.h"
#include "interfaces/MemoryControllerInterface.h"
#include "interfaces/RegisterBankInterface.h"

namespace gbxcore
{

class Z80X : public std::enable_shared_from_this<Z80X>
{
public:
    Z80X() = default;
    ~Z80X() = default;

    void Initialize(std::shared_ptr<interfaces::ControlUnitInterface>, 
                    std::shared_ptr<interfaces::ClockInterface>,
                    std::shared_ptr<interfaces::ArithmeticLogicUnitInterface>,
                    std::shared_ptr<interfaces::MemoryControllerInterface>,
                    std::shared_ptr<interfaces::RegisterBankInterface>);

    void Run();

protected:
    std::shared_ptr<interfaces::ControlUnitInterface> _controlUnit;
    std::shared_ptr<interfaces::RegisterBankInterface> _registers;
    std::shared_ptr<interfaces::ClockInterface> _clock;
    std::shared_ptr<interfaces::ArithmeticLogicUnitInterface> _alu;
    std::shared_ptr<interfaces::MemoryControllerInterface> _memoryController;
};

}