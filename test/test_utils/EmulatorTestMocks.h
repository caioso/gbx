#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <memory>
#include <variant>

#include "DebugMessage.h"
#include "Runtime.h"
#include "ServerTransport.h"
#include "RegisterBankInterface.h"

using ::testing::Return;
using ::testing::_;

class RuntimeMock : public gbxcore::interfaces::Runtime
{
public:
    virtual ~RuntimeMock() = default;
    MOCK_METHOD(void, Run, ());
    MOCK_METHOD((std::variant<uint8_t, uint16_t>), ReadRegister, (gbxcore::interfaces::Register));
    MOCK_METHOD(void, WriteRegister, (gbxcore::interfaces::Register, (std::variant<uint8_t, uint16_t>)));
};

class ServerTransportMock : public gbxdb::interfaces::ServerTransport
{
public:
    virtual ~ServerTransportMock() = default;
    MOCK_METHOD(void, WaitForClient, ());
    MOCK_METHOD(void, SendMessage, (std::shared_ptr<gbxdb::interfaces::DebugMessage>));
    MOCK_METHOD(void, Subscribe, (std::weak_ptr<gbxcommons::Observer>));
    MOCK_METHOD(void, Unsubscribe, (std::weak_ptr<gbxcommons::Observer>));
};