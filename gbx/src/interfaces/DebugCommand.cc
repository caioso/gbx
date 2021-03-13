#include "DebugCommand.h"

namespace gbx::interfaces
{

DebugCommand::DebugCommand(uint16_t type)
    : _type(type)
{}

uint16_t DebugCommand::Type()
{
    return _type;
}

}