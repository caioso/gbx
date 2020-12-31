#pragma once

#include <memory>

#include "ArithmeticLogicUnit.h"
#include "AddressRange.h"
#include "ControlUnit.h"
#include "EngineParameters.h"
#include "MemoryController.h"
#include "RegisterBank.h"
#include "ROM.h"

namespace gbx
{

class CPU
{
public:
    CPU();
    virtual ~CPU() = default;

    void Initialize();
    void Run();
    void Run(uint32_t);

protected:
    inline void UpdateEmulation();
    inline void InitializeMemorySubsystem();
    inline void InitializeReferences();

    std::shared_ptr<ControlUnit> _controlUnit;
    std::shared_ptr<ArithmeticLogicUnit> _alu;
    std::shared_ptr<MemoryController> _memoryController;

    // Memory Resources
    std::shared_ptr<ROM> _internalROM;
};

}