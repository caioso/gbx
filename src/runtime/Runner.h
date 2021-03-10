#pragma once

#include <cstdint>
#include <memory>
#include <queue>
#include <variant>
#include <thread>

#include "../interfaces/DebugMessage.h"
#include "../interfaces/DebuggableRunner.h"
#include "../interfaces/ServerTransport.h"
#include "MessageHandler.h"
#include "interfaces/Runtime.h"
#include "CancellationToken.h"

namespace gbx::runtime
{

enum class RunnerMode
{
    Runtime,
    Debug
};

class Runner : public interfaces::DebuggableRunner
             , public std::enable_shared_from_this<Runner>
{
public:
    Runner(std::shared_ptr<gbxcore::interfaces::Runtime>);
    Runner(std::shared_ptr<gbxcore::interfaces::Runtime>, std::shared_ptr<interfaces::ServerTransport>);
    virtual ~Runner() = default;

    void Run(CancellationToken&);
    void Run(size_t, CancellationToken&);

    void ClientJoined() override;
    void ClientLeft() override;

    [[nodiscard]] RunnerMode Mode();
    
private:
    inline void RunInDebugMode();
    inline void RunHeadless(CancellationToken&);
    inline void RunHeadless(size_t, CancellationToken&);
    inline void RunWithDebugger(CancellationToken&);
    inline void RunWithDebugger(size_t, CancellationToken&);
    inline void InitializeDebugInfraIfNeeded();

    std::shared_ptr<gbxcore::interfaces::Runtime> _runtime;
    std::shared_ptr<interfaces::ServerTransport> _transport;
    
    std::shared_ptr<MessageHandler> _handler;
    
    std::queue<std::shared_ptr<interfaces::DebugMessage>> _requestQueue;
    
    RunnerMode _mode;

    bool _clientJoined{};
    bool _halted{};
};

}