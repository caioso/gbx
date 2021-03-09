#pragma once

#include <memory>

namespace gbx::interfaces
{

class DebuggableRunner
{
public:
    DebuggableRunner() = default;
    virtual ~DebuggableRunner() = default;

    // Blocking call
    virtual void ClientJoined() = 0;
    virtual void ClientLeft() = 0;
};

}