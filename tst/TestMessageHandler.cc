#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

#include "../src/runtime/MessageHandler.h"
#include "../src/interfaces/ServerTransport.h"

using namespace gbx::runtime;
using namespace gbx::interfaces;
using namespace gbx;
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