#pragma once

#include <cstdint>

#include "../GBXCoreExceptions.h"

namespace gbx::interfaces
{

enum class Flag
{
    Z = 7,
    N = 6,
    H = 5,
    CY = 4,
};

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
    PIR = 9, // Not Default Register -> It holds the pre-opcode of the current instruction.
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

    NoRegister
};

class RegisterBankInterface 
{
public:
    virtual std::uint8_t Read(Register) = 0;
    virtual std::uint16_t ReadPair(Register) = 0;

    virtual void Write(Register, std::uint8_t) = 0;
    virtual void WritePair(Register, std::uint16_t) = 0;
    
    virtual inline void SetFlag(Flag) = 0;
    virtual inline void ClearFlag(Flag) = 0;
    virtual inline uint8_t ReadFlag(Flag) = 0;
    virtual inline void WriteFlag(Flag, uint8_t) = 0;

    inline static uint8_t ToInstructionSource(interfaces::Register reg)
    {
        switch (reg)
        {
            case Register::B: return 0x00;
            case Register::C: return 0x01;
            case Register::D: return 0x02;
            case Register::E: return 0x03;
            case Register::H: return 0x04;
            case Register::L: return 0x05;
            case Register::A: return 0x07;
            default:
                throw RegisterBankException("invalid source register");
        }
    }

    inline static interfaces::Register FromInstructionSource(uint8_t reg)
    {
        switch (reg)
        {
            case 0x00: return Register::B;
            case 0x01: return Register::C;
            case 0x02: return Register::D;
            case 0x03: return Register::E;
            case 0x04: return Register::H;
            case 0x05: return Register::L;
            case 0x07: return Register::A;
            default:
                throw RegisterBankException("invalid source register");
        }
    }

    inline static uint8_t ToInstructionDestination(interfaces::Register reg)
    {
        switch (reg)
        {
            case Register::B: return 0x00;
            case Register::C: return 0x01;
            case Register::D: return 0x02;
            case Register::E: return 0x03;
            case Register::H: return 0x04;
            case Register::L: return 0x05;
            case Register::A: return 0x07;
            default:
                throw RegisterBankException("invalid destination register");
        }
    }

    inline static interfaces::Register FromInstructionDestination(uint8_t reg)
    {
        switch (reg)
        {
            case 0x00: return Register::B;
            case 0x01: return Register::C;
            case 0x02: return Register::D;
            case 0x03: return Register::E;
            case 0x04: return Register::H;
            case 0x05: return Register::L;
            case 0x07: return Register::A;
            default:
                throw RegisterBankException("invalid destination register");
        }
    } 

    inline static uint8_t ToInstructionRegisterPair(interfaces::Register reg)
    {
        switch (reg)
        {
            case Register::BC: return 0;
            case Register::DE: return 1;
            case Register::HL: return 2;
            case Register::SP: return 3;
            default:
                throw RegisterBankException("invalid register pair");
        }
    }
    
    inline static interfaces::Register FromInstructionToPair(uint8_t reg)
    {
        switch (reg)
        {
            case 0: return Register::BC;
            case 1: return Register::DE;
            case 2: return Register::HL;
            case 3: return Register::SP;
            default:
                throw RegisterBankException("invalid register pair");
        }
    }
    
    inline static uint8_t ToInstructionRegisterPushPair(interfaces::Register reg)
    {
        switch (reg)
        {
            case Register::BC: return 0;
            case Register::DE: return 1;
            case Register::HL: return 2;
            case Register::AF: return 3;
            default:
                throw RegisterBankException("invalid register pair");
        }
    }

    inline static interfaces::Register FromInstructionToPushPair(uint8_t reg)
    {
        switch (reg)
        {
            case 0: return Register::BC;
            case 1: return Register::DE;
            case 2: return Register::HL;
            case 3: return Register::AF;
            default:
                throw RegisterBankException("invalid register pair");
        }
    }
};

}