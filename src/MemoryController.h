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

    std::variant<uint8_t, uint16_t> Read(uint16_t, MemoryAccessType);
    void Write(std::variant<uint8_t, uint16_t>, uint16_t);
    void Load(std::shared_ptr<uint8_t*>, size_t, uint16_t, std::optional<size_t>);

    void RegisterMemoryResource(std::shared_ptr<Memory>, AddressRange);
    void UnregisterMemoryResource(std::shared_ptr<Memory>);

    // Channels
    std::shared_ptr<Channel<MemoryMessage>> MemoryControllerALUChannel;

private:
    inline void SortResources();
    inline void DetectMisfit(std::shared_ptr<Memory>, AddressRange);
    inline void DetectOverlap(AddressRange);

    inline void HandleReadRequest(MemoryMessage, std::shared_ptr<Channel<MemoryMessage>>&);
    inline void HandleWriteRequest(MemoryMessage, std::shared_ptr<Channel<MemoryMessage>>&);

    inline void OnALUMessage(MemoryMessage);

    std::optional<ResourceIndexAndAddress> CalculateLocalAddress(uint16_t address);
    std::vector<MemoryResource> _resources; 
};

}