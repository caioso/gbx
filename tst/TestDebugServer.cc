#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <chrono>
#include <iostream>
#include <limits>
#include <memory>
#include <string> 
#include <thread> 
#include <variant> 

#include "../src/interfaces/CommunicationChannel.h"
#include "../src/DebugServer.h"

using namespace std;
using namespace gbx::interfaces;
using namespace gbx;

using ::testing::Return;
using ::testing::_;

class CommunicationChannelMock : public CommunicationChannel
{
public:
    virtual ~CommunicationChannelMock() = default;
    MOCK_METHOD(void, Initialize, ());
    MOCK_METHOD(void, Send, (std::string));
    MOCK_METHOD(void, Receive, (std::string&));
};

TEST(TestDebugServer, Construction)
{
    auto communicationChannel = make_shared<CommunicationChannelMock>();
    auto debugServer = make_shared<DebugServer>(communicationChannel);
}

TEST(TestDebugServer, InitializeServer)
{
    auto communicationChannel = make_shared<CommunicationChannelMock>();
    auto debugServer = make_shared<DebugServer>(communicationChannel);

    debugServer->Initialize()
}