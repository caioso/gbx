#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "AddressRange.h"
#include "Channel.h"
#include "GBXExceptions.h"
#include "Memory.h"

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

enum class MemoryRequestType
{
    Read,
    Write,
    Result
};
typedef struct MemoryMessage_t
{
    MemoryRequestType Request;
    uint16_t Address;
    std::variant<uint8_t, uint16_t> Data;
    MemoryAccessType AccessType;
}
MemoryMessage;

class MemoryController
{
public:
    MemoryController();
    ~MemoryController() = default;

    std::variant<uint8_t, uint16_t> Read(uint16_t address, MemoryAccessType accessType);
    void Write(std::variant<uint8_t, uint16_t> value, uint16_t address);
    void Load(std::shared_ptr<uint8_t*> dataPointer, size_t size, uint16_t address, std::optional<size_t> offset);

    void RegisterMemoryResource(std::shared_ptr<Memory> resource, AddressRange range);
    void UnregisterMemoryResource(std::shared_ptr<Memory> resource);

    // Channels
    std::shared_ptr<Channel<MemoryMessage>> MemoryControllerALUChannel;

private:
    void SortResources();
    void DetectMisfit(std::shared_ptr<Memory> resource, AddressRange range);
    void DetectOverlap(AddressRange range);

    void HandleReadRequest(MemoryMessage message, std::shared_ptr<Channel<MemoryMessage>>& channel);
    void HandleWriteRequest(MemoryMessage message, std::shared_ptr<Channel<MemoryMessage>>& channel);

    void OnALUMessage(MemoryMessage message);

    std::optional<ResourceIndexAndAddress> CalculateLocalAddress(uint16_t address);
    std::vector<MemoryResource> _resources; 
};

}