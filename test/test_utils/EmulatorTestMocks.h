#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <memory>
#include <optional>
#include <variant>

#include "Runtime.h"
#include "RegisterBankInterface.h"

using ::testing::Return;
using ::testing::_;

class RuntimeMock : public gbxcore::interfaces::Runtime
{
public:
    virtual ~RuntimeMock() = default;
    MOCK_METHOD(void, Run, ());
    
    MOCK_METHOD(gbxcore::SecurityLevel, SecurityLevel, ());
    MOCK_METHOD(void, SetSecurityLevel, (gbxcore::SecurityLevel));
  
    MOCK_METHOD((std::variant<uint8_t, uint16_t>), ReadRegister, (gbxcore::interfaces::Register));
    MOCK_METHOD(void, WriteRegister, (gbxcore::interfaces::Register, (std::variant<uint8_t, uint16_t>)));
  
    MOCK_METHOD((std::variant<uint8_t, uint16_t>), ReadROM, (uint16_t, (std::optional<uint16_t>), (gbxcore::interfaces::MemoryAccessType)));
  
    MOCK_METHOD(void, LoadGame, (std::string));
    MOCK_METHOD(void, LoadBIOS, (std::string));
};