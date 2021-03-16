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
    virtual void JoinServer() = 0;
    virtual void SendMessage(std::shared_ptr<DebugMessage>) = 0;
};

}