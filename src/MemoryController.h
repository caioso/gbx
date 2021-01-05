#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "GBXExceptions.h"
#include "interfaces/MemoryControllerInterface.h"
#include "interfaces/MemoryInterface.h"

#include <memory>

namespace gbx
{

typedef struct MemoryResource_t
{
    std::shared_ptr<interfaces::MemoryInterface> resource;
    AddressRange range;
}
MemoryResource;

typedef struct ResourceIndexAndAddress_t
{
    uint8_t resourceIndex;
    uint16_t localAddress;
}
ResourceIndexAndAddress;

class MemoryController : public interfaces::MemoryControllerInterface
{
public:
    MemoryController() = default;
    virtual ~MemoryController() = default;

    std::variant<uint8_t, uint16_t> Read(uint16_t, interfaces::MemoryAccessType);
    void Write(std::variant<uint8_t, uint16_t>, uint16_t);
    void Load(std::shared_ptr<uint8_t*>, size_t, uint16_t, std::optional<size_t>);

    void RegisterMemoryResource(std::shared_ptr<interfaces::MemoryInterface>, AddressRange);
    void UnregisterMemoryResource(std::shared_ptr<interfaces::MemoryInterface>);

private:
    inline void SortResources();
    inline void DetectMisfit(std::shared_ptr<interfaces::MemoryInterface>, AddressRange);
    inline void DetectOverlap(AddressRange);

    std::optional<ResourceIndexAndAddress> CalculateLocalAddress(uint16_t address);
    std::vector<MemoryResource> _resources; 
};

}