#pragma once

#include <memory>

#include "ClockSource.h"
#include "ClockObserver.h"
#include "ControlUnit.h"
#include "EngineParameters.h"
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
    void InitializeRegisterBank();
    void InitializeClockSubsystem();

    std::unique_ptr<ClockSource> _clock;
    std::unique_ptr<RegisterBank> _registerBank;
    std::unique_ptr<ROM> _internalROM;
    std::unique_ptr<ControlUnit> _controlUnit;
};

}