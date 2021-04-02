#pragma once

#include <sstream>

#include "ROM.h"
#include "GBXCoreExceptions.h"

namespace gbxcore::memory
{

class BankedROM : public ROM
{
public:
    BankedROM(size_t, size_t);
    virtual ~BankedROM() = default;

    size_t BankSize();
    size_t ResourceSize();
    size_t BankCount();
    size_t CurrentBank();
    void SelectBank(size_t);

private:
    size_t _bankSize{};
    size_t _activeBank{};
};

}