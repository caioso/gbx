#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <chrono>
#include <thread>
#include <memory>

#include "TestUtils.h"

#include "GameBoyX.h"
#include "GBXCoreExceptions.h"

using namespace std;
using namespace gbxcore;

std::string SampleGameFileName()
{
    return GBXTestEnvironment::TestDataPath + "sample.bin";
}

TEST(CoreTests_GameBoyXTests, Construction) 
{
    GameBoyX gbx;    
}

TEST(CoreTests_GameBoyXTests, LoadGame) 
{
    GameBoyX gbx;
    gbx.LoadGame(SampleGameFileName());
}
