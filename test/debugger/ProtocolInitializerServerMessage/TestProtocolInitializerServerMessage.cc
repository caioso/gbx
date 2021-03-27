#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <random>

#include "DebuggableRunner.h"
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

shared_ptr<array<uint8_t, MaxMessageBufferSize>> CreateProtocolInitializerMessage()
{
    auto buffer = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*buffer)[0] = MessageID::MessageProtocolInitializer & 0xFF;
    (*buffer)[1] = (MessageID::MessageProtocolInitializer >> 0x08) & 0xFF;
    return buffer;
}

TEST(DebuggerTests_ProtocolInitializerServerMessage, SendProtocolInitializerMessage) 
{
    auto transportMock = make_unique<ServerTransportMock>();
    auto transportPointer = transportMock.get();
    auto runtimeMock = make_shared<RuntimeMock>();
    auto messageHandler = make_shared<ServerMessageHandler>(std::move(transportMock));
    auto runnerMock = make_shared<DebuggableRunnerMock>();

    auto initialzierMessage = make_shared<DebugMessage>(CreateProtocolInitializerMessage());
    auto notificationArguments = make_shared<DebugMessageNotificationArguments>(initialzierMessage);
    auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);

    EXPECT_CALL((*transportPointer), Subscribe(::_)).Times(1);
    EXPECT_CALL((*transportPointer), WaitForClient()).Times(1);
    EXPECT_CALL((*transportPointer), InitializeProtocol()).Times(1);
    messageHandler->Initialize();

    messageHandler->Notify(argumentsPointer);
    EXPECT_EQ(1llu, messageHandler->Pending());

    // The content of the Initializer message is freely defined by the concrete implementation of the transports
    EXPECT_CALL((*transportPointer), SendMessage(::_)).WillOnce(testing::Invoke([&](shared_ptr<DebugMessage> argument)
    {
        uint16_t messageId = (*(argument->Buffer()))[0] | (*(argument->Buffer()))[1] << 0x08;
        EXPECT_EQ(MessageID::MessageProtocolInitializer, messageId);
    }));

    messageHandler->ProcessMessages(runtimeMock, runnerMock);
    EXPECT_EQ(0llu, messageHandler->Pending());
}