#pragma once

namespace gbxdb::protocol
{

enum ErrorID : uint16_t
{
    // Invalid register on Read/Write Debug Messages
    InvalidRegister = 0x0001
};

}