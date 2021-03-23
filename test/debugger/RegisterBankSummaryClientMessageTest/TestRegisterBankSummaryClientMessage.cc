#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <memory>
#include <random>

#include "MessageID.h"
#include "ClientMessageHandler.h"
#include "ClientTransport.h"
#include "DebuggableRunner.h"
#include "DebugMessage.h"
#include "DebugMessageNotificationArguments.h"
#include "GBXEmulatorExceptions.h"
#include "OutputDriver.h"
#include "RegisterBank.h"

#include "TestUtils.h"

using namespace gbx;
using namespace gbxcommons;
using namespace gbxcore;
using namespace gbxdb;
using namespace gbxdb::interfaces;
using namespace gbxdb::protocol;
using namespace std;

using ::testing::Return;
using ::testing::_;

class TransportMock : public ClientTransport
{
public:
    virtual ~TransportMock() = default;
    MOCK_METHOD(void, JoinServer, ());
    MOCK_METHOD(void, LeaveServer, ());
    MOCK_METHOD(void, SendMessage, (shared_ptr<DebugMessage>));
    MOCK_METHOD(void, Subscribe, (weak_ptr<Observer>));
    MOCK_METHOD(void, Unsubscribe, (weak_ptr<Observer>));
};

class OutputDriverMock : public OutputDriver
{
public:
    virtual ~OutputDriverMock() = default;
    MOCK_METHOD(void, DisplayRegisterbank, ((array<uint8_t, RegisterBankSizeInBytes>)));
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
    OutputDriverMock outputMock; 
    auto transportMock = make_shared<TransportMock>();
    auto messageHandler = make_shared<ClientMessageHandler>(static_pointer_cast<ClientTransport>(transportMock), outputMock);

    EXPECT_CALL((*transportMock), Subscribe(::_)).Times(1);
    EXPECT_CALL((*transportMock), JoinServer()).Times(1);
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

    EXPECT_CALL(outputMock, DisplayRegisterbank(::_)).WillOnce(testing::Invoke([&](std::array<uint8_t, gbxcore::RegisterBankSizeInBytes> registerBankBytes)
    {
        EXPECT_EQ(0xA1, registerBankBytes[0]);
        EXPECT_EQ(0xA2, registerBankBytes[1]);
        EXPECT_EQ(0xA3, registerBankBytes[2]);
        EXPECT_EQ(0xA4, registerBankBytes[3]);
        EXPECT_EQ(0xA5, registerBankBytes[4]);
        EXPECT_EQ(0xA6, registerBankBytes[5]);
        EXPECT_EQ(0xA7, registerBankBytes[6]);
        EXPECT_EQ(0xA8, registerBankBytes[7]);
        EXPECT_EQ(0xA9, registerBankBytes[8]);
        EXPECT_EQ(0xAA, registerBankBytes[9]);
        EXPECT_EQ(0xFA, registerBankBytes[10]);
        EXPECT_EQ(0xFB, registerBankBytes[11]);
        EXPECT_EQ(0xFD, registerBankBytes[12]);
        EXPECT_EQ(0xFE, registerBankBytes[13]);
        EXPECT_EQ(0x00, registerBankBytes[14]);
        EXPECT_EQ(0x00, registerBankBytes[15]);
        EXPECT_EQ(0x00, registerBankBytes[16]);
        EXPECT_EQ(0x00, registerBankBytes[17]);
        EXPECT_EQ(0x00, registerBankBytes[18]);
        EXPECT_EQ(0x00, registerBankBytes[19]);
    }));
    messageHandler->Notify(argumentsPointer);
}
