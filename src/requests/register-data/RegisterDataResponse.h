#pragma once

#include <variant>

#include "../../interfaces/DebugMessage.h"
#include "interfaces/RegisterBankInterface.h"

namespace gbx::requests
{

class RegisterDataResponse : public interfaces::DebugMessage
{
public:
    RegisterDataResponse(gbxcore::interfaces::Register, std::variant<uint8_t, uint16_t>);
    virtual ~RegisterDataResponse() = default;

    gbxcore::interfaces::Register Register();
    std::variant<uint8_t, uint16_t> Value();

private:
    gbxcore::interfaces::Register _register;
    std::variant<uint8_t, uint16_t> _value;
};

}