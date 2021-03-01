#pragma once

#include <optional>
#include <variant>

#include "../../interfaces/DebugMessage.h"
#include "interfaces/RegisterBankInterface.h"
namespace gbx::requests
{

enum class RegisterDataOperation
{
    Read,
    Write
};

class RegisterDataRequest : public interfaces::DebugMessage
{
public:
    RegisterDataRequest(gbxcore::interfaces::Register, RegisterDataOperation, std::optional<std::variant<std::uint8_t, std::uint16_t>>);
    virtual ~RegisterDataRequest() = default;

    gbxcore::interfaces::Register Register();
    RegisterDataOperation Operation();
    std::variant<std::uint8_t, std::uint16_t> Value();

private:
    gbxcore::interfaces::Register _register;
    RegisterDataOperation _operation;
    std::variant<std::uint8_t, std::uint16_t> _value;
};

}