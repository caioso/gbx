#include "BankedROM.h"

using namespace std;

namespace gbxcore::memory
{

BankedROM::BankedROM(size_t resourceSize, size_t bankSize)
    : ROM(resourceSize)
    , _bankSize(bankSize)
{}

size_t BankedROM::BankSize()
{
    return _bankSize;
}

size_t BankedROM::ResourceSize()
{
    return Size();
}

size_t BankedROM::BankCount()
{
    return Size()/_bankSize;
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

}