#include "Clock.h"

using namespace std;
using namespace std::this_thread;
using namespace std::chrono_literals;
using std::chrono::system_clock;

namespace gbx
{

Clock::Clock(uint64_t clockPeriod)
    : _clockPeriodInNanoSeconds(clockPeriod)
    , _ticks(0)
{}

double Clock::Period()
{
    return _clockPeriodInNanoSeconds;
}

uint64_t Clock::Ticks()
{
    return _ticks;
}

void Clock::Tick(uint64_t ticks, uint64_t instructionExecutionTimInNanoseconds)
{
    auto sleepTime = std::chrono::nanoseconds(static_cast<int64_t>(_clockPeriodInNanoSeconds*ticks) - instructionExecutionTimInNanoseconds);

    if (sleepTime.count() > 0)
    {
        // This tecknique ensures more precision, because the current thread does not go through the OS scheduler
        // like this_thread::sleep_for would.
        auto start = chrono::high_resolution_clock::now();
        while(chrono::high_resolution_clock::now() - start < sleepTime);
    }

    _ticks += ticks;
}

}