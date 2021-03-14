#pragma once

#include <memory>

namespace gbxcommons
{

class Observer;

class Subject
{
public:
    virtual ~Subject() = default;
    virtual void Subscribe(std::weak_ptr<Observer>) = 0;
    virtual void Unsubscribe(std::weak_ptr<Observer>) = 0;
};

}