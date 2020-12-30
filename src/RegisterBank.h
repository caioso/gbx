#pragma once

#include <algorithm>
#include <array>
#include <assert.h>
#include <cstdint>
#include <exception>

#include "GBXExceptions.h"

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

    NoRegiser
};

class RegisterBank
{
public: 
    RegisterBank();
    ~RegisterBank() = default;
    std::uint8_t Read(Register);
    std::uint16_t ReadPair(Register);
    void Write(Register, std::uint8_t);
    void WritePair(Register, std::uint16_t);
    
    static uint8_t ToInstructionSource(Register);
    static Register FromInstructionSource(uint8_t);
    static uint8_t ToInstructionDestination(Register);
    static Register FromInstructionDestination(uint8_t);

private:
    constexpr uint8_t RegisterToIndex(Register);
    constexpr bool IsSingleRegister(Register);

    inline void Swap();

    inline uint8_t PairToHighIndex(Register);
    inline uint8_t PairToLowIndex(Register);

    std::array<std::uint8_t, 19> _registers;
    std::array<std::uint8_t, 8> _alternates;
};

}