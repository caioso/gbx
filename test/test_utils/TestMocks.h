#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <memory>
#include <variant>

#include "ClientTransport.h"
#include "DebugMessage.h"
#include "DebuggableRunner.h"
#include "Observer.h"
#include "OutputDriver.h"
#include "RegisterBank.h"
#include "RegisterBankInterface.h"
#include "Runtime.h"
#include "ServerTransport.h"

using ::testing::Return;
using ::testing::_;

class ServerTransportMock : public gbxdb::interfaces::ServerTransport
{
public:
    virtual ~ServerTransportMock() = default;
    MOCK_METHOD(void, WaitForClient, ());
    MOCK_METHOD(void, SendMessage, (std::shared_ptr<gbxdb::interfaces::DebugMessage>));

    MOCK_METHOD(void, Subscribe, ((std::weak_ptr<gbxcommons::Observer>)));
    MOCK_METHOD(void, Unsubscribe, ((std::weak_ptr<gbxcommons::Observer>)));
};

class ClientTransportMock : public gbxdb::interfaces::ClientTransport
{
public:
    virtual ~ClientTransportMock() = default;
    MOCK_METHOD(void, JoinServer, ());
    MOCK_METHOD(void, LeaveServer, ());
    MOCK_METHOD(void, SendMessage, (std::shared_ptr<gbxdb::interfaces::DebugMessage>));
    MOCK_METHOD(void, Subscribe, (std::weak_ptr<gbxcommons::Observer>));
    MOCK_METHOD(void, Unsubscribe, (std::weak_ptr<gbxcommons::Observer>));
};

class RuntimeMock : public gbxcore::interfaces::Runtime
{
public:
    virtual ~RuntimeMock() = default;
    MOCK_METHOD(void, Run, ());
    MOCK_METHOD((std::variant<uint8_t, uint16_t>), ReadRegister, (gbxcore::interfaces::Register));
    MOCK_METHOD(void, WriteRegister, (gbxcore::interfaces::Register, (std::variant<uint8_t, uint16_t>)));
};

class DebuggableRunnerMock : public gbxdb::interfaces::DebuggableRunner
{
public:
    virtual ~DebuggableRunnerMock() = default;
    MOCK_METHOD(void, ClientJoined, ());
    MOCK_METHOD(void, ClientLeft, ());
};

class OutputDriverMock : public gbxdb::interfaces::OutputDriver
{
public:
    virtual ~OutputDriverMock() = default;
    MOCK_METHOD(void, DisplayRegisterbank, ((std::array<uint8_t, gbxcore::RegisterBankSizeInBytes>)));
};
