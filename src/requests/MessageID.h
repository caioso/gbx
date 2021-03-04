#pragma once

#include <cstdint>

namespace gbx::requests
{

class MessageID
{
public:
    inline static const uint16_t ClientConnectedMessage = 0xFFFF;
    
    inline static const uint16_t DebugServerStatusRequest = 0xFFFE;
    inline static const uint16_t DebugServerStatusResponse = 0xFFFE;
};

}