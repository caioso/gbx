#include "ConnectionAvailabilityAlgorithm.h"

using namespace std;
namespace gbxdb::algorithms
{

ConnectionAvailability::ConnectionAvailability(std::optional<size_t> limit)
{
    if (limit == nullopt)
        _failureLimit = 10;
    else
        _failureLimit = limit.value();
}

ConnectionState ConnectionAvailability::State()
{
    return _state;
}

void ConnectionAvailability::EstablishConnection()
{
    _state = ConnectionState::Connected;
    _missedBeacons = 0;
}

void ConnectionAvailability::Tick(BeaconState state)
{
    if (state == BeaconState::NoBeaconReceived)
        _missedBeacons++;
    else if (state == BeaconState::BeaconReceived)
        _missedBeacons= 0;

    EvaluateConnectionState();
}

void ConnectionAvailability::EvaluateConnectionState()
{
    if (_missedBeacons == _failureLimit)
    {
        _state = ConnectionState::NotConnected;
        _missedBeacons = 0;
    }
}

size_t ConnectionAvailability::MissedBeacons()
{
    return _missedBeacons;
}

}