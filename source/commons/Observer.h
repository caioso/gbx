#pragma once

#include <memory>

#include "Subject.h"

namespace gbxcommons
{

class NotificationArguments
{
public:
    NotificationArguments() = default;
    virtual ~NotificationArguments() = default;
};
    
class Observer
{
public:
    virtual ~Observer() = default;
    virtual void Notify(std::shared_ptr<NotificationArguments>) = 0;
};

}