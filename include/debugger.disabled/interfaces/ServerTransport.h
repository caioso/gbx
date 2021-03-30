#pragma once

#include <memory>

#include "DebugMessage.h"
#include "Subject.h"

namespace gbxdb::interfaces
{

class ServerTransport : public gbxcommons::Subject
{
public:
    ServerTransport() = default;
    virtual ~ServerTransport() = default;

    // Blocking call
    virtual void WaitForClient() = 0;
    virtual void SendMessage(std::shared_ptr<DebugMessage>) = 0;
};

}