#pragma once

#include <algorithm>
#include <array>
#include <assert.h>
#include <cstdint>
#include <exception>
#include <type_traits>

#include "GBXCoreExceptions.h"
#include "RegisterBankInterface.h"

namespace gbxcore
{

const size_t RegisterBankSizeInBytes = 20;

class RegisterBank : public interfaces::RegisterBankInterface 
{
public: 
    RegisterBank();
    virtual ~RegisterBank() = default;

    std::uint8_t Read(interfaces::Register);
    std::uint16_t ReadPair(interfaces::Register);

    void Write(interfaces::Register, std::uint8_t);
    void WritePair(interfaces::Register, std::uint16_t);
    
    inline void SetFlag(interfaces::Flag flag)
    {
        _registers[static_cast<uint8_t>(interfaces::Register::F)] |= 1 << static_cast<uint8_t>(flag);
    }
    
    inline void ClearFlag(interfaces::Flag flag)
    {
        _registers[static_cast<uint8_t>(interfaces::Register::F)] &= ~(1 << static_cast<uint8_t>(flag));
    }

    inline uint8_t ReadFlag(interfaces::Flag flag)
    {
        return ((_registers[static_cast<uint8_t>(interfaces::Register::F)] >> static_cast<uint8_t>(flag)) & 0x01);
    }

    inline void WriteFlag(interfaces::Flag flag, uint8_t value)
    {
        _registers[static_cast<uint8_t>(interfaces::Register::F)] &= ~(1 << static_cast<uint8_t>(flag));
        _registers[static_cast<uint8_t>(interfaces::Register::F)] |= value << static_cast<uint8_t>(flag);
    }
    
private:
    constexpr uint8_t RegisterToIndex(interfaces::Register);
    constexpr bool IsSingleRegister(interfaces::Register);

    inline void Swap();

    inline uint8_t PairToHighIndex(interfaces::Register);
    inline uint8_t PairToLowIndex(interfaces::Register);

    std::array<std::uint8_t, RegisterBankSizeInBytes> _registers;
    std::array<std::uint8_t, RegisterBankSizeInBytes> _alternates;
};

}