#pragma once

namespace gbx::protocol
{

enum ErrorID : uint16_t
{
    // Invalid register on Read/Write Debug Messages
    InvalidRegister = 0x0001
};

}