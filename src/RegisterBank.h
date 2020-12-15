#pragma once

#include <array>
#include <assert.h>
#include <cstdint>
#include <exception>

namespace gbx
{

enum class Register
{
    // Basic Registers
    B = 0,
    C = 1,
    D = 2,
    E = 3,
    H = 4,
    L = 5,

    // Accumualtor
    A = 6,

    // Status Registers
    F = 7,

    // Special Registers
    PC = 8,
    SP = 10,

    // Register Pairs
    HL = 12,
    BC = 13,
    DE = 14,
    AF = 15,
};

class RegisterBank
{
public: 
    RegisterBank();
    ~RegisterBank() = default;
    std::uint8_t Read(Register reg);
    std::uint16_t ReadPair(Register reg);
    void Write(Register reg, std::uint8_t val);
    void WritePair(Register reg, std::uint16_t val);

private:
    constexpr uint8_t RegisterToIndex(Register reg);
    constexpr bool IsSingleRegister(Register reg);
    uint8_t PairToHighIndex(Register reg);
    uint8_t PairToLowIndex(Register reg);

    std::array<std::uint8_t, 12> _registers;
};

}