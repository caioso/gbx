#pragma once

#include <algorithm>
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

    // Instruction Register
    IR = 8,

    // Special Registers
    I = 9,
    R = 10,
    PC = 11,
    SP = 13,

    // Index Registers
    IX = 15, 
    IY = 17,

    // Register Pairs (only representations, they have no physical meaning)
    HL = 19,
    BC = 20,
    DE = 21,
    AF = 22,
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

    void Swap();

    uint8_t PairToHighIndex(Register reg);
    uint8_t PairToLowIndex(Register reg);

    std::array<std::uint8_t, 18> _registers;
    std::array<std::uint8_t, 8> _alternates;
};

}