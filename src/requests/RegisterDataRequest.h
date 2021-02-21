#pragma once

#include "../interfaces/DebugRequest.h"
#include "interfaces/RegisterBankInterface.h"
namespace gbx::requests
{

enum class RegisterDataOperation
{
    Read,
    Write
};

class RegisterDataRequest : public interfaces::DebugRequest
{
public:
    RegisterDataRequest(interfaces::Register, RegisterDataOperation);
    virtual ~RegisterDataRequest() = default;

    interfaces::Register Register();
    RegisterDataOperation Operation();

private:
    interfaces::Register _register;
    RegisterDataOperation _operation;
};

}