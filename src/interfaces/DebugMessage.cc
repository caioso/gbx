#include "DebugMessage.h"

using namespace std;

namespace gbx::interfaces
{

DebugMessage::DebugMessage(shared_ptr<array<uint8_t, MaxMessageBufferSize>> message)
    : _messageBytes(message)
{}

shared_ptr<array<uint8_t, MaxMessageBufferSize>> DebugMessage::Buffer()
{
    return _messageBytes;
}

}