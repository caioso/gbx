#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <chrono>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <string> 
#include <thread> 
#include <variant> 

#include "../src/interfaces/CommunicationChannel.h"
#include "../src/interfaces/ServerStrategy.h"
#include "../src/interfaces/StrategyParameters.h"
#include "../src/DebugServer.h"
#include "interfaces/Runtime.h"

using namespace std;
using namespace gbxcore::interfaces;
using namespace gbxcore;
using namespace gbx::interfaces;
using namespace gbx;

using ::testing::Return;
using ::testing::_;

class RuntimeMock : public Runtime
{
public:
    virtual ~RuntimeMock() = default;
    MOCK_METHOD(void, Run, ());
    MOCK_METHOD((std::variant<uint8_t, uint16_t>), ReadRegister, (Register));
    MOCK_METHOD(void, WriteRegister, (Register, (std::variant<uint8_t, uint16_t>)));
};

class ServerStrategyMock : public ServerStrategy
{
public:
    virtual ~ServerStrategyMock() = default;
    MOCK_METHOD(void, Initialize, (StrategyParameters));
    MOCK_METHOD(void, WaitForClient, ());
};

TEST(TestDebugServer, InitializeServer)
{
    StrategyParameters strategyParameters;
    auto runtime = make_shared<RuntimeMock>();
    auto strategy = make_shared<ServerStrategyMock>();
    auto debugServer = make_shared<DebugServer>(static_pointer_cast<Runtime>(runtime),
                                                static_pointer_cast<ServerStrategy>(strategy));
    
    EXPECT_CALL((*strategy), Initialize(::_));
    debugServer->Initialize(strategyParameters);
}

TEST(TestDebugServer, WaitForClientToClient)
{
    StrategyParameters strategyParameters;
    auto runtime = make_shared<RuntimeMock>();
    auto strategy = make_shared<ServerStrategyMock>();
    auto debugServer = make_shared<DebugServer>(static_pointer_cast<Runtime>(runtime),
                                                static_pointer_cast<ServerStrategy>(strategy));
    
    EXPECT_CALL((*strategy), Initialize(::_));
    debugServer->Initialize(strategyParameters);

    EXPECT_CALL((*strategy), WaitForClient());
    debugServer->WaitForClient();
}

// TODO: ADD EVENTS TO THE STRATEGY, WHICH WILL INFORM WHEN THE CLIENT ARRIVED, AND WHEN MESSAGES HAVE BEEN RECEIVED
// THE CLIENT MUST RUN IN A SEPARATE THREAD