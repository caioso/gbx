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
    RegisterDataRequest(gbxcore::interfaces::Register, RegisterDataOperation);
    virtual ~RegisterDataRequest() = default;

    gbxcore::interfaces::Register Register();
    RegisterDataOperation Operation();

private:
    gbxcore::interfaces::Register _register;
    RegisterDataOperation _operation;
};

}