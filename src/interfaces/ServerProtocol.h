#pragma once

#include <array>
#include <memory>

#include "DebugMessage.h"
#include "Observer.h"
#include "ServerProtocolParameters.h"

namespace gbx::interfaces
{

const size_t MaxRawRequestSize = 128;
class RawDebugMessageEventArgs
{
public:
    RawDebugMessageEventArgs(std::array<uint8_t, MaxRawRequestSize>);
    ~RawDebugMessageEventArgs() = default;
    
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
    virtual void AddEventListener(std::weak_ptr<gbxcommons::Observer<RawDebugMessageEventArgs>>) = 0;
    virtual void Send(std::shared_ptr<DebugMessage>) = 0;
};

}