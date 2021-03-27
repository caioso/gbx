#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <random>

#include "DebugMessageNotificationArguments.h"
#include "GBXEmulatorExceptions.h"
#include "MessageID.h"
#include "ServerMessageHandler.h"

#include "DebuggerTestMocks.h"
#include "TestUtils.h"

using namespace gbxruntime;
using namespace gbxdb;
using namespace gbxdb::interfaces;
using namespace gbxdb::protocol;
using namespace gbxcore::interfaces;
using namespace gbxcommons;
using namespace std;

using ::testing::Return;
using ::testing::_;

shared_ptr<array<uint8_t, MaxMessageBufferSize>> CreateDummyMessage(uint16_t type)
{
    auto buffer = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*buffer)[0] = type & 0x00;
    (*buffer)[1] = (type >> 8) & 0x00;
    return buffer;
}

shared_ptr<array<uint8_t, MaxMessageBufferSize>> CreateClientJoinedMessage()
{
    auto buffer = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*buffer)[0] = 0xFF;
    (*buffer)[1] = 0xFF;
    return buffer;
}

TEST(DebuggerTests_ServerMessageHandler, Construction) 
{
    auto transportMock = make_unique<ServerTransportMock>();
    auto messageHandler = make_shared<ServerMessageHandler>(std::move(transportMock));
}

TEST(ServerTestMessagHandler, Initialize) 
{
    auto transportMock = make_unique<ServerTransportMock>();
    auto transportPointer = transportMock.get();
    auto messageHandler = make_shared<ServerMessageHandler>(std::move(transportMock));

    EXPECT_CALL((*transportPointer), Subscribe(::_)).Times(1);
    EXPECT_CALL((*transportPointer), WaitForClient()).Times(1);
    EXPECT_CALL((*transportPointer), InitializeProtocol()).Times(1);
    messageHandler->Initialize();
}

TEST(ServerTestMessagHandler, DecodeReceivedUnknownMessage) 
{
    auto transportMock = make_unique<ServerTransportMock>();
    auto transportPointer = transportMock.get();
    auto messageHandler = make_shared<ServerMessageHandler>(std::move(transportMock));
    
    auto dummyMessage = make_shared<DebugMessage>(CreateDummyMessage(0x0000));
    auto notificationArguments = make_shared<DebugMessageNotificationArguments>(dummyMessage);
    auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);

    EXPECT_CALL((*transportPointer), Subscribe(::_)).Times(1);
    EXPECT_CALL((*transportPointer), WaitForClient()).Times(1);
    EXPECT_CALL((*transportPointer), InitializeProtocol()).Times(1);
    messageHandler->Initialize();

    ASSERT_EXCEPTION( { messageHandler->Notify(argumentsPointer); }, 
                      MessageHandlerException, 
                      "Invalid debug message recieved and will be ignored");
}

TEST(ServerTestMessagHandler, DecodeClientJoinedMessage) 
{
    auto runtimeMock = make_shared<RuntimeMock>();
    auto transportMock = make_unique<ServerTransportMock>();
    auto transportPointer = transportMock.get();
    auto messageHandler = make_shared<ServerMessageHandler>(std::move(transportMock));
    auto runnerMock = make_shared<DebuggableRunnerMock>();
    
    auto clientJoinedMessage = make_shared<DebugMessage>(CreateClientJoinedMessage());
    auto notificationArguments = make_shared<DebugMessageNotificationArguments>(clientJoinedMessage);
    auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);

    EXPECT_CALL((*transportPointer), Subscribe(::_)).Times(1);
    EXPECT_CALL((*transportPointer), WaitForClient()).Times(1);
    EXPECT_CALL((*transportPointer), InitializeProtocol()).Times(1);
    messageHandler->Initialize();

    messageHandler->Notify(argumentsPointer);
    EXPECT_EQ(1llu, messageHandler->Pending());

    // Expect call to Runtime Read Register Method
    EXPECT_CALL((*runnerMock), ClientJoined()).Times(1);
    messageHandler->ProcessMessages(runtimeMock, runnerMock);
    EXPECT_EQ(0llu, messageHandler->Pending());
}
