#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <random>

#include "DebuggableRunner.h"
#include "DebugMessage.h"
#include "ServerTransport.h"
#include "MessageID.h"
#include "DebugMessageNotificationArguments.h"
#include "MessageHandler.h"
#include "GBXEmulatorExceptions.h"
#include "Runtime.h"

#include "TestUtils.h"

using namespace gbx;
using namespace gbx::runtime;
using namespace gbxdb;
using namespace gbxdb::interfaces;
using namespace gbxdb::protocol;
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

class RunnerMock : public interfaces::DebuggableRunner
{
public:
    virtual ~RunnerMock() = default;
    MOCK_METHOD(void, ClientJoined, ());
    MOCK_METHOD(void, ClientLeft, ());
};

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

TEST(TestMessagHandler, DecodeClientJoinedMessage) 
{
    auto transportMock = make_shared<TransportMock>();
    auto runtimeMock = make_shared<RuntimeMock>();
    auto messageHandler = make_shared<MessageHandler>(static_pointer_cast<ServerTransport>(transportMock));
    auto runnerMock = make_shared<RunnerMock>();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0x00, 0xFF};

    auto clientJoinedMessage = make_shared<DebugMessage>(CreateClientJoinedMessage());
    auto notificationArguments = make_shared<DebugMessageNotificationArguments>(clientJoinedMessage);
    auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);

    EXPECT_CALL((*transportMock), Subscribe(::_)).Times(1);
    messageHandler->Initialize();

    messageHandler->Notify(argumentsPointer);
    EXPECT_EQ(1llu, messageHandler->Pending());

    // Expect call to Runtime Read Register Method
    EXPECT_CALL((*runnerMock), ClientJoined()).Times(1);
    messageHandler->ProcessMessages(runtimeMock, runnerMock);
    EXPECT_EQ(0llu, messageHandler->Pending());
}
