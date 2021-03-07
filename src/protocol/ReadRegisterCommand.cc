#include "ReadRegisterCommand.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx::protocol
{
ReadRegisterCommand::ReadRegisterCommand(gbxcore::interfaces::Register reg)
    : DebugCommand(static_cast<uint16_t>(CommandID::CommandReadRegister))
    , _register(reg)
{}

gbxcore::interfaces::Register ReadRegisterCommand::RegisterToRead()
{
    return _register;
}

}