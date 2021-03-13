#include "ROM.h"

using namespace std;
using namespace gbxcore::interfaces;

namespace gbxcore
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

void ROM::Load(shared_ptr<uint8_t*> content, size_t size, optional<size_t> offset)
{
    if (size + offset.value_or(0) > _size)
    {
        stringstream ss;
        ss << "load size + offset exceeds ROM size ( load size: " << size << " offset " << offset.value_or(0) << " ROM size: " << _size << ")";
        throw MemoryAccessException(ss.str());
    }

    copy(&(*content)[0], &(*content)[size], &_rom.get()[offset.value_or(0)]);
}

std::variant<uint8_t, uint16_t> ROM::Read(uint16_t address, MemoryAccessType accessType)
{
    CheckReadConditions(address, accessType);

    if (accessType == MemoryAccessType::Byte)
        return variant<uint8_t, uint16_t>{in_place_index<0>, _rom[address]};
    else
        return variant<uint8_t, uint16_t>{in_place_index<1>, (_rom[address + 1] << 8 | _rom[address])};
}

void ROM::Write([[maybe_unused]] std::variant<uint8_t, uint16_t> value, [[maybe_unused]] uint16_t address)
{
    throw MemoryAccessException("Attempted to write to a read-only resource");
}

inline void ROM::CheckReadConditions(uint16_t address, MemoryAccessType accessType)
{
    if (address >= _size && accessType == MemoryAccessType::Byte)
    {
        stringstream ss;
        ss << "bad memory address when reading byte from (" << address << " out of " << _size << ")";
        throw MemoryAccessException(ss.str());
    }
    else if ((address >= _size || address + 1 >= _size) && accessType == MemoryAccessType::Word)
    {
        stringstream ss;
        ss << "bad memory address when reading word from (" << address << " out of " << _size << ")";
        throw MemoryAccessException(ss.str());
    }
}

}