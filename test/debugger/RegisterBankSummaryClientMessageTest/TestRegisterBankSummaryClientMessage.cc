#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <random>

#include "MessageID.h"
#include "ClientMessageHandler.h"
#include "ClientTransport.h"
#include "DebuggableRunner.h"
#include "DebugMessage.h"
#include "DebugMessageNotificationArguments.h"
#include "GBXEmulatorExceptions.h"

#include "TestUtils.h"

using namespace gbx;
using namespace gbxdb;
using namespace gbxdb::interfaces;
using namespace gbxdb::protocol;
using namespace gbxcommons;
using namespace std;

using ::testing::Return;
using ::testing::_;

class TransportMock : public ClientTransport
{
public:
    virtual ~TransportMock() = default;
    MOCK_METHOD(void, JoinServer, ());
    MOCK_METHOD(void, SendMessage, (shared_ptr<DebugMessage>));
    MOCK_METHOD(void, Subscribe, (weak_ptr<Observer>));
    MOCK_METHOD(void, Unsubscribe, (weak_ptr<Observer>));
};

shared_ptr<array<uint8_t, MaxMessageBufferSize>> CreateRegisterBankSummaryResponseMessage()
{
    auto buffer = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*buffer)[0] = static_cast<uint16_t>(MessageID::MessageRegisterBankSummary) & 0xFF;
    (*buffer)[1] = ((static_cast<uint16_t>(MessageID::MessageRegisterBankSummary)) >> 0x08) & 0xFF;
    (*buffer)[2] = 0xA1;
    (*buffer)[3] = 0xA2;
    (*buffer)[4] = 0xA3;
    (*buffer)[5] = 0xA4;
    (*buffer)[6] = 0xA5;
    (*buffer)[7] = 0xA6;
    (*buffer)[8] = 0xA7;
    (*buffer)[9] = 0xA8;
    (*buffer)[10] = 0xA9;
    (*buffer)[11] = 0xAA; 
    (*buffer)[12] = 0xFA; 
    (*buffer)[13] = 0xFB;
    (*buffer)[14] = 0xFD;
    (*buffer)[15] = 0xFE;
    return buffer;
}

shared_ptr<array<uint8_t, MaxMessageBufferSize>> CreateInvalidRegisterBankSummaryResponseMessage()
{
    auto buffer = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*buffer)[0] = 0x00;
    (*buffer)[1] = 0x01;
    (*buffer)[2] = 0xA1;
    (*buffer)[3] = 0xA2;
    (*buffer)[4] = 0xA3;
    (*buffer)[5] = 0xA4;
    (*buffer)[6] = 0xA5;
    (*buffer)[7] = 0xA6;
    (*buffer)[8] = 0xA7;
    (*buffer)[9] = 0xA8;
    (*buffer)[10] = 0xA9;
    (*buffer)[11] = 0xAA; 
    (*buffer)[12] = 0xFA; 
    (*buffer)[13] = 0xFB;
    (*buffer)[14] = 0xFD;
    (*buffer)[15] = 0xFE;
    return buffer;
}

TEST(DebuggerTests_RegisterBankSummaryClientMessage, RequestRegisterBankSummaryAndProcessResult) 
{
    auto transportMock = make_shared<TransportMock>();
    auto messageHandler = make_shared<ClientMessageHandler>(static_pointer_cast<ClientTransport>(transportMock));

    EXPECT_CALL((*transportMock), Subscribe(::_)).Times(1);
    messageHandler->Initialize();

    // Send RegisterBankSummary Message
    EXPECT_CALL((*transportMock), SendMessage(::_)).WillOnce(testing::Invoke([&](shared_ptr<DebugMessage> argument)
    {
        uint16_t messageId = (*(argument->Buffer()))[0] | (*(argument->Buffer()))[1] << 0x08;
        EXPECT_EQ(MessageID::MessageRegisterBankSummary, messageId);

        for (auto i = 2llu; i < 256llu; ++i)
            EXPECT_EQ(static_cast<uint8_t>(0), (*(argument->Buffer()))[i]);
    }));
    messageHandler->SendRegisterBankSummaryMessage();

    auto message = CreateRegisterBankSummaryResponseMessage();
    auto notificationArguments = make_shared<DebugMessageNotificationArguments>(make_shared<DebugMessage>(message));
    auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);
    messageHandler->Notify(argumentsPointer);

    // Proceed implementing an output class that can receive the register bank's byte commands
    // Then implement a concrete class that can show the results in the terminal
}
