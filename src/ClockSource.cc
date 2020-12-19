#include "ClockSource.h"

using namespace std;

namespace gbx
{
ClockSource::ClockSource(uint64_t frequencyInHertz)
    : _clockPeriod(1.0/frequencyInHertz)
    , _ticks(0)
    , _observersCounter(0)
{
    _threadSleepDuration = _clockPeriod*1000000000;
}

void ClockSource::Subscribe(std::weak_ptr<ClockObserver> observer)
{
    HasBeenRegistered(observer);
    _observers[_observersCounter++] = observer;
}

void ClockSource::HasBeenRegistered(std::weak_ptr<ClockObserver> observer)
{
    for(auto i = static_cast<size_t>(0); i < _observersCounter; i++)
    {
        if (observer.expired() || _observers.at(i).expired())
            throw ClockSourceException("null memory resource detected");

        if (_observers.at(i).lock() == observer.lock())
            throw ClockSourceException("observer already registerd");
    }
}

void ClockSource::Tick()
{
    this_thread::sleep_for(chrono::nanoseconds(_threadSleepDuration));
    _ticks++;

    for (auto i = static_cast<size_t>(0); i < _observersCounter; i++)
        if (!_observers.at(i).expired()) _observers.at(i).lock()->OnTick();
}

uint64_t ClockSource::Ticks()
{
    return _ticks;
}

double ClockSource::Period()
{
    return _clockPeriod;
}

}