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

TEST(DebuggerTests_ConnectionAvailabilityAlgorithm, ConstructionClientAvailability) 
{
    ConnectionAvailability algorithm(nullopt);
    algorithm.EstablishConnection();
    EXPECT_EQ(ConnectionState::Connected, algorithm.State());
}

TEST(DebuggerTests_ConnectionAvailabilityAlgorithm, ValidTick) 
{
    ConnectionAvailability algorithm(nullopt);
    algorithm.EstablishConnection();
    EXPECT_EQ(ConnectionState::Connected, algorithm.State());

    algorithm.Tick(BeaconState::BeaconReceived);
    EXPECT_EQ(0x00, algorithm.MissedBeacons());
    EXPECT_EQ(ConnectionState::Connected, algorithm.State());
}

TEST(DebuggerTests_ConnectionAvailabilityAlgorithm, MissOneTick) 
{
    ConnectionAvailability algorithm(nullopt);
    algorithm.EstablishConnection();
    EXPECT_EQ(ConnectionState::Connected, algorithm.State());

    algorithm.Tick(BeaconState::NoBeaconReceived);
    EXPECT_EQ(0x01, algorithm.MissedBeacons());
    EXPECT_EQ(ConnectionState::Connected, algorithm.State());
    
    algorithm.Tick(BeaconState::BeaconReceived);
    EXPECT_EQ(0x00, algorithm.MissedBeacons());
    EXPECT_EQ(ConnectionState::Connected, algorithm.State());
}

TEST(DebuggerTests_ConnectionAvailabilityAlgorithm, MissLimitTick) 
{
    ConnectionAvailability algorithm(nullopt);
    algorithm.EstablishConnection();
    EXPECT_EQ(ConnectionState::Connected, algorithm.State());

    for (auto i = 0; i < 10; i++)
    {
        algorithm.Tick(BeaconState::NoBeaconReceived);
        
        if (i != 9)
        {
            EXPECT_EQ(i + 1, algorithm.MissedBeacons());
            EXPECT_EQ(ConnectionState::Connected, algorithm.State());
        }
        else
        {    
            EXPECT_EQ(0, algorithm.MissedBeacons());
            EXPECT_EQ(ConnectionState::NotConnected, algorithm.State());
        }
    }
}

TEST(DebuggerTests_ConnectionAvailabilityAlgorithm, RestoreCommunicationAfterAFewFailures) 
{
    ConnectionAvailability algorithm(nullopt);
    algorithm.EstablishConnection();

    for (auto i = 0; i < 5; i++)
    {
        algorithm.Tick(BeaconState::BeaconReceived);
        EXPECT_EQ(0x00, algorithm.MissedBeacons());
        EXPECT_EQ(ConnectionState::Connected, algorithm.State());
    }

    for (auto i = 0; i < 5; i++)
    {
        EXPECT_EQ(ConnectionState::Connected, algorithm.State());
        algorithm.Tick(BeaconState::NoBeaconReceived);
        EXPECT_EQ(i + 1, algorithm.MissedBeacons());
        EXPECT_EQ(ConnectionState::Connected, algorithm.State());
    }

    for (auto i = 0; i < 5; i++)
    {
        algorithm.Tick(BeaconState::BeaconReceived);
        EXPECT_EQ(0x00, algorithm.MissedBeacons());
        EXPECT_EQ(ConnectionState::Connected, algorithm.State());
    }
}

TEST(DebuggerTests_ConnectionAvailabilityAlgorithm, CustomFailureLimit) 
{
    ConnectionAvailability algorithm(80);
    algorithm.EstablishConnection();
    EXPECT_EQ(ConnectionState::Connected, algorithm.State());

    for (auto i = 0; i < 80; i++)
    {
        algorithm.Tick(BeaconState::NoBeaconReceived);
        
        if (i != 79)
        {
            EXPECT_EQ(i + 1, algorithm.MissedBeacons());
            EXPECT_EQ(ConnectionState::Connected, algorithm.State());
        }
        else
        {    
            EXPECT_EQ(0, algorithm.MissedBeacons());
            EXPECT_EQ(ConnectionState::NotConnected, algorithm.State());
        }
    }
}