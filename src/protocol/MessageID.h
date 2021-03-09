#pragma once

namespace gbx::protocol
{

enum MessageID : uint16_t
{
    // Client Jointed
    MessageClientJoined = 0xFFFF,
    // Register Bank Interaction Messages
    MessageReadRegister = 0xFFFE
};

}