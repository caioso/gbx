#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <string> 
#include <thread> 
#include <variant> 

#include "../src/interfaces/CommunicationChannel.h"
#include "../src/interfaces/DebugMessage.h"
#include "../src/interfaces/ServerProtocol.h"
#include "../src/interfaces/ServerProtocolParameters.h"
#include "../src/requests/debug-server-status/DebugServerStatusRequest.h"
#include "../src/DebugServer.h"
#include "../src/GBXExceptions.h"
#include "interfaces/Runtime.h"

using namespace std;
using namespace gbx::interfaces;
using namespace gbx::requests;
using namespace gbx;
using namespace gbxcommons;
using namespace gbxcore::interfaces;
using namespace gbxcore;

using ::testing::Return;
using ::testing::_;

class DebugServerWrapper : public DebugServer
{
public:
    DebugServerWrapper(shared_ptr<gbxcore::interfaces::Runtime> gbx,
                       shared_ptr<gbx::interfaces::ServerProtocol> protocol)
        : DebugServer(gbx, protocol)
    {}
    ~DebugServerWrapper() = default;

    DebugServerState State()
    {
        return _state;
    }
};

class DummyDebugMessage : public gbx::interfaces::DebugMessage
{
public:
    DummyDebugMessage() : DebugMessage(MessageType::UnknownMessage)
    {}
    ~DummyDebugMessage() = default;
};

class RuntimeMock : public Runtime
{
public:
    virtual ~RuntimeMock() = default;
    MOCK_METHOD(void, Run, ());
    MOCK_METHOD((std::variant<uint8_t, uint16_t>), ReadRegister, (Register));
    MOCK_METHOD(void, WriteRegister, (Register, (std::variant<uint8_t, uint16_t>)));
};

class ServerProtocolMock : public ServerProtocol
{
public:
    virtual ~ServerProtocolMock() = default;
    MOCK_METHOD(void, Initialize, (std::shared_ptr<ServerProtocolParameters>));
    MOCK_METHOD(void, WaitForClient, ());
    MOCK_METHOD(void, AddEventListener, (std::weak_ptr<gbxcommons::Observer<gbx::interfaces::RawDebugMessageEventArgs>>));
    MOCK_METHOD(void, Send, (std::shared_ptr<DebugMessage>));
};

void ConnectServer(shared_ptr<DebugServerWrapper> debugServer, shared_ptr<ServerProtocolMock> protocol)
{
    auto protocolParameters = make_shared<ServerProtocolParameters>();
    EXPECT_CALL((*protocol), Initialize(::_));
    EXPECT_CALL((*protocol), AddEventListener(::_));
    debugServer->Initialize(protocolParameters);
    EXPECT_EQ(DebugServerState::Initialized, debugServer->State());

    EXPECT_CALL((*protocol), WaitForClient());
    debugServer->WaitForClient();
    EXPECT_EQ(DebugServerState::WaitingForClient, debugServer->State());

    // Pretend that the the protocol object sent a notification to the request translator and that it notified the server about a new requets.
    auto clientConnectedRequest = make_shared<ClientConnectedRequest>();
    auto debugMessageEventArgs = make_shared<DebugMessageEventArgs>(clientConnectedRequest);
    debugServer->Notify(debugMessageEventArgs);

    EXPECT_EQ(DebugServerState::Connected, debugServer->State());
}

TEST(TestDebugServer, InitializeServer)
{
    auto protocolParameters = make_shared<ServerProtocolParameters>();
    auto runtime = make_shared<RuntimeMock>();
    auto protocol = make_shared<ServerProtocolMock>();
    auto debugServer = make_shared<DebugServerWrapper>(static_pointer_cast<Runtime>(runtime),
                                                static_pointer_cast<ServerProtocol>(protocol));
    
    EXPECT_CALL((*protocol), Initialize(::_));
    EXPECT_CALL((*protocol), AddEventListener(::_));
    debugServer->Initialize(protocolParameters);
    EXPECT_EQ(DebugServerState::Initialized, debugServer->State());
}

TEST(TestDebugServer, WaitForClientToClient)
{
    auto protocolParameters = make_shared<ServerProtocolParameters>();
    auto runtime = make_shared<RuntimeMock>();
    auto protocol = make_shared<ServerProtocolMock>();
    auto debugServer = make_shared<DebugServerWrapper>(static_pointer_cast<Runtime>(runtime),
                                                static_pointer_cast<ServerProtocol>(protocol));
    
    EXPECT_CALL((*protocol), Initialize(::_));
    EXPECT_CALL((*protocol), AddEventListener(::_));
    debugServer->Initialize(protocolParameters);
    EXPECT_EQ(DebugServerState::Initialized, debugServer->State());

    EXPECT_CALL((*protocol), WaitForClient());
    debugServer->WaitForClient();
    EXPECT_EQ(DebugServerState::WaitingForClient, debugServer->State());
}

TEST(TestDebugServer, TryToWaitForClientWithoutInitialization)
{
    auto protocolParameters = make_shared<ServerProtocolParameters>();
    auto runtime = make_shared<RuntimeMock>();
    auto protocol = make_shared<ServerProtocolMock>();
    auto debugServer = make_shared<DebugServerWrapper>(static_pointer_cast<Runtime>(runtime),
                                                static_pointer_cast<ServerProtocol>(protocol));

    ASSERT_EXCEPTION( { debugServer->WaitForClient();} , 
                      DebugServerException, 
                      "Attempted to perform operaton in an uninitialized debug server");
}

TEST(TestDebugServer, ClientConnected)
{
    auto protocolParameters = make_shared<ServerProtocolParameters>();
    auto runtime = make_shared<RuntimeMock>();
    auto protocol = make_shared<ServerProtocolMock>();
    auto debugServer = make_shared<DebugServerWrapper>(static_pointer_cast<Runtime>(runtime),
                                                static_pointer_cast<ServerProtocol>(protocol));
    
    ConnectServer(debugServer, protocol);
}

TEST(TestDebugServer, ClientConnectedNotificationReceivedAtIncorrectTime)
{
    auto protocolParameters = make_shared<ServerProtocolParameters>();
    auto runtime = make_shared<RuntimeMock>();
    auto protocol = make_shared<ServerProtocolMock>();
    auto debugServer = make_shared<DebugServerWrapper>(static_pointer_cast<Runtime>(runtime),
                                                static_pointer_cast<ServerProtocol>(protocol));
    
    ConnectServer(debugServer, protocol);
     // Pretend that the the protocol object sent a notification to the request translator and that it notified the server about a new requets.
    auto clientConnectedRequest = make_shared<ClientConnectedRequest>();
    auto debugMessageEventArgs = make_shared<DebugMessageEventArgs>(clientConnectedRequest);

    ASSERT_EXCEPTION( { debugServer->Notify(debugMessageEventArgs); } , 
                      DebugServerException, 
                      "Client already connected");
}

TEST(TestDebugServer, ClientConnectedNotificationReceivedAtIncorrectTime2)
{
    auto protocolParameters = make_shared<ServerProtocolParameters>();
    auto runtime = make_shared<RuntimeMock>();
    auto protocol = make_shared<ServerProtocolMock>();
    auto debugServer = make_shared<DebugServerWrapper>(static_pointer_cast<Runtime>(runtime),
                                                static_pointer_cast<ServerProtocol>(protocol));
    
    EXPECT_CALL((*protocol), Initialize(::_));
    EXPECT_CALL((*protocol), AddEventListener(::_));
    debugServer->Initialize(protocolParameters);
    EXPECT_EQ(DebugServerState::Initialized, debugServer->State());

    // Pretend that the the protocol object sent a notification to the request translator and that it notified the server about a new requets.
    auto clientConnectedRequest = make_shared<ClientConnectedRequest>();
    auto debugMessageEventArgs = make_shared<DebugMessageEventArgs>(clientConnectedRequest);

    ASSERT_EXCEPTION( { debugServer->Notify(debugMessageEventArgs); } , 
                      DebugServerException, 
                      "Invalid connection request received");
}

TEST(TestDebugServer, NotifyUnknownArgsType)
{
    auto protocolParameters = make_shared<ServerProtocolParameters>();
    auto runtime = make_shared<RuntimeMock>();
    auto protocol = make_shared<ServerProtocolMock>();
    auto debugServer = make_shared<DebugServerWrapper>(static_pointer_cast<Runtime>(runtime),
                                                static_pointer_cast<ServerProtocol>(protocol));
    
    EXPECT_CALL((*protocol), Initialize(::_));
    EXPECT_CALL((*protocol), AddEventListener(::_));
    debugServer->Initialize(protocolParameters);
    
    EXPECT_CALL((*protocol), WaitForClient());
    debugServer->WaitForClient();

    auto dummyRequest = make_shared<DummyDebugMessage>();
    auto debugMessageEventArgs = make_shared<DebugMessageEventArgs>(dummyRequest);

    ASSERT_EXCEPTION( { debugServer->Notify(debugMessageEventArgs); } , 
                      DebugServerException, 
                      "Unknown NotificationArgs type received");
}

TEST(TestDebugServer, ReceiveStatusRequest)
{
    auto protocolParameters = make_shared<ServerProtocolParameters>();
    auto runtime = make_shared<RuntimeMock>();
    auto protocol = make_shared<ServerProtocolMock>();
    auto debugServer = make_shared<DebugServerWrapper>(static_pointer_cast<Runtime>(runtime),
                                                static_pointer_cast<ServerProtocol>(protocol));
    
    ConnectServer(debugServer, protocol);

    auto debugServerStatusRequest = make_shared<DebugServerStatusRequest>();
    auto debugMessageEventArgs = make_shared<DebugMessageEventArgs>(debugServerStatusRequest);

    auto response = make_shared<DebugServerStatusResponse>(DebugServerStatus::Halted);
    EXPECT_CALL((*protocol), Send(::_)).WillOnce(testing::Invoke([&](shared_ptr<DebugMessage> arguement)
    {
        auto message = dynamic_pointer_cast<DebugServerStatusResponse>(arguement);
        EXPECT_NE(nullptr, arguement);
        EXPECT_EQ(MessageType::StatusResponse, message->Type());
        EXPECT_EQ(DebugServerStatus::Halted, message->Status());
    }));

    debugServer->Notify(debugMessageEventArgs);
}