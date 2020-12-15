#include "ROM.h"

using namespace std;

namespace gbx
{

ROM::ROM(std::size_t sizeInBytes)
    : _size(sizeInBytes)
    , _rom(new uint8_t[sizeInBytes])
{
    fill(&_rom.get()[0], &_rom.get()[_size], 0x00);
}

size_t ROM::Size()
{
    return _size;
}

uint8_t ROM::ReadByte(uint16_t address)
{
    if (address >= _size)
    {
        stringstream ss;
        ss << "bad memory address when reading byte from (" << address << " out of " << _size << ")";
        throw MemoryAccessException(ss.str());
    }

    return _rom[address];
}
uint16_t ROM::ReadWord(uint16_t address)
{
    if (address >= _size || address + 1 >= _size)
    {
        stringstream ss;
        ss << "bad memory address when reading word from (" << address << " out of " << _size << ")";
        throw MemoryAccessException(ss.str());
    }

    return _rom[address + 1] << 8 | _rom[address];
}

void ROM::WriteByte(uint8_t value, uint16_t address)
{
    if (address >= _size || address + 1 >= _size)
    {
        stringstream ss;
        ss << "bad memory address when writing byte to (" << address << " out of " << _size << ")";
        throw MemoryAccessException(ss.str());
    }

    _rom[address] = value;
}

void ROM::WriteWord(uint16_t value, uint16_t address)
{
    if (address >= _size || address + 1 >= _size)
    {
        stringstream ss;
        ss << "bad memory address when writing word to (" << address << " out of " << _size << ")";
        throw MemoryAccessException(ss.str());
    }

    _rom[address] = value & 0xFF;
    _rom[address + 1] = (value >> 8) & 0xFF;
}

}