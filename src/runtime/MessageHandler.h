#pragma once

#include <array>
#include <memory>

#include "../interfaces/ServerTransport.h"
#include "Observer.h"

namespace gbx::runtime
{

class MessageHandler : public gbxcommons::Observer, 
                       public std::enable_shared_from_this<MessageHandler>
{
public:
    MessageHandler(std::shared_ptr<interfaces::ServerTransport>);
    ~MessageHandler() = default;
    
    void Initialize();
    virtual void Notify(std::shared_ptr<gbxcommons::NotificationArguments>) override;

private:
    std::shared_ptr<interfaces::ServerTransport> _transport;
};

}