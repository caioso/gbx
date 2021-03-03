#pragma once

#include <array>
#include <memory>

#include "DebugMessage.h"
#include "Observer.h"
#include "ServerProtocolParameters.h"

namespace gbx::interfaces
{

const size_t MaxRawRequestSize = 128;
class RawRequestEventArgs
{
public:
    RawRequestEventArgs(std::array<uint8_t, MaxRawRequestSize>);
    virtual ~RawRequestEventArgs() = default;
    
    std::array<uint8_t, MaxRawRequestSize> RawRequest();

private:
    std::array<uint8_t, MaxRawRequestSize> _rawRequewst;
};

class ServerProtocol
{
public:
    virtual ~ServerProtocol() = default;

    virtual void Initialize(std::shared_ptr<ServerProtocolParameters>) = 0;
    virtual void WaitForClient() = 0;
    virtual void AddEventListener(std::shared_ptr<gbxcommons::Observer<RawRequestEventArgs>>) = 0;
    virtual void Send(std::shared_ptr<DebugMessage>) = 0;
};

}