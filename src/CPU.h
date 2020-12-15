#pragma once

#include <memory>
#include "RegisterBank.h"
#include "ROM.h"

namespace gbx
{

class CPU
{
public:
    CPU();
    ~CPU() = default;

    void Run();

protected:
    void InitializeRegisterBank();

    std::unique_ptr<RegisterBank> _registerBank;
    std::unique_ptr<ROM> _internalROM;
};

}