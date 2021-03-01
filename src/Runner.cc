#include "Runner.h"

using namespace gbx::interfaces;
using namespace gbxcore::interfaces;
using namespace std;

namespace gbx
{

Runner::Runner(shared_ptr<Runtime> runner)
    : _runtime(runner)
{}

void Runner::Run(size_t numberOfCycles, CancellationToken& token)
{
    for (auto i = 0llu; i < numberOfCycles && !token.IsCancelled(); ++i)
        RunHeadless();
}

void Runner::Run(CancellationToken& token)
{
    while (!token.IsCancelled())
        RunHeadless();
}

void Runner::RunWithDebugSupport(CancellationToken& token)
{
    while (!token.IsCancelled())
        RunWithDebugger();
}

void Runner::RunWithDebugSupport(size_t numberOfCycles, CancellationToken& token)
{
    for (auto i = 0llu; i < numberOfCycles && !token.IsCancelled(); ++i)
        RunWithDebugger();
}

inline void Runner::RunHeadless()
{
    _runtime->Run();
}

inline void Runner::RunWithDebugger()
{
    _runtime->Run();

    while (_requestQueue.size() != 0)
    {
        auto request = _requestQueue.front();
        // Handle Message
        //auto response = make_shared<DebugMessage>(MessageType::UnknownMessage);
        _requestQueue.pop();
    }
}

inline std::variant<uint8_t, uint16_t> Runner::ReadRegister(Register reg)
{
    return _runtime->ReadRegister(reg);
}

}