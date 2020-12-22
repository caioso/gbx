#pragma once

#include <map>
#include <memory>

#include "ArithmeticLogicUnit.h"
#include "ClockObserver.h"
#include "AddressRange.h"
#include "ClockSource.h"
#include "ControlUnit.h"
#include "EngineParameters.h"
#include "MemoryController.h"
#include "RegisterBank.h"
#include "ROM.h"

namespace gbx
{

class CPU : public ClockObserver, public std::enable_shared_from_this<CPU>
{
public:
    CPU();
    virtual ~CPU() = default;

    // Clock Observer
    virtual void OnTick() override;

    void Initialize();
    void Run();
    void Run(uint32_t cycles);

protected:
    inline void UpdateEmulation();
    void InitializeClockSubsystem();
    void InitializeMemorySubsystem();

    std::unique_ptr<ClockSource> _clock;
    std::unique_ptr<ControlUnit> _controlUnit;
    std::unique_ptr<ArithmeticLogicUnit> _alu;
    std::unique_ptr<MemoryController> _memoryController;

    // Memory Resources
    std::shared_ptr<ROM> _internalROM;
};

}