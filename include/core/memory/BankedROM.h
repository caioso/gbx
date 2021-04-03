#pragma once

#include <sstream>
#include <variant>

#include "ROM.h"
#include "GBXCoreExceptions.h"
#include "MemoryInterface.h"

namespace gbxcore::memory
{

class BankedROM : public ROM
{
public:
    BankedROM(size_t, size_t);
    virtual ~BankedROM() = default;

    std::variant<uint8_t, uint16_t> Read(uint16_t, interfaces::MemoryAccessType) override;
    void Write(std::variant<uint8_t, uint16_t>, uint16_t) override;

    size_t BankSize();
    size_t PhysicalResourceSize();
    size_t Size() override;
    size_t BankCount();
    size_t CurrentBank();
    void SelectBank(size_t);

private:
    void EvaluateAddress(uint16_t, interfaces::MemoryAccessType);

    size_t _bankSize{};
    size_t _activeBank{};
};

}