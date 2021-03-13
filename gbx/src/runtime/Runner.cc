#include "Runner.h"

using namespace gbx::interfaces;
using namespace gbxcore::interfaces;
using namespace std;

namespace gbx::runtime
{

Runner::Runner(shared_ptr<Runtime> runner)
    : _runtime(runner)
    , _mode(RunnerMode::Runtime)
{}

Runner::Runner(shared_ptr<Runtime> runner, shared_ptr<ServerTransport> transport)
    : _runtime(runner)
    , _transport(transport)
    , _mode(RunnerMode::Debug)
    , _halted(true)
{}

RunnerMode Runner::Mode()
{
    return _mode;
}

void Runner::Run(size_t numberOfCycles, CancellationToken& token)
{
    if (_mode == RunnerMode::Runtime)
        RunHeadless(numberOfCycles, token);
    else
        RunWithDebugger(numberOfCycles, token);
}

void Runner::Run(CancellationToken& token)
{
    if (_mode == RunnerMode::Runtime)
        RunHeadless(token);
    else
        RunWithDebugger(token);
}

inline void Runner::RunHeadless(CancellationToken& token)
{
    while (!token.IsCancelled())
        _runtime->Run();
}

inline void Runner::RunHeadless(size_t numberOfCycles, CancellationToken& token)
{
    for (auto i = 0llu; i < numberOfCycles && !token.IsCancelled(); ++i)
        _runtime->Run();
}

inline void Runner::RunWithDebugger(CancellationToken& token)
{
    InitializeDebugInfraIfNeeded();

    while (!token.IsCancelled())
        RunInDebugMode();
}

inline void Runner::RunWithDebugger(size_t numberOfCycles, CancellationToken& token)
{
    InitializeDebugInfraIfNeeded();
    for (auto i = 0llu; i < numberOfCycles && !token.IsCancelled(); ++i)
        RunInDebugMode();
}

inline void Runner::InitializeDebugInfraIfNeeded()
{
    if (_handler == nullptr)
    {
        _handler = make_shared<MessageHandler>(_transport);
        _handler->Initialize();
    }

    // Initialize Transport Thread
    _transport->WaitForClient();

    while(!_clientJoined)
    {
        while (_handler->Pending() != 0llu)
            _handler->ProcessMessages(_runtime, shared_from_this());
        
        std::this_thread::sleep_for(10ms);
    }
}

inline void Runner::RunInDebugMode()
{
    if (!_halted)
        _runtime->Run();

    while (_handler->Pending() != 0llu)
        _handler->ProcessMessages(_runtime, shared_from_this());
}

void Runner::ClientJoined()
{
    _clientJoined = true;
}

void Runner::ClientLeft()
{
    _clientJoined = false;
}

}