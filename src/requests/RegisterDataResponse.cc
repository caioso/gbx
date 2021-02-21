#include "RegisterDataResponse.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx::requests
{

RegisterDataResponse::RegisterDataResponse(enum Register reg, variant<uint8_t, uint16_t> value)
    : DebugResponse(ResponseType::Register)
    , _register(reg)
    , _value(value)
{}

Register RegisterDataResponse::Register()
{
    return _register;
}

variant<uint8_t, uint16_t> RegisterDataResponse::Value()
{
    return _value;
}

}