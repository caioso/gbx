#pragma once

#include <algorithm>
#include <cstdlib>
#include <cstdint>
#include <memory>
#include <optional>
#include <sstream>
#include <variant>

#include "interfaces/MemoryInterface.h"
#include "GBXCoreExceptions.h"
#include "ROM.h"

namespace gbxcore
{
    
class RAM : public ROM
{
public:
    RAM(std::size_t);
    virtual ~RAM() = default;

    virtual void Write(std::variant<uint8_t, uint16_t>, uint16_t) override;

private:
    inline void CheckWriteConditions(std::variant<uint8_t, uint16_t>, uint16_t);
    
};

}