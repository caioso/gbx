#include "ServerProtocol.h"

namespace gbx::interfaces
{

RawRequestEventArgs::RawRequestEventArgs(std::array<uint8_t, MaxRawRequestSize> rawRequest)
    : _rawRequewst(rawRequest)
{}

std::array<uint8_t, MaxRawRequestSize> RawRequestEventArgs::RawRequest()
{
    return _rawRequewst;
}

}