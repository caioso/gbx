#pragma once

namespace gbxdb::protocol
{

enum MessageID : uint16_t
{
    // Client Jointed
    MessageClientJoined = 0xFFFF,
    
    // Register Bank Interaction Messages
    MessageReadRegister = 0xFFFE,
    MessageRegisterBankSummary = 0xFFFD,
    MessageWriteRegister = 0xFFFC,

    // Message Error
    MessageError = 0x0001
};

}