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
#include "MemoryInterface.h"
#include "SystemMode.h"

#include <memory>

namespace gbxcore::memory
{

typedef struct MemoryResource_t
{
    std::unique_ptr<interfaces::MemoryInterface> Resource;
    AddressRange Range;
    size_t ID;
}
MemoryResource;

typedef struct ResourceIndexAndAddress_t
{
    uint8_t ResourceIndex;
    uint16_t LocalAddress;
}
ResourceIndexAndAddress;

class MemoryController : public interfaces::MemoryControllerInterface
{
public:
    MemoryController() = default;
    virtual ~MemoryController() = default;

    [[nodiscard]] std::variant<uint8_t, uint16_t> Read(uint16_t, interfaces::MemoryAccessType) override;
    void Write(std::variant<uint8_t, uint16_t>, uint16_t) override;
    void Load(std::unique_ptr<uint8_t*>, size_t, uint16_t, std::optional<size_t>) override;
    
    void SwitchBank(uint16_t, size_t) override;
    void SetMode(gbxcore::Mode) override;
    gbxcore::Mode Mode();

    size_t RegisterMemoryResource(std::unique_ptr<interfaces::MemoryInterface>, AddressRange,  Ownership) override;
    void UnregisterMemoryResource(size_t, Ownership) override;

private:
    inline void SortResources();
    inline void DetectMisfit(interfaces::MemoryInterface*, AddressRange);
    inline void DetectOverlap(AddressRange);
    inline std::vector<MemoryResource>* SelectResource();

    std::optional<ResourceIndexAndAddress> CalculateLocalAddress(uint16_t address);
    std::vector<MemoryResource> _userResources; 
    std::vector<MemoryResource> _systemResources; 

    gbxcore::Mode _mode{};
    size_t _ID;
};

}