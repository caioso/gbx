#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "interfaces/ServerProtocol.h"
#include "interfaces/DebugMessage.h"

#include "requests/MessageID.h"
#include "requests/client-connected/ClientConnectedRequest.h"
#include "requests/debug-server-status/DebugServerStatusRequest.h"
#include "Observer.h"

namespace gbx
{

class DebugMessageEventArgs
{
public:
    DebugMessageEventArgs(std::shared_ptr<interfaces::DebugMessage>);
    ~DebugMessageEventArgs() = default;

    std::shared_ptr<interfaces::DebugMessage> Message();

private:
    std::shared_ptr<interfaces::DebugMessage> _message;
};

class RequestTranslator : public gbxcommons::Observer<interfaces::RawDebugMessageEventArgs>
{
public:
    RequestTranslator() = default;
    ~RequestTranslator() = default;

    virtual void Notify(std::shared_ptr<interfaces::RawDebugMessageEventArgs>) override;
    void AddEventListener(std::weak_ptr<gbxcommons::Observer<DebugMessageEventArgs>>);

private:
    std::shared_ptr<DebugMessageEventArgs> DecodeRawRequest(std::array<uint8_t, interfaces::MaxRawRequestSize>);

    std::vector<std::weak_ptr<gbxcommons::Observer<DebugMessageEventArgs>>> _observers;
};

}