#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

#include "../src/interfaces/ServerTransport.h"
#include "../src/interfaces/DebugMessage.h"
#include "../src/protocol/MessageID.h"
#include "../src/runtime/DebugMessageNotificationArguments.h"
#include "../src/runtime/MessageHandler.h"
#include "../src/GBXExceptions.h"
#include "interfaces/Runtime.h"

#include "TestUtils.h"

using namespace gbx::runtime;
using namespace gbx::interfaces;
using namespace gbx::protocol;
using namespace gbx;
using namespace gbxcore::interfaces;
using namespace gbxcommons;
using namespace std;

using ::testing::Return;
using ::testing::_;

class TransportMock : public ServerTransport
{
public:
    virtual ~TransportMock() = default;
    MOCK_METHOD(void, WaitForClient, ());
    MOCK_METHOD(void, SendMessage, (shared_ptr<DebugMessage>));

    MOCK_METHOD(void, Subscribe, ((std::weak_ptr<Observer>)));
    MOCK_METHOD(void, Unsubscribe, ((std::weak_ptr<Observer>)));
};

class RuntimeMock : public Runtime
{
public:
    virtual ~RuntimeMock() = default;
    MOCK_METHOD(void, Run, ());
    MOCK_METHOD((std::variant<uint8_t, uint16_t>), ReadRegister, (Register));
    MOCK_METHOD(void, WriteRegister, (Register, (std::variant<uint8_t, uint16_t>)));
};

shared_ptr<array<uint8_t, MaxMessageBufferSize>> CreateDummyMessage(uint16_t type)
{
    auto buffer = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*buffer)[0] = type & 0xFF;
    (*buffer)[1] = (type >> 8) & 0xFF;
    return buffer;
}

shared_ptr<array<uint8_t, MaxMessageBufferSize>> CreateReadRegisterMessage(uint8_t targetRegister)
{
    auto buffer = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*buffer)[0] = 0xFE;
    (*buffer)[1] = 0xFF;
    (*buffer)[2] = targetRegister;
    return buffer;
}

TEST(TestMessagHandler, Construction) 
{
    auto transportMock = make_shared<TransportMock>();
    auto messageHandler = make_shared<MessageHandler>(static_pointer_cast<ServerTransport>(transportMock));
}

TEST(TestMessagHandler, Initialize) 
{
    auto transportMock = make_shared<TransportMock>();
    auto messageHandler = make_shared<MessageHandler>(static_pointer_cast<ServerTransport>(transportMock));

    EXPECT_CALL((*transportMock), Subscribe(::_)).Times(1);
    messageHandler->Initialize();
}

TEST(TestMessagHandler, DecodeReceivedUnknownMessage) 
{
    auto transportMock = make_shared<TransportMock>();
    auto messageHandler = make_shared<MessageHandler>(static_pointer_cast<ServerTransport>(transportMock));
    
    auto dummyMessage = make_shared<DebugMessage>(CreateDummyMessage(0x0000));
    auto notificationArguments = make_shared<DebugMessageNotificationArguments>(dummyMessage);
    auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);

    EXPECT_CALL((*transportMock), Subscribe(::_)).Times(1);
    messageHandler->Initialize();

    ASSERT_EXCEPTION( { messageHandler->Notify(argumentsPointer); }, 
                      MessageHandlerException, 
                      "Invalid debug message recieved and will be ignored");
}

TEST(TestMessagHandler, DecodeReadRegisterBankMessage) 
{
    auto operandList = {Register::B, Register::C, Register::D, Register::E, Register::H, Register::L,
                        Register::A, Register::F, Register::IR, Register::PIR, Register::R, Register::PC,
                        Register::SP, Register::IX, Register::IY,  Register::HL, Register::BC, Register::DE, Register::AF};

    auto transportMock = make_shared<TransportMock>();
    auto runtimeMock = make_shared<RuntimeMock>();
    auto messageHandler = make_shared<MessageHandler>(static_pointer_cast<ServerTransport>(transportMock));

    for (auto operand : operandList)
    {
        auto readRegisterBankMessage = make_shared<DebugMessage>(CreateReadRegisterMessage(static_cast<uint8_t>(operand)));
        auto notificationArguments = make_shared<DebugMessageNotificationArguments>(readRegisterBankMessage);
        auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);

        EXPECT_CALL((*transportMock), Subscribe(::_)).Times(1);
        messageHandler->Initialize();

        messageHandler->Notify(argumentsPointer);
        EXPECT_EQ(1llu, messageHandler->Pending());

        // Expect call to Runtime Read Register Method
        EXPECT_CALL((*runtimeMock), ReadRegister(operand)).WillOnce(Return(static_cast<uint16_t>(0x0000)));
        messageHandler->ProcessMessages(runtimeMock);
        EXPECT_EQ(0llu, messageHandler->Pending());
    }
}

TEST(TestMessagHandler, DecodeReadRegisterUnknownRegister) 
{
    auto transportMock = make_shared<TransportMock>();
    auto runtimeMock = make_shared<RuntimeMock>();
    auto messageHandler = make_shared<MessageHandler>(static_pointer_cast<ServerTransport>(transportMock));

    auto readRegisterBankMessage = make_shared<DebugMessage>(CreateReadRegisterMessage(0xFF));
    auto notificationArguments = make_shared<DebugMessageNotificationArguments>(readRegisterBankMessage);
    auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);

    EXPECT_CALL((*transportMock), Subscribe(::_)).Times(1);
    messageHandler->Initialize();

    ASSERT_EXCEPTION( { messageHandler->Notify(argumentsPointer); }, 
                    MessageHandlerException, 
                    "Invalid target register found when parsing 'ReadRegister' command");
}