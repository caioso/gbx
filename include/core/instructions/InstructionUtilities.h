#pragma once

#include <cstdint>
#include <optional>
#include <memory>

#include "AddressingMode.h"
#include "MemoryController.h"

namespace gbxcore::instructions
{
    
class InstructionUtilities
{
public: 
    template<typename ...T>
    static inline bool IsAddressingMode(AddressingMode target, T ... modes)
    {
        return ((target == modes) || ...);
    }
};

}