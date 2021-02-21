#pragma once

#include <variant>

#include "../interfaces/DebugResponse.h"
#include "interfaces/RegisterBankInterface.h"

namespace gbx::requests
{

class RegisterDataResponse : public interfaces::DebugResponse
{
public:
    RegisterDataResponse(interfaces::Register, std::variant<uint8_t, uint16_t>);
    virtual ~RegisterDataResponse() = default;

    interfaces::Register Register();
    std::variant<uint8_t, uint16_t> Value();

private:
    interfaces::Register _register;
    std::variant<uint8_t, uint16_t> _value;
};

}