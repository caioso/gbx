
#pragma once

namespace gbxdb::protocol
{

enum ClientCommandID : uint16_t
{
    // Client Joined the Server Command
    JoinedServer = 0x0000,
    // Register Bank Interaction Commands
    CommandReadRegister = 0x0001,
    CommandRegisterBankSummary = 0x0002,
    CommandWriteRegister = 0x0003,
    
    // Command Error
    CommandError = 0xFFFF
};

}