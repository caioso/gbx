#pragma once

#include <memory>

#include "DebugMessage.h"
#include "Subject.h"

namespace gbxdb::interfaces
{

class ClientTransport : public gbxcommons::Subject
{
public:
    ClientTransport() = default;
    virtual ~ClientTransport() = default;

    // Blocking call
    virtual void InitializeProtocol(std::shared_ptr<std::array<uint8_t, gbxdb::interfaces::MaxMessageBufferSize>>) = 0;
    virtual void JoinServer() = 0;
    virtual void LeaveServer() = 0;
    virtual void SendMessage(std::shared_ptr<DebugMessage>) = 0;
};

}