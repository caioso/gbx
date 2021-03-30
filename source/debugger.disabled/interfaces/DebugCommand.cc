#include "DebugCommand.h"

namespace gbxdb::interfaces
{

DebugCommand::DebugCommand(uint16_t type)
    : _type(type)
{}

uint16_t DebugCommand::Type()
{
    return _type;
}

}