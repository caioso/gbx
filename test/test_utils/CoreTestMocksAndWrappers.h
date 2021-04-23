#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <memory>
#include <variant>

#include "ArithmeticLogicUnit.h"
#include "MemoryController.h"
#include "VideoControllerInterface.h"

using ::testing::Return;
using ::testing::_;

class MemoryControllerMock : public gbxcore::interfaces::MemoryControllerInterface
{
public:
    virtual ~MemoryControllerMock() = default;
    MOCK_METHOD((std::variant<uint8_t, uint16_t>), Read, (size_t a, gbxcore::interfaces::MemoryAccessType b));
    MOCK_METHOD(void, SwitchBank, (size_t, size_t));
    MOCK_METHOD(void, SetMode, (gbxcore::Ownership));
    MOCK_METHOD(gbxcore::Mode, Mode, ());
    MOCK_METHOD(void, Write, ((std::variant<uint8_t, uint16_t>), size_t));
    MOCK_METHOD(void, Load, ((std::unique_ptr<uint8_t*>), size_t, size_t, (std::optional<size_t>)));
    MOCK_METHOD(size_t, RegisterMemoryResource, ((std::unique_ptr<gbxcore::interfaces::MemoryResource>), gbxcore::AddressRange, gbxcore::Ownership));
    MOCK_METHOD(void, UnregisterMemoryResource, (size_t, gbxcore::Ownership));
    MOCK_METHOD(void, RegisterMemoryMappedRegister, ((std::unique_ptr<gbxcore::interfaces::MemoryMappedRegister>), size_t, gbxcore::Ownership));
    MOCK_METHOD(void, UnregisterMemoryMappedRegister, (size_t, gbxcore::Ownership));
};

class VideoControllerMock : public gbxcore::interfaces::VideoControllerInterface
{
public:
    virtual ~VideoControllerMock() = default;
    MOCK_METHOD(void, EnableVideo, ());
    MOCK_METHOD(void, DisableVideo, ());
    MOCK_METHOD(void, ShowWindow, ());
    MOCK_METHOD(void, HideWindow, ());
    MOCK_METHOD(void, ShowSprites, ());
    MOCK_METHOD(void, HideSprites, ());
    MOCK_METHOD(void, ShowBackground, ());
    MOCK_METHOD(void, HideBackground, ());
    MOCK_METHOD(void, SelectWindowTileMap, (size_t));
    MOCK_METHOD(void, SelectBackgroundTileMap, (size_t));
    MOCK_METHOD(void, SelectWindowAndBackgroundTileSet, (size_t));
    MOCK_METHOD(void, SetSpriteMode, (uint8_t));
    MOCK_METHOD(void, SetInterruptMode, (uint8_t));
    MOCK_METHOD(void, ScrollBackgroundY, (size_t));
    MOCK_METHOD(void, ScrollBackgroundX, (size_t));
    MOCK_METHOD(void, ScrollWindowX, (size_t));
    MOCK_METHOD(void, ScrollWindowY, (size_t));
    MOCK_METHOD(void, RegisterDMGBackgroundPaletteColor, (uint8_t, gbxcore::interfaces::PaletteColor));
    MOCK_METHOD(void, RegisterDMGObjectPaletteColor, (uint8_t, gbxcore::interfaces::DMGPalette, gbxcore::interfaces::PaletteColor));
    MOCK_METHOD(void, RegisterCGBBackgroundPaletteColorByte, (uint8_t, uint8_t));
};

class ArithmeticLogicDecorator : public gbxcore::ArithmeticLogicUnit
{
public:
    gbxcore::interfaces::RegisterBankInterface* GetRegisterBank()
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
