#include "Runner.h"

using namespace gbx::interfaces;

namespace gbx
{

Runner::Runner(std::shared_ptr<Runtime> runner)
    : _runtime(runner)
{}

void Runner::Run()
{
    while (true)
        _runtime->Run();
}

void Runner::Run(size_t numberOfCycles)
{
    for (auto i = 0llu; i < numberOfCycles; ++i)
        _runtime->Run();
}

void Runner::Run(CancellationToken& token)
{
    while (!token.IsCancelled())
        _runtime->Run();
}

}

