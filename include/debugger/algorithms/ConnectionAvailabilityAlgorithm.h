#pragma once

#include <optional>
#include <stdint.h>

namespace gbxdb::algorithms
{

enum class ConnectionState
{
    NotConnected,
    Connected,
};

enum class BeaconState
{
    BeaconReceived,
    NoBeaconReceived
};

// This runs at the SERVER side
class ConnectionAvailability
{
public:
    ConnectionAvailability(std::optional<size_t>);
    ~ConnectionAvailability() = default;

    ConnectionAvailability(const ConnectionAvailability&) = default;
    ConnectionAvailability(ConnectionAvailability&&) = default;
    ConnectionAvailability& operator=(const ConnectionAvailability&) = default;
    ConnectionAvailability& operator=(ConnectionAvailability&&) = default;

    ConnectionState State();
    
    void Tick(BeaconState);
    void EstablishConnection();
    void EvaluateConnectionState();

    size_t MissedBeacons();

private:
    ConnectionState _state{};
    size_t _missedBeacons{};
    size_t _failureLimit;
};

}