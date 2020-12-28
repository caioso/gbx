#include "MemoryController.h"

using namespace std;

namespace gbx
{

MemoryController::MemoryController()
    : MemoryControllerALUChannel(make_shared<Channel<MemoryMessage>>(ChannelType::InOut))
{
    MemoryControllerALUChannel->OnReceived([this](MemoryMessage message) -> void { this->OnALUMessage(message); });
}

std::variant<uint8_t, uint16_t> MemoryController::Read(uint16_t address, MemoryAccessType accessType)
{
    auto localAddress = CalculateLocalAddress(address);

    if (localAddress == nullopt)
        throw MemoryControllerException("requested address to write to does not fall into any resource");

    return _resources[localAddress.value().resourceIndex].resource.get()->Read(localAddress.value().localAddress, accessType);
}

void MemoryController::Write(std::variant<uint8_t, uint16_t> value, uint16_t address)
{
    auto localAddress = CalculateLocalAddress(address);

    if (localAddress == nullopt)
        throw MemoryControllerException("requested address to read from does not fall into any resource");

    _resources[localAddress.value().resourceIndex].resource.get()->Write(value, localAddress.value().localAddress);
}

void MemoryController::Load(std::shared_ptr<uint8_t*> dataPointer, size_t size, uint16_t address, optional<size_t> offset)
{
    auto localAddress = CalculateLocalAddress(address);

    if (localAddress == nullopt)
        throw MemoryControllerException("requested address to load data to does not fall into any resource");

    _resources[localAddress.value().resourceIndex].resource.get()->Load(dataPointer, size, offset);
}

void MemoryController::RegisterMemoryResource(std::shared_ptr<Memory> resource, AddressRange range)
{
    DetectOverlap(range);
    DetectMisfit(resource, range);

    _resources.push_back({resource, range});
    SortResources();
}

void MemoryController::UnregisterMemoryResource(std::shared_ptr<Memory> resource)
{
    for (size_t i = static_cast<size_t>(0); i < _resources.size(); i++)
    {
        if(_resources[i].resource.get() == resource.get())
        {
            _resources.erase(begin(_resources) + i);
            return;
        }
    }

    throw MemoryControllerException("the resource to be unregisterd could not found");
}

void MemoryController::DetectMisfit(std::shared_ptr<Memory> resource, AddressRange range)
{
    if (range.End() - range.Begin() + 1 != resource.get()->Size())
        throw MemoryControllerException("resouce and range misfit");
}

void MemoryController::SortResources()
{
    sort(begin(_resources), end(_resources),
        [] (const MemoryResource& resourceA, const MemoryResource& resourceB) -> bool 
        {
            return resourceA.range.Begin() > resourceB.range.Begin();
        }
    );
}

void MemoryController::DetectOverlap(AddressRange range)
{
    for (auto [_resource, _range] : _resources)
    {
        if (range.Begin() < _range.End() && _range.Begin() < range.End())
            throw MemoryControllerException("ranges overlap");
    }
}

std::optional<ResourceIndexAndAddress> MemoryController::CalculateLocalAddress(uint16_t address)
{
    // TODO: Optimize this later
    for (auto i = static_cast<size_t>(0); i < _resources.size(); i++)
    {
        if (address >= _resources[i].range.Begin() && address <=_resources[i].range.End())
        {
            return make_optional<ResourceIndexAndAddress>({
                                static_cast<uint8_t>(i), 
                                static_cast<uint16_t>(address - _resources[i].range.Begin())});
        }
    }

    return nullopt;
}

void MemoryController::OnALUMessage(MemoryMessage message)
{
    if (message.Request == MemoryRequestType::Result)   
        throw MemoryControllerException("invalid memory operation detected in ALU Channel");
    
    if (message.Request == MemoryRequestType::Read)
        HandleReadRequest(message, MemoryControllerALUChannel);

    if (message.Request == MemoryRequestType::Write)
        HandleWriteRequest(message, MemoryControllerALUChannel);
}

void MemoryController::HandleReadRequest(MemoryMessage message, shared_ptr<Channel<MemoryMessage>>& channel)
{
    auto readData = Read(message.Address, message.AccessType);
    MemoryMessage response = {MemoryRequestType::Result, message.Address, readData, message.AccessType};

    channel->Send(response);
}


void MemoryController::HandleWriteRequest(MemoryMessage message, shared_ptr<Channel<MemoryMessage>>& channel)
{
    Write(message.Data, message.Address);
    MemoryMessage response = {MemoryRequestType::Result, message.Address, message.Data, message.AccessType};

    channel->Send(response);
}

}