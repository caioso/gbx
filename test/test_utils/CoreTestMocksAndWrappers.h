#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <memory>
#include <variant>

#include "ArithmeticLogicUnit.h"
#include "MemoryController.h"

using ::testing::Return;
using ::testing::_;

class MemoryControllerMock : public gbxcore::interfaces::MemoryControllerInterface
{
public:
    virtual ~MemoryControllerMock() = default;
    MOCK_METHOD((std::variant<uint8_t, uint16_t>), Read, (uint16_t a, gbxcore::interfaces::MemoryAccessType b));
    MOCK_METHOD(void, Write, ((std::variant<uint8_t, uint16_t>), uint16_t));
    MOCK_METHOD(void, Load, ((std::shared_ptr<uint8_t*>), size_t, uint16_t, (std::optional<size_t>)));
    MOCK_METHOD(void, RegisterMemoryResource, ((std::shared_ptr<gbxcore::interfaces::MemoryInterface>), gbxcore::AddressRange));
    MOCK_METHOD(void, UnregisterMemoryResource, ((std::shared_ptr<gbxcore::interfaces::MemoryInterface>)));
};


class ArithmeticLogicDecorator : public gbxcore::ArithmeticLogicUnit
{
public:
    std::shared_ptr<gbxcore::interfaces::RegisterBankInterface> GetRegisterBank()
    {
        return this->_registers;
    }    
    
    gbxcore::interfaces::DecodedInstruction& GetInstructionData() { return _instructionData; }
    
    void DecodeInstruction(uint8_t opcode, std::optional<uint8_t> preOpcode)
    {
        _registers->Write(gbxcore::interfaces::Register::IR, opcode);
        _registers->Write(gbxcore::interfaces::Register::PIR, preOpcode.value_or(0x00));
        Decode();
    }
};
