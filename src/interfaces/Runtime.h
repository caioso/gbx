#pragma once

#include "RegisterBankInterface.h"
namespace gbxcore::interfaces
{
struct Runtime
{
    virtual void Run() = 0;
    virtual uint8_t ReadRegister(Register) = 0;
};

}