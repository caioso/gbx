#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "GBXExceptions.h"
#include "MemoryControllerInterface.h"

#include <memory>

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

typedef struct MemoryMessage_t
{
    MemoryRequestType Request;
    uint16_t Address;
    std::variant<uint8_t, uint16_t> Data;
    MemoryAccessType AccessType;
}
MemoryMessage;

class MemoryController : public MemoryControllerInterface
{
public:
    MemoryController() = default;
    virtual ~MemoryController() = default;

    std::variant<uint8_t, uint16_t> Read(uint16_t, MemoryAccessType);
    void Write(std::variant<uint8_t, uint16_t>, uint16_t);
    void Load(std::shared_ptr<uint8_t*>, size_t, uint16_t, std::optional<size_t>);

    void RegisterMemoryResource(std::shared_ptr<Memory>, AddressRange);
    void UnregisterMemoryResource(std::shared_ptr<Memory>);

private:
    inline void SortResources();
    inline void DetectMisfit(std::shared_ptr<Memory>, AddressRange);
    inline void DetectOverlap(AddressRange);

    inline void OnALUMessage(MemoryMessage);

    std::optional<ResourceIndexAndAddress> CalculateLocalAddress(uint16_t address);
    std::vector<MemoryResource> _resources; 
};

}