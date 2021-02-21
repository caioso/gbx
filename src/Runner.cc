#include "Runner.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

Runner::Runner(shared_ptr<Runtime> runner, shared_ptr<DebugRequestProducer> producer)
    : DebugRequestConsumer(producer)
    , _runtime(runner)
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

void Runner::ConsumeRequest(shared_ptr<interfaces::DebugRequest> request)
{
    _requestQueue.push(request);
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
        // Handle Request
        auto response = make_shared<DebugResponse>(ResponseType::UnknownType);
        _producer->ConsumeResponse(response);
        _requestQueue.pop();
    }
}

inline uint8_t Runner::ReadRegister(Register reg)
{
    return _runtime->ReadRegister(reg);
}

}