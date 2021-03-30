#include "Runner.h"


using namespace gbxcore::interfaces;
using namespace std;

namespace gbxruntime::runner
{

Runner::Runner(shared_ptr<Runtime> runner)
    : _runtime(runner)
    , _mode(RunnerMode::Runtime)
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
{}

inline void Runner::RunInDebugMode()
{}

}