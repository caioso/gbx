#include "RegisterDataRequest.h"

namespace gbx::requests
{

RegisterDataRequest::RegisterDataRequest(size_t reg, RegisterDataOperation op, interfaces::RequestType type)
    : interfaces::DebugRequest(type)
    , _register(reg)
    , _operation(op)
{}

size_t RegisterDataRequest::Register()
{
    return _register;
}

RegisterDataOperation RegisterDataRequest::Operation()
{
    return _operation;
}

}