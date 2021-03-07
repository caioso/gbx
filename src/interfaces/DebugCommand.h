#pragma once

#include <cstdint>

namespace gbx::interfaces
{

class DebugCommand
{
public:
    DebugCommand(uint16_t);
    virtual ~DebugCommand() = default;
    
    uint16_t Type();

private:
    uint16_t _type;
};

}