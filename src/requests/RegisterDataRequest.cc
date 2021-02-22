#include "RegisterDataRequest.h"

using namespace gbx::interfaces;
using namespace gbxcore::interfaces;

namespace gbx::requests
{

RegisterDataRequest::RegisterDataRequest(enum Register reg, RegisterDataOperation op, std::optional<std::variant<std::uint8_t, std::uint16_t>> value)
    : interfaces::DebugRequest(RequestType::Register)
    , _register(reg)
    , _operation(op)
    , _value(value.value_or(static_cast<uint8_t>(0x00)))
{}

Register RegisterDataRequest::Register()
{
    return _register;
}

RegisterDataOperation RegisterDataRequest::Operation()
{
    return _operation;
}

std::variant<std::uint8_t, std::uint16_t> RegisterDataRequest::Value()
{
    return _value;
}

}