#pragma once

#include <iostream>
#include <memory>
#include <vector>

#include "interfaces/ServerProtocol.h"
#include "Observer.h"

namespace gbx
{

class RequestEventArgs
{
public:
    RequestEventArgs() = default;
    virtual ~RequestEventArgs() = default;
};

class RequestTranslator : public gbxcommons::Observer<interfaces::RawRequestEventArgs>
{
public:
    RequestTranslator();
    ~RequestTranslator() = default;

    virtual void Notify(std::shared_ptr<interfaces::RawRequestEventArgs>) override;
    void AddEventListener(std::shared_ptr<gbxcommons::Observer<RequestEventArgs>>);

private:
    std::vector<std::shared_ptr<gbxcommons::Observer<RequestEventArgs>>> _observers;
};

}