#pragma once

#include <memory>

#include "ArithmeticLogicUnitInterface.h"
#include "ClockInterface.h"
#include "ControlUnitInterface.h"
#include "MemoryControllerInterface.h"
#include "RegisterBankInterface.h"
#include "VideoOutputInterface.h"

namespace gbxcore
{

class Z80X
{
public:
    Z80X() = default;
    ~Z80X() = default;

    void Initialize(std::unique_ptr<interfaces::ControlUnitInterface>, 
                    std::unique_ptr<interfaces::ClockInterface>,
                    std::unique_ptr<interfaces::ArithmeticLogicUnitInterface>,
                    std::unique_ptr<interfaces::MemoryControllerInterface>,
                    std::unique_ptr<interfaces::RegisterBankInterface>,
                    std::unique_ptr<interfaces::VideoOutputInterface>);

    void Run();

protected:
    std::unique_ptr<interfaces::ControlUnitInterface> _controlUnit;
    std::unique_ptr<interfaces::RegisterBankInterface> _registers;
    std::unique_ptr<interfaces::ClockInterface> _clock;
    std::unique_ptr<interfaces::ArithmeticLogicUnitInterface> _alu;
    std::unique_ptr<interfaces::MemoryControllerInterface> _memoryController;
    std::unique_ptr<interfaces::VideoOutputInterface> _videoOutput;
};

}