
#pragma once

namespace gbx::protocol
{

enum CommandID : uint16_t
{
    // Client Joined Command
    CommandClientJoined = 0,
    // Register Bank Interaction Commands
    CommandReadRegister = 1
};

}