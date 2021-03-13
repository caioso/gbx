#include "RAM.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore
{
RAM::RAM(size_t size)
    : ROM(size)
{}
    
void RAM::Write(variant<uint8_t, uint16_t> value, uint16_t address)
{
    CheckWriteConditions(value, address);

    if (holds_alternative<uint8_t>(value))
    {
        auto byte = get<uint8_t>(value);
        _rom[address] = byte & 0xFF;
    }
    else if (holds_alternative<uint16_t>(value))
    {
        auto word = get<uint16_t>(value);
        _rom[address] = word & 0xFF;
        _rom[address + 1] = (word >> 8) & 0xFF;
    }
    else
        throw MemoryAccessException("variant has no value.");
}

inline void RAM::CheckWriteConditions(std::variant<uint8_t, uint16_t> value, uint16_t address)
{
    if (holds_alternative<uint8_t>(value) && address >= _size)
    {
        stringstream ss;
        ss << "bad memory address when writing byte to (" << address << " out of " << _size << ")";
        throw MemoryAccessException(ss.str());
    }
    else if (holds_alternative<uint16_t>(value) && (address >= _size || address + 1 >= _size))
    {
        stringstream ss;
        ss << "bad memory address when writing word to (" << address << " out of " << _size << ")";
        throw MemoryAccessException(ss.str());
    }
}

}