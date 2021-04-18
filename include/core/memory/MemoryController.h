#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <sstream>
#include <variant>
#include <vector>

#include "BankedROM.h"
#include "GBXCoreExceptions.h"
#include "MemoryControllerInterface.h"
#include "MemoryResource.h"
#include "SystemMode.h"

#include <map>
#include <memory>

namespace gbxcore::memory
{

typedef struct RegisteredMemoryResource_t
{
    std::unique_ptr<interfaces::MemoryResource> Resource;
    AddressRange Range;
    size_t ID;
}
RegisteredMemoryResource;

typedef struct RegisteredMemoryMappedRegister_t
{
    std::unique_ptr<interfaces::MemoryMappedRegister> Register;
    size_t Address;
}
RegisteredMemoryMappedRegister;

typedef struct ResourceIndexAndAddress_t
{
    uint8_t ResourceIndex;
    size_t LocalAddress;
}
ResourceIndexAndAddress;

class MemoryController : public interfaces::MemoryControllerInterface
{
public:
    MemoryController() = default;
    virtual ~MemoryController() = default;

    [[nodiscard]] std::variant<uint8_t, uint16_t> Read(size_t, interfaces::MemoryAccessType) override;
    void Write(std::variant<uint8_t, uint16_t>, size_t) override;
    void Load(std::unique_ptr<uint8_t*>, size_t, size_t, std::optional<size_t>) override;
    
    void SwitchBank(size_t, size_t) override;
    void SetMode(gbxcore::Mode) override;
    gbxcore::Mode Mode() override;

    size_t RegisterMemoryResource(std::unique_ptr<interfaces::MemoryResource>, AddressRange,  Ownership) override;
    void UnregisterMemoryResource(size_t, Ownership) override;

    void RegisterMemoryMappedRegister(std::unique_ptr<interfaces::MemoryMappedRegister>, size_t, Ownership) override;
    void UnregisterMemoryMappedRegister(size_t, Ownership) override;

private:
    inline void SortResources();
    inline void DetectMisfit(interfaces::MemoryResource*, AddressRange);
    inline void DetectOverlap(AddressRange);
    inline std::vector<RegisteredMemoryResource>* SelectResource();
    inline std::map<uint16_t, RegisteredMemoryMappedRegister>* SelectRegisterSource();
    inline std::map<uint16_t, RegisteredMemoryMappedRegister>* GetRegisterSource(Ownership);
    inline std::map<uint16_t, RegisteredMemoryMappedRegister>::iterator IsRegisterAddress(size_t);

    std::optional<ResourceIndexAndAddress> CalculateLocalAddress(size_t address);
    std::vector<RegisteredMemoryResource> _userResources; 
    std::vector<RegisteredMemoryResource> _systemResources; 
    std::map<uint16_t, RegisteredMemoryMappedRegister> _systemRegisters;
    std::map<uint16_t, RegisteredMemoryMappedRegister> _userRegisters;
    std::map<uint16_t, RegisteredMemoryMappedRegister> _bothRegisters;

    gbxcore::Mode _mode{};
    size_t _resourcesID;
};

}