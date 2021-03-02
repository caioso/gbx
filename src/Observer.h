#pragma once

#include <memory>

namespace gbxcommons
{

class NotificationArgs
{
public:
    NotificationArgs() = default;
    virtual ~NotificationArgs() = default;
};

class Observer
{
public:
    virtual ~Observer() = default;
    virtual void Notify(std::shared_ptr<NotificationArgs>) = 0;
};

}