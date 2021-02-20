#pragma once

#include "../interfaces/DebugRequest.h"

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
    RegisterDataRequest(size_t, RegisterDataOperation, interfaces::RequestType);
    virtual ~RegisterDataRequest() = default;

    size_t Register();
    RegisterDataOperation Operation();

private:
    size_t _register;
    RegisterDataOperation _operation;
};

}