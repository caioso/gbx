#pragma once

#include <cstdint>
#include <memory>

#include "DebugMessage.h"

namespace gbxdb::interfaces
{

class DebugCommand
{
public:
    DebugCommand(uint16_t);
    virtual ~DebugCommand() = default;

    virtual void DecodeRequestMessage(std::shared_ptr<DebugMessage>) = 0;
    virtual std::shared_ptr<DebugMessage> EncodeRequestMessage() = 0;
    uint16_t Type();

private:
    uint16_t _type;
};

}