#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <memory>
#include <random>


#include "ClientMessageHandler.h"
#include "MessageID.h"
#include "DebugMessageNotificationArguments.h"
#include "RegisterBank.h"

#include "DebuggerTestMocks.h"
#include "TestUtils.h"

using namespace gbxcore;
using namespace gbxcommons;
using namespace gbxcore::interfaces;
using namespace gbxdb;
using namespace gbxdb::interfaces;
using namespace gbxdb::protocol;
using namespace std;

using ::testing::Return;
using ::testing::_;

std::shared_ptr<std::array<uint8_t, MaxMessageBufferSize>> CreateJoinedServerMessage()
{
    auto message = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*message)[0] = MessageID::MessageJoined & 0xFF;
    (*message)[1] = (MessageID::MessageJoined >> 0x08) & 0xFF;

    return message;
}

TEST(DebuggerTests_ClientMessageHandler, Construction) 
{
    OutputDriverMock outputMock;
    auto transportMock = make_shared<ClientTransportMock>();
    auto messageHandler = make_shared<ClientMessageHandler>(static_pointer_cast<ClientTransport>(transportMock), outputMock);
}

TEST(DebuggerTests_ClientMessageHandler, InitializationTest) 
{
    OutputDriverMock outputMock;
    auto transportMock = make_shared<ClientTransportMock>();
    auto messageHandler = make_shared<ClientMessageHandler>(static_pointer_cast<ClientTransport>(transportMock), outputMock);

    EXPECT_CALL((*transportMock), Subscribe(::_)).Times(1);
    EXPECT_CALL((*transportMock), JoinServer()).Times(1);
    messageHandler->Initialize();
}

TEST(DebuggerTests_ClientMessageHandler, JoinServerMessage) 
{
    OutputDriverMock outputMock;
    auto transportMock = make_shared<ClientTransportMock>();
    auto messageHandler = make_shared<ClientMessageHandler>(static_pointer_cast<ClientTransport>(transportMock), outputMock);

    auto clientJoinedMessage = make_shared<DebugMessage>(CreateJoinedServerMessage());
    auto notificationArguments = make_shared<DebugMessageNotificationArguments>(clientJoinedMessage);
    auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);

    EXPECT_CALL((*transportMock), Subscribe(::_)).Times(1);
    EXPECT_CALL((*transportMock), JoinServer()).Times(1);
    messageHandler->Initialize();

    messageHandler->Notify(argumentsPointer);
    EXPECT_TRUE(messageHandler->IsConnected());
}
