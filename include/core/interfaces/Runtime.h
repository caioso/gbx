#pragma once

#include <optional>
#include <string>
#include <variant>

#include "MemoryResource.h"
#include "RegisterBankInterface.h"
#include "SystemMode.h"

namespace gbxcore::interfaces
{
    
struct Runtime
{
    virtual ~Runtime() = default;
    virtual void Run() = 0;
    virtual void LoadGame(std::string) = 0;
    virtual void LoadBIOS(std::string) = 0;
    
    virtual gbxcore::SecurityLevel SecurityLevel() = 0;
    virtual void SetSecurityLevel(gbxcore::SecurityLevel) = 0;

    virtual std::variant<uint8_t, uint16_t> ReadROM(uint16_t, std::optional<uint16_t>, interfaces::MemoryAccessType) = 0;
    
    virtual std::variant<uint8_t, uint16_t> ReadRegister(Register) = 0;
    virtual void WriteRegister(Register, std::variant<uint8_t, uint16_t>) = 0;
};

}