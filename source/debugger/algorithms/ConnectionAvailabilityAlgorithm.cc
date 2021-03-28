#include "ConnectionAvailabilityAlgorithm.h"

namespace gbxdb::algorithms
{

ClientAvailability::ClientAvailability()
{}

ConnectionState ClientAvailability::State()
{
    return _state;
}

void ClientAvailability::EstablishConnection()
{
    _state = ConnectionState::Connected;
}

uint8_t ClientAvailability::Beacon()
{
    return _beacon;
}

ServerAvailability::ServerAvailability()
{}

ConnectionState ServerAvailability::State()
{
    return _state;
}

void ServerAvailability::EstablishConnection()
{
    _state = ConnectionState::Connected;
}

uint8_t ServerAvailability::Beacon()
{
    return _beacon;
}

}