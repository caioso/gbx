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
    EXPECT_EQ(0x00llu, algorithm.MissedBeacons());
    EXPECT_EQ(ConnectionState::Connected, algorithm.State());
}

TEST(DebuggerTests_ConnectionAvailabilityAlgorithm, MissOneTick) 
{
    ConnectionAvailability algorithm(nullopt);
    algorithm.EstablishConnection();
    EXPECT_EQ(ConnectionState::Connected, algorithm.State());

    algorithm.Tick(BeaconState::NoBeaconReceived);
    EXPECT_EQ(0x01llu, algorithm.MissedBeacons());
    EXPECT_EQ(ConnectionState::Connected, algorithm.State());
    
    algorithm.Tick(BeaconState::BeaconReceived);
    EXPECT_EQ(0x00llu, algorithm.MissedBeacons());
    EXPECT_EQ(ConnectionState::Connected, algorithm.State());
}

TEST(DebuggerTests_ConnectionAvailabilityAlgorithm, MissLimitTick) 
{
    ConnectionAvailability algorithm(nullopt);
    algorithm.EstablishConnection();
    EXPECT_EQ(ConnectionState::Connected, algorithm.State());

    for (size_t i = 0; i < 10llu; i++)
    {
        algorithm.Tick(BeaconState::NoBeaconReceived);
        
        if (i != 9)
        {
            EXPECT_EQ(i + 1, algorithm.MissedBeacons());
            EXPECT_EQ(ConnectionState::Connected, algorithm.State());
        }
        else
        {    
            EXPECT_EQ(0x00llu, algorithm.MissedBeacons());
            EXPECT_EQ(ConnectionState::NotConnected, algorithm.State());
        }
    }
}

TEST(DebuggerTests_ConnectionAvailabilityAlgorithm, RestoreCommunicationAfterAFewFailures) 
{
    ConnectionAvailability algorithm(nullopt);
    algorithm.EstablishConnection();

    for (auto i = 0llu; i < 5llu; i++)
    {
        algorithm.Tick(BeaconState::BeaconReceived);
        EXPECT_EQ(0x00llu, algorithm.MissedBeacons());
        EXPECT_EQ(ConnectionState::Connected, algorithm.State());
    }

    for (auto i = 0llu; i < 5llu; i++)
    {
        EXPECT_EQ(ConnectionState::Connected, algorithm.State());
        algorithm.Tick(BeaconState::NoBeaconReceived);
        EXPECT_EQ(i + 1, algorithm.MissedBeacons());
        EXPECT_EQ(ConnectionState::Connected, algorithm.State());
    }

    for (auto i = 0llu; i < 5llu; i++)
    {
        algorithm.Tick(BeaconState::BeaconReceived);
        EXPECT_EQ(0x00llu, algorithm.MissedBeacons());
        EXPECT_EQ(ConnectionState::Connected, algorithm.State());
    }
}

TEST(DebuggerTests_ConnectionAvailabilityAlgorithm, CustomFailureLimit) 
{
    ConnectionAvailability algorithm(80llu);
    algorithm.EstablishConnection();
    EXPECT_EQ(ConnectionState::Connected, algorithm.State());

    for (auto i = 0llu; i < 80llu; i++)
    {
        algorithm.Tick(BeaconState::NoBeaconReceived);
        
        if (i != 79llu)
        {
            EXPECT_EQ(i + 1, algorithm.MissedBeacons());
            EXPECT_EQ(ConnectionState::Connected, algorithm.State());
        }
        else
        {    
            EXPECT_EQ(0x00llu, algorithm.MissedBeacons());
            EXPECT_EQ(ConnectionState::NotConnected, algorithm.State());
        }
    }
}