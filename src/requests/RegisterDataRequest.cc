#include "RegisterDataRequest.h"

using namespace gbx::interfaces;
namespace gbx::requests
{

RegisterDataRequest::RegisterDataRequest(enum Register reg, RegisterDataOperation op)
    : interfaces::DebugRequest(RequestType::Register)
    , _register(reg)
    , _operation(op)
{}

Register RegisterDataRequest::Register()
{
    return _register;
}

RegisterDataOperation RegisterDataRequest::Operation()
{
    return _operation;
}

}