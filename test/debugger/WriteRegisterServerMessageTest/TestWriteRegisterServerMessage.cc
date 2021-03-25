#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <random>
#include <variant>

#include "DebugMessageNotificationArguments.h"
#include "ErrorID.h"
#include "GBXEmulatorExceptions.h"
#include "MessageID.h"
#include "ServerMessageHandler.h"

#include "TestMocks.h"
#include "TestUtils.h"

using namespace gbx;
using namespace gbxdb; 
using namespace gbxdb::interfaces; 
using namespace gbxdb::protocol;
using namespace gbxcore::interfaces;
using namespace gbxcommons;
using namespace std;

using ::testing::Return;
using ::testing::_;

shared_ptr<array<uint8_t, MaxMessageBufferSize>> CreateWriteRegisterMessage(uint8_t targetRegister, uint16_t registerValue)
{
    auto buffer = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*buffer)[0] = 0xFC;
    (*buffer)[1] = 0xFF;
    (*buffer)[2] = targetRegister;
    (*buffer)[3] = registerValue & 0xFF;
    (*buffer)[4] = (registerValue >> 0x08) & 0xFF;
    return buffer;
}

TEST(DebuggerTests_WriteRegisterServerMessage, Write8bitRegisterWithRandomValue) 
{
    auto operand8BitList = {Register::B, Register::C, Register::D, Register::E, Register::H, Register::L, Register::A, Register::F };
    auto runtimeMock = make_shared<RuntimeMock>();
    auto transportMock = make_unique<ServerTransportMock>();
    auto transportPointer = transportMock.get();
    auto messageHandler = make_shared<ServerMessageHandler>(std::move(transportMock));
    auto runnerMock = make_shared<DebuggableRunnerMock>();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0x00, 0xFF};

    for (auto operand : operand8BitList)
    {
        auto registerValue = distribution(engine);
        auto readRegisterBankMessage = make_shared<DebugMessage>(CreateWriteRegisterMessage(static_cast<uint8_t>(operand), static_cast<uint16_t>(registerValue)));
        auto notificationArguments = make_shared<DebugMessageNotificationArguments>(readRegisterBankMessage);
        auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);

        EXPECT_CALL((*transportPointer), Subscribe(::_)).Times(1);
        EXPECT_CALL((*transportPointer), WaitForClient()).Times(1);
        messageHandler->Initialize();

        // Simulate message arriving from the client (stored in the internal queue)
        messageHandler->Notify(argumentsPointer);
        EXPECT_EQ(1llu, messageHandler->Pending());

        // Expect call to Runtime Read Register Method
        variant<uint8_t, uint16_t> value = registerValue;
        EXPECT_CALL((*runtimeMock), WriteRegister(operand, value));
        EXPECT_CALL((*transportPointer), SendMessage(::_)).WillOnce(testing::Invoke([&](shared_ptr<DebugMessage> argument)
        {
            uint16_t messageId = (*(argument->Buffer()))[0] | (*(argument->Buffer()))[1] << 0x08;
            // Target register
            uint16_t targetRegister = (*(argument->Buffer()))[2];
            // Written value
            uint16_t targetValue = (*(argument->Buffer()))[3] | ((*(argument->Buffer()))[4] << 0x08);

            EXPECT_EQ(MessageID::MessageWriteRegister, messageId);
            EXPECT_EQ(targetRegister, static_cast<uint8_t>(operand));
            EXPECT_EQ(targetValue, static_cast<uint16_t>(registerValue));
        }));

        // Take message from the queue and process it
        messageHandler->ProcessMessages(runtimeMock, runnerMock);
        EXPECT_EQ(0llu, messageHandler->Pending());
    }
}

TEST(DebuggerTests_WriteRegisterServerMessage, Write16bitRegisterWithRandomValue) 
{
    auto operand16BitList = {Register::PC, Register::SP, Register::HL, Register::BC, Register::DE, Register::AF};
    auto runtimeMock = make_shared<RuntimeMock>();
    auto transportMock = make_unique<ServerTransportMock>();
    auto transportPointer = transportMock.get();
    auto messageHandler = make_shared<ServerMessageHandler>(std::move(transportMock));
    auto runnerMock = make_shared<DebuggableRunnerMock>();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint16_t> distribution{0x00, 0xFFFF};

    for (auto operand : operand16BitList)
    {
        auto registerValue = distribution(engine);
        auto readRegisterBankMessage = make_shared<DebugMessage>(CreateWriteRegisterMessage(static_cast<uint8_t>(operand), registerValue));
        auto notificationArguments = make_shared<DebugMessageNotificationArguments>(readRegisterBankMessage);
        auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);

        EXPECT_CALL((*transportPointer), Subscribe(::_)).Times(1);
        EXPECT_CALL((*transportPointer), WaitForClient()).Times(1);
        messageHandler->Initialize();

        // Simulate message arriving from the client (stored in the internal queue)
        messageHandler->Notify(argumentsPointer);
        EXPECT_EQ(1llu, messageHandler->Pending());

        // Expect call to Runtime Read Register Method
        variant<uint8_t, uint16_t> value = static_cast<uint16_t>(registerValue);
        EXPECT_CALL((*runtimeMock), WriteRegister(operand, value));
        EXPECT_CALL((*transportPointer), SendMessage(::_)).WillOnce(testing::Invoke([&](shared_ptr<DebugMessage> argument)
        {
            uint16_t messageId = (*(argument->Buffer()))[0] | (*(argument->Buffer()))[1] << 0x08;
            // Target register
            uint16_t targetRegister = (*(argument->Buffer()))[2];
            // Written value
            uint16_t targetValue = (*(argument->Buffer()))[3] | ((*(argument->Buffer()))[4] << 0x08);

            EXPECT_EQ(MessageID::MessageWriteRegister, messageId);
            EXPECT_EQ(targetRegister, static_cast<uint8_t>(operand));
            EXPECT_EQ(targetValue, registerValue);
        }));

        // Take message from the queue and process it
        messageHandler->ProcessMessages(runtimeMock, runnerMock);
        EXPECT_EQ(0llu, messageHandler->Pending());
    }
}

TEST(DebuggerTests_WriteRegisterServerMessage, WriteInvalidRegisterAddress) 
{
    auto runtimeMock = make_shared<RuntimeMock>();
    auto transportMock = make_unique<ServerTransportMock>();
    auto transportPointer = transportMock.get();
    auto messageHandler = make_shared<ServerMessageHandler>(std::move(transportMock));
    auto runnerMock = make_shared<DebuggableRunnerMock>();

    auto readRegisterBankMessage = make_shared<DebugMessage>(CreateWriteRegisterMessage(0xFF, 0x1111));
    auto notificationArguments = make_shared<DebugMessageNotificationArguments>(readRegisterBankMessage);
    auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);

    EXPECT_CALL((*transportPointer), Subscribe(::_)).Times(1);
    EXPECT_CALL((*transportPointer), WaitForClient()).Times(1);
    messageHandler->Initialize();

    // Simulate message arriving from the client (stored in the internal queue)
    // Note that here, since the parsing has an error, the command in the queue is an ErrorCommand
    messageHandler->Notify(argumentsPointer);
    EXPECT_EQ(1llu, messageHandler->Pending());

    // Expect call to Runtime Read Register Method
    EXPECT_CALL((*transportPointer), SendMessage(::_)).WillOnce(testing::Invoke([&](shared_ptr<DebugMessage> argument)
    {
        uint16_t messageId = (*(argument->Buffer()))[0] | (*(argument->Buffer()))[1] << 0x08;
        // Error Code
        uint16_t targetError = (*(argument->Buffer()))[2] | (*(argument->Buffer()))[3] << 0x08;
      
        EXPECT_EQ(MessageID::MessageError, messageId);
        EXPECT_EQ(ErrorID::InvalidRegister, targetError);
    }));

    // Take message from the queue and process it
    messageHandler->ProcessMessages(runtimeMock, runnerMock);
    EXPECT_EQ(0llu, messageHandler->Pending());
}