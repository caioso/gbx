#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <memory>
#include <random>

#include "ClientTransport.h"
#include "ClientMessageHandler.h"
#include "ClientMessageID.h"
#include "DebugMessage.h"
#include "DebugMessageNotificationArguments.h"
#include "Observer.h"
#include "Runtime.h"

#include "TestUtils.h"

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
    (*message)[0] = ClientMessageID::JoinedServer & 0xFF;
    (*message)[1] = (ClientMessageID::JoinedServer >> 0x08) & 0xFF;

    return message;
}

class RuntimeMock : public Runtime
{
public:
    virtual ~RuntimeMock() = default;
    MOCK_METHOD(void, Run, ());
    MOCK_METHOD((std::variant<uint8_t, uint16_t>), ReadRegister, (Register));
    MOCK_METHOD(void, WriteRegister, (Register, (std::variant<uint8_t, uint16_t>)));
};

class TransportMock : public ClientTransport
{
public:
    virtual ~TransportMock() = default;
    MOCK_METHOD(void, JoinServer, ());
    MOCK_METHOD(void, SendMessage, (shared_ptr<DebugMessage>));
    MOCK_METHOD(void, Subscribe, (weak_ptr<Observer>));
    MOCK_METHOD(void, Unsubscribe, (weak_ptr<Observer>));
};

TEST(DebuggerTests_ClientMessageHandler, Construction) 
{
    auto transportMock = make_shared<TransportMock>();
    auto messageHandler = make_shared<ClientMessageHandler>(static_pointer_cast<ClientTransport>(transportMock));
}

TEST(DebuggerTests_ClientMessageHandler, InitializationTest) 
{
    auto transportMock = make_shared<TransportMock>();
    auto messageHandler = make_shared<ClientMessageHandler>(static_pointer_cast<ClientTransport>(transportMock));

    EXPECT_CALL((*transportMock), Subscribe(::_)).Times(1);
    messageHandler->Initialize();
}

TEST(DebuggerTests_ClientMessageHandler, JoinServerMessage) 
{
    auto transportMock = make_shared<TransportMock>();
    auto messageHandler = make_shared<ClientMessageHandler>(static_pointer_cast<ClientTransport>(transportMock));

    auto clientJoinedMessage = make_shared<DebugMessage>(CreateJoinedServerMessage());
    auto notificationArguments = make_shared<DebugMessageNotificationArguments>(clientJoinedMessage);
    auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);

    EXPECT_CALL((*transportMock), Subscribe(::_)).Times(1);
    messageHandler->Initialize();

    messageHandler->Notify(argumentsPointer);
    EXPECT_TRUE(messageHandler->IsConnected());
}
