#include "ServerProtocol.h"

namespace gbx::interfaces
{

RawDebugMessageEventArgs::RawDebugMessageEventArgs(std::array<uint8_t, MaxRawRequestSize> rawRequest)
    : _rawRequewst(rawRequest)
{}

std::array<uint8_t, MaxRawRequestSize> RawDebugMessageEventArgs::RawRequest()
{
    return _rawRequewst;
}

}