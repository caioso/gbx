#include "RegisterBank.h"

using namespace std;

namespace gbx
{

RegisterBank::RegisterBank()
    : _registers({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})
{}

uint8_t RegisterBank::Read(Register reg)
{
    assert(IsSingleRegister(reg));
    
    auto index = RegisterToIndex(reg);
    return _registers[index];
}

std::uint16_t RegisterBank::ReadPair(Register reg)
{
    assert(!IsSingleRegister(reg));

    auto highIndex = PairToHighIndex(reg);
    auto lowIndex = PairToLowIndex(reg);

    return static_cast<uint16_t>(_registers[highIndex] << 8 | _registers[lowIndex]);
}

void RegisterBank::Write(Register reg, uint8_t val)
{
    assert(IsSingleRegister(reg));

    auto index = RegisterToIndex(reg);

    if(IsSingleRegister(reg))
        _registers[index] = val & 0xFF;
    else
        WritePair(reg, val);
}

void RegisterBank::WritePair(Register reg, uint16_t val)
{
    assert(!IsSingleRegister(reg));

    auto highIndex = PairToHighIndex(reg);
    auto lowIndex = PairToLowIndex(reg);

    _registers[highIndex] = (val >> 8) & 0xFF;
    _registers[lowIndex] = val & 0xFF;
}

constexpr uint8_t RegisterBank::RegisterToIndex(Register reg)
{
    return static_cast<uint8_t>(reg);
}

constexpr bool RegisterBank::IsSingleRegister(Register reg)
{
    return reg != Register::BC && reg != Register::DE && reg != Register::HL && 
           reg != Register::AF && reg != Register::PC && reg != Register::SP;
}


uint8_t RegisterBank::PairToHighIndex(Register reg)
{
    switch (reg)
    {
        case Register::HL:
            return RegisterToIndex(Register::H);
        case Register::DE:
            return RegisterToIndex(Register::D);
        case Register::BC:
            return RegisterToIndex(Register::B);
        case Register::AF:
            return RegisterToIndex(Register::A);
        case Register::PC:
            return RegisterToIndex(Register::PC);
        case Register::SP:
            return RegisterToIndex(Register::SP);
        case Register::A:
        case Register::B:
        case Register::C:
        case Register::D:
        case Register::E:
        case Register::H:
        case Register::L:
        case Register::F:
            throw invalid_argument("invalid pair register");
    }
}

uint8_t RegisterBank::PairToLowIndex(Register reg)
{
    switch (reg)
    {
        case Register::HL:
            return RegisterToIndex(Register::L);
        case Register::DE:
            return RegisterToIndex(Register::E);
        case Register::BC:
            return RegisterToIndex(Register::C);
        case Register::AF:
            return RegisterToIndex(Register::F);
        case Register::PC:
            return RegisterToIndex(Register::PC) + 1;
        case Register::SP:
            return RegisterToIndex(Register::SP) + 1;
        case Register::A:
        case Register::B:
        case Register::C:
        case Register::D:
        case Register::E:
        case Register::H:
        case Register::L:
        case Register::F:
            throw invalid_argument("invalid pair register");
    }
}

}