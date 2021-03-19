#pragma once 

namespace gbxdb
{

enum ClientMessageID : uint16_t
{
    // Joined Server
    JoinedServer = 0xFFFF,

    // Register Bank Interaction Messages
    MessageReadRegister = 0xFFFE,
    MessageRegisterBankSummary = 0xFFFD,
    MessageWriteRegister = 0xFFFC,

    // Message Error
    MessageError = 0x0001
};

}