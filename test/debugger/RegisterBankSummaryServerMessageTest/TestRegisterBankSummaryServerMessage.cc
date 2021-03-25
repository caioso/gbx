#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <random>

#include "DebuggableRunner.h"
#include "DebugMessageNotificationArguments.h"
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

shared_ptr<array<uint8_t, MaxMessageBufferSize>> CreateRegisterBankSummaryMessage()
{
    auto buffer = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*buffer)[0] = 0xFD;
    (*buffer)[1] = 0xFF;
    return buffer;
}

TEST(DebuggerTests_RegisterBankSummaryServerMessage, RequestRegisterBankSummary) 
{
    auto transportMock = make_unique<ServerTransportMock>();
    auto transportPointer = transportMock.get();
    auto runtimeMock = make_shared<RuntimeMock>();
    auto messageHandler = make_shared<ServerMessageHandler>(std::move(transportMock));
    auto runnerMock = make_shared<DebuggableRunnerMock>();

    auto readRegisterBankMessage = make_shared<DebugMessage>(CreateRegisterBankSummaryMessage());
    auto notificationArguments = make_shared<DebugMessageNotificationArguments>(readRegisterBankMessage);
    auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);

    EXPECT_CALL((*transportPointer), Subscribe(::_)).Times(1);
    EXPECT_CALL((*transportPointer), WaitForClient()).Times(1);
    messageHandler->Initialize();

    messageHandler->Notify(argumentsPointer);
    EXPECT_EQ(1llu, messageHandler->Pending());

    // Expect call to Runtime Read Register Method
    EXPECT_CALL((*runtimeMock), ReadRegister(Register::B)).WillOnce(Return(static_cast<uint8_t>(0x01)));
    EXPECT_CALL((*runtimeMock), ReadRegister(Register::C)).WillOnce(Return(static_cast<uint8_t>(0x02)));
    EXPECT_CALL((*runtimeMock), ReadRegister(Register::D)).WillOnce(Return(static_cast<uint8_t>(0x03)));
    EXPECT_CALL((*runtimeMock), ReadRegister(Register::E)).WillOnce(Return(static_cast<uint8_t>(0x04)));
    EXPECT_CALL((*runtimeMock), ReadRegister(Register::H)).WillOnce(Return(static_cast<uint8_t>(0x05)));
    EXPECT_CALL((*runtimeMock), ReadRegister(Register::L)).WillOnce(Return(static_cast<uint8_t>(0x06)));
    EXPECT_CALL((*runtimeMock), ReadRegister(Register::A)).WillOnce(Return(static_cast<uint8_t>(0x07)));
    EXPECT_CALL((*runtimeMock), ReadRegister(Register::F)).WillOnce(Return(static_cast<uint8_t>(0x08)));
    EXPECT_CALL((*runtimeMock), ReadRegister(Register::IR)).WillRepeatedly(Return(static_cast<uint8_t>(0x09)));
    EXPECT_CALL((*runtimeMock), ReadRegister(Register::PIR)).WillRepeatedly(Return(static_cast<uint8_t>(0x10)));
    EXPECT_CALL((*runtimeMock), ReadRegister(Register::PC)).WillRepeatedly(Return(static_cast<uint16_t>(0xCCC0)));
    EXPECT_CALL((*runtimeMock), ReadRegister(Register::SP)).WillRepeatedly(Return(static_cast<uint16_t>(0xDDD0)));
    EXPECT_CALL((*transportPointer), SendMessage(::_)).WillOnce(testing::Invoke([&](shared_ptr<DebugMessage> argument)
    {
        uint16_t messageId = (*(argument->Buffer()))[0] | (*(argument->Buffer()))[1] << 0x08;
        uint16_t registerB = (*(argument->Buffer()))[2];
        uint16_t registerC = (*(argument->Buffer()))[3];
        uint16_t registerD = (*(argument->Buffer()))[4];
        uint16_t registerE = (*(argument->Buffer()))[5];
        uint16_t registerH = (*(argument->Buffer()))[6];
        uint16_t registerL = (*(argument->Buffer()))[7];
        uint16_t registerA = (*(argument->Buffer()))[8];
        uint16_t registerF = (*(argument->Buffer()))[9];
        uint16_t registerIR = (*(argument->Buffer()))[10];
        uint16_t registerPIR= (*(argument->Buffer()))[11];
        uint16_t registerPC = (*(argument->Buffer()))[12] | (*(argument->Buffer()))[13] << 0x08;
        uint16_t registerSP = (*(argument->Buffer()))[14] | (*(argument->Buffer()))[15] << 0x08;

        EXPECT_EQ(MessageID::MessageRegisterBankSummary, messageId);
        EXPECT_EQ(registerB, 0x01);
        EXPECT_EQ(registerC, 0x02);
        EXPECT_EQ(registerD, 0x03);
        EXPECT_EQ(registerE, 0x04);
        EXPECT_EQ(registerH, 0x05);
        EXPECT_EQ(registerL, 0x06);
        EXPECT_EQ(registerA, 0x07);
        EXPECT_EQ(registerF, 0x08);
        EXPECT_EQ(registerIR, 0x09);
        EXPECT_EQ(registerPIR, 0x10);
        EXPECT_EQ(registerPC, 0xCCC0);
        EXPECT_EQ(registerSP, 0xDDD0);
    }));

    messageHandler->ProcessMessages(runtimeMock, runnerMock);
    EXPECT_EQ(0llu, messageHandler->Pending());
}