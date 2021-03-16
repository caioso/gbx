#pragma once

#include <array>
#include <cstdlib>
#include <memory>

namespace gbxdb::interfaces
{

const size_t MaxMessageBufferSize = 256;
class DebugMessage
{
public:
    DebugMessage(std::shared_ptr<std::array<uint8_t, MaxMessageBufferSize>>);
    ~DebugMessage() = default;

    std::shared_ptr<std::array<uint8_t, MaxMessageBufferSize>> Buffer();

private:
    std::shared_ptr<std::array<uint8_t, MaxMessageBufferSize>> _messageBytes;
};

}