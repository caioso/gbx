#pragma once

#include <memory>

#include "../interfaces/DebugCommand.h"
#include "interfaces/RegisterBankInterface.h"
#include "CommandID.h"

namespace gbx::protocol
{

class ReadRegisterCommand : public interfaces::DebugCommand
{
public:
    ReadRegisterCommand(gbxcore::interfaces::Register);
    ~ReadRegisterCommand() = default;

    gbxcore::interfaces::Register RegisterToRead();

private:
    gbxcore::interfaces::Register _register;
};

}