#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "AddressRange.h"
#include "GBXExceptions.h"
#include "Memory.h"

namespace gbx
{

typedef struct MemoryResource_t
{
    std::shared_ptr<Memory> resource;
    AddressRange range;
}
MemoryResource;

typedef struct ResourceIndexAndAddress_t
{
    uint8_t resourceIndex;
    uint16_t localAddress;
}
ResourceIndexAndAddress;
class MemoryController
{
public:
    MemoryController() = default;
    ~MemoryController() = default;

    std::variant<uint8_t, uint16_t> Read(uint16_t address, MemoryAccessType accessType);
    void Write(std::variant<uint8_t, uint16_t> value, uint16_t address);

    void RegisterMemoryResource(std::shared_ptr<Memory> resource, AddressRange range);
    void UnregisterMemoryResource(std::shared_ptr<Memory> resource);

private:
    void SortResources();
    void DetectMisfit(std::shared_ptr<Memory> resource, AddressRange range);
    void DetectOverlap(AddressRange range);

    std::optional<ResourceIndexAndAddress> CalculateLocalAddress(uint16_t address);

    std::vector<MemoryResource> _resources; 
};

}