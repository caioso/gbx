#pragma once

#include <optional>
#include <variant>

#include "AddressRange.h"
#include "MemoryMappedRegister.h"
#include "MemoryResource.h"
#include "SystemMode.h"

namespace gbxcore::interfaces
{

enum class MemoryRequestType
{
    Read,
    Write,
    Result
};

class MemoryControllerInterface
{
public:
    virtual ~MemoryControllerInterface() = default;
   
    virtual std::variant<uint8_t, uint16_t> Read(size_t, interfaces::MemoryAccessType) = 0;
    virtual void Write(std::variant<uint8_t, uint16_t>, size_t) = 0;
    virtual void Load(std::unique_ptr<uint8_t*>, size_t, size_t, std::optional<size_t>) = 0;
 
    virtual void SwitchBank(size_t, size_t) = 0;
    virtual void SetMode(Mode) = 0;
    virtual Mode Mode() = 0;

    virtual size_t RegisterMemoryResource(std::unique_ptr<interfaces::MemoryResource>, AddressRange, Ownership) = 0;
    virtual void UnregisterMemoryResource(size_t, Ownership) = 0;
    
    virtual void RegisterMemoryMappedRegister(std::unique_ptr<interfaces::MemoryMappedRegister>, size_t, Ownership) = 0;
    virtual void UnregisterMemoryMappedRegister(size_t, Ownership) = 0;
};

}