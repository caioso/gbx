#include "BankedROM.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::memory
{

BankedROM::BankedROM(size_t resourceSize, size_t bankSize)
    : ROM(resourceSize)
    , _bankSize(bankSize)
{}

size_t BankedROM::PhysicalResourceSize()
{
    return ROM::Size();
}

size_t BankedROM::Size()
{
    return _bankSize;
}

size_t BankedROM::BankSize()
{
    return _bankSize;
}

size_t BankedROM::BankCount()
{
    return PhysicalResourceSize()/_bankSize;
}

size_t BankedROM::CurrentBank()
{
    return _activeBank;
}

void BankedROM::SelectBank(size_t bank)
{
    if (bank >= BankCount())
    {
        stringstream ss;
        ss << "Invalid memory bank '" << bank << "' selected";
        throw BankedMemoryException(ss.str()); 
    }

    _activeBank = bank;
}

variant<uint8_t, uint16_t> BankedROM::Read(uint16_t address, MemoryAccessType type)
{
    EvaluateAddress(address, type);
    uint16_t addressOffset = _bankSize * _activeBank;
    return ROM::Read(addressOffset + address, type);
}

void BankedROM::Write(variant<uint8_t, uint16_t> value, uint16_t address)
{
    EvaluateAddress(address, value.index() == 0 ? MemoryAccessType::Byte : MemoryAccessType::Word);
    ROM::Write(value, address);
}

void BankedROM::EvaluateAddress(uint16_t address, MemoryAccessType type)
{
    if (type == MemoryAccessType::Byte && address >= _bankSize)
    {
        stringstream ss;
        ss << "Address '" << address << "' is out of bank '" << _activeBank << "' bounds (bank size = " << _bankSize << ")";
        throw BankedMemoryException(ss.str());       
    }
    else if (type == MemoryAccessType::Word && (address >= _bankSize || address + 1 >= _bankSize))
    {
        stringstream ss;
        ss << "Addresses '" << address << "' and '" << address + 1 << "' are out of bank '" << _activeBank << "' bounds (bank size = " << _bankSize << ")";
        throw BankedMemoryException(ss.str());       
    }
}

}