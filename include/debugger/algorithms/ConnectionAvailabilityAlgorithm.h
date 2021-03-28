#pragma once

#include <stdint.h>

namespace gbxdb::algorithms
{

enum class ConnectionState
{
    NotConnected,
    Connected,
    Unstable
};

// This runs at the SERVER side
class ClientAvailability
{
public:
    ClientAvailability();
    ~ClientAvailability() = default;

    ClientAvailability(const ClientAvailability&) = default;
    ClientAvailability(ClientAvailability&&) = default;
    ClientAvailability& operator=(const ClientAvailability&) = default;
    ClientAvailability& operator=(ClientAvailability&&) = default;

    ConnectionState State();
    void EstablishConnection();
    uint8_t Beacon();

private:
    ConnectionState _state{};
    uint8_t _beacon{};
};

// This runs at the CLIENT side
class ServerAvailability
{
public:
    ServerAvailability();
    ~ServerAvailability() = default;

    ServerAvailability(const ServerAvailability&) = default;
    ServerAvailability(ServerAvailability&&) = default;
    ServerAvailability& operator=(const ServerAvailability&) = default;
    ServerAvailability& operator=(ServerAvailability&&) = default;

    ConnectionState State();
    void EstablishConnection();
    uint8_t Beacon();

private:
    ConnectionState _state{};
    uint8_t _beacon{};
};

}