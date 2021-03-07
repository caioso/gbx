#pragma once

namespace gbx::protocol
{

enum MessageID : uint16_t
{
    // Register Bank Interaction Messages
    MessageReadRegister = 0xFFFE
};

}