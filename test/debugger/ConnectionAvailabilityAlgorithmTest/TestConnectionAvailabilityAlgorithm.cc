#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ConnectionAvailabilityAlgorithm.h"

#include <array>
#include <memory>
#include <optional>
#include <string>

using namespace gbxdb::algorithms;
using namespace std;

using ::testing::Return;
using ::testing::_;

TEST(DebuggerTests_ServerConnectionAvailabilityAlgorithm, ConstructionClientAvailability) 
{
    ClientAvailability algorithm;
}

TEST(DebuggerTests_ServerConnectionAvailabilityAlgorithm, CheckInitialStateClientAvailability) 
{
    ClientAvailability algorithm;
    EXPECT_EQ(ConnectionState::NotConnected, algorithm.State());
}

TEST(DebuggerTests_ServerConnectionAvailabilityAlgorithm, InitializeConnectionClientAvailability) 
{
    ClientAvailability algorithm;
    algorithm.EstablishConnection();
    EXPECT_EQ(ConnectionState::Connected, algorithm.State());
    EXPECT_EQ(0x00, algorithm.Beacon());
}

TEST(DebuggerTests_ServerConnectionAvailabilityAlgorithm, ConstructionServerAvailability) 
{
    ServerAvailability algorithm;
}

TEST(DebuggerTests_ServerConnectionAvailabilityAlgorithm, CheckInitialStateServerAvailability) 
{
    ServerAvailability algorithm;
    EXPECT_EQ(ConnectionState::NotConnected, algorithm.State());
}

TEST(DebuggerTests_ServerConnectionAvailabilityAlgorithm, InitializeConnectionServerAvailability) 
{
    ServerAvailability algorithm;
    algorithm.EstablishConnection();
    EXPECT_EQ(ConnectionState::Connected, algorithm.State());
    EXPECT_EQ(0x00, algorithm.Beacon());
}