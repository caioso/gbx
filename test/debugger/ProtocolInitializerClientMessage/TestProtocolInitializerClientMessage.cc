#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <memory>
#include <random>

#include "MessageID.h"
#include "ClientMessageHandler.h"
#include "GBXEmulatorExceptions.h"
#include "RegisterBank.h"

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

shared_ptr<array<uint8_t, MaxMessageBufferSize>> CreateProtocolInitializerClientMessage()
{
    auto buffer = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*buffer)[0] = MessageID::MessageProtocolInitializer & 0xFF;
    (*buffer)[1] = (MessageID::MessageProtocolInitializer >> 0x08) & 0xFF;
    return buffer;
}

TEST(DebuggerTests_ProtocolInitializerClientMessage, SendProtocolInitializerMessage) 
{
    OutputDriverMock outputMock; 
    auto transportMock = make_shared<ClientTransportMock>();
    auto messageHandler = make_shared<ClientMessageHandler>(static_pointer_cast<ClientTransport>(transportMock), outputMock);

    EXPECT_CALL((*transportMock), Subscribe(::_)).Times(1);
    EXPECT_CALL((*transportMock), JoinServer()).Times(1);
    messageHandler->Initialize();
    
    // Send Protocol Initializer Message. The message content is dependent on the concrete implementation of the transport.
    auto message = CreateProtocolInitializerClientMessage();
    auto notificationArguments = make_shared<DebugMessageNotificationArguments>(make_shared<DebugMessage>(message));
    auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);

    EXPECT_CALL((*transportMock), InitializeProtocol(::_)).WillOnce(testing::Invoke([&](std::shared_ptr<std::array<uint8_t, gbxdb::interfaces::MaxMessageBufferSize>> registerBankBytes)
    {
        uint16_t messageId = (*(registerBankBytes))[0] | (*(registerBankBytes))[1] << 0x08;
        EXPECT_EQ(MessageID::MessageProtocolInitializer, messageId);

        for (auto i = 2llu; i < 256llu; ++i)
            EXPECT_EQ(static_cast<uint8_t>(0), (*(registerBankBytes))[i]);
    }));
    messageHandler->Notify(argumentsPointer);
}