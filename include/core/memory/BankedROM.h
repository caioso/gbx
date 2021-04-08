#pragma once

#include <iostream>
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

    std::variant<uint8_t, uint16_t> Read(size_t, interfaces::MemoryAccessType) override;
    void Write(std::variant<uint8_t, uint16_t>, size_t) override;
    void Load(std::shared_ptr<uint8_t*>, std::size_t, std::optional<size_t>) override;

    size_t BankSize();
    size_t PhysicalResourceSize();
    size_t Size() override;
    size_t BankCount();
    size_t CurrentBank();
    void SelectBank(size_t);

private:
    void EvaluateAddress(size_t, interfaces::MemoryAccessType);

    size_t _bankSize{};
    size_t _activeBank{};
};

}