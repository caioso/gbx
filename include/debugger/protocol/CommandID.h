
#pragma once

namespace gbxdb::protocol
{

enum CommandID : uint16_t
{
    // Client Joined Command
    CommandJoined = 0x0000,
    // Register Bank Interaction Commands
    CommandReadRegister = 0x0001,
    CommandRegisterBankSummary = 0x0002,
    CommandWriteRegister = 0x0003,
    
    // Protocol Initializer
    CommandProtocolInitializer = 0xFFFE,
    
    // Command Error
    CommandError = 0xFFFF
};

}