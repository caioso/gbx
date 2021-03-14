#pragma once

#include <variant>

#include "RegisterBankInterface.h"
namespace gbxcore::interfaces
{
struct Runtime
{
    virtual ~Runtime() = default;
    virtual void Run() = 0;
    virtual std::variant<uint8_t, uint16_t> ReadRegister(Register) = 0;
    virtual void WriteRegister(Register, std::variant<uint8_t, uint16_t>) = 0;
};

}