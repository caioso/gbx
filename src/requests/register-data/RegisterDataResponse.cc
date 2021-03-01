#include "RegisterDataResponse.h"

using namespace gbxcore::interfaces;
using namespace gbx::interfaces;
using namespace std;

namespace gbx::requests
{

RegisterDataResponse::RegisterDataResponse(enum Register reg, variant<uint8_t, uint16_t> value)
    : DebugMessage(MessageType::RegisterResponse)
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