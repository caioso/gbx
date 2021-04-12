#include "MemoryController.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::interfaces;

namespace gbxcore::memory
{

std::variant<uint8_t, uint16_t> MemoryController::Read(size_t address, MemoryAccessType accessType)
{
    auto localAddress = CalculateLocalAddress(address);
    auto& targetResource = *SelectResource();

    if (localAddress == nullopt)
        throw MemoryControllerException("requested address to write to does not fall into any resource");

    return targetResource[localAddress.value().ResourceIndex].Resource.get()->Read(localAddress.value().LocalAddress, accessType);
}

void MemoryController::Write(std::variant<uint8_t, uint16_t> value, size_t address)
{
    auto localAddress = CalculateLocalAddress(address);
    auto& targetResource = *SelectResource();

    if (localAddress == nullopt)
        throw MemoryControllerException("requested address to read from does not fall into any resource");

    targetResource[localAddress.value().ResourceIndex].Resource.get()->Write(value, localAddress.value().LocalAddress);
}

void MemoryController::Load(std::unique_ptr<uint8_t*> dataPointer, size_t size, size_t address, optional<size_t> offset)
{
    auto localAddress = CalculateLocalAddress(address);
    auto& targetResource = *SelectResource();

    if (localAddress == nullopt)
        throw MemoryControllerException("requested address to load data to does not fall into any resource");

    targetResource[localAddress.value().ResourceIndex].Resource.get()->Load(std::move(dataPointer), size, offset);
}

void MemoryController::SwitchBank(size_t address, size_t bank)
{
    auto localAddress = CalculateLocalAddress(address);
    
    // Test for Banked ROM or Banked RAM, otherwise, throw
    if (auto& targetResource = *SelectResource();
        dynamic_cast<BankedROM*>(targetResource[localAddress.value().ResourceIndex].Resource.get()) != nullptr)
        dynamic_cast<BankedROM*>(targetResource[localAddress.value().ResourceIndex].Resource.get())->SelectBank(bank);
    else
    {
        stringstream ss;
        ss << "Memory region cotaining address '" << address << "' is not a banked resource";
        throw MemoryControllerException(ss.str());
    }
}

void MemoryController::SetMode(gbxcore::Mode mode)
{
    _mode = mode;
}

Ownership MemoryController::Mode()
{
    return _mode;
}

size_t MemoryController::RegisterMemoryResource(std::unique_ptr<MemoryResource> resource, AddressRange range, Ownership owner)
{
    auto oldMode = Mode();
    SetMode(owner);

        DetectOverlap(range);
        DetectMisfit(resource.get(), range);

        auto targetID = _ID++;
        SelectResource()->push_back({std::move(resource), range, targetID});
        SortResources();
    
    SetMode(oldMode);
    return targetID;
}

void MemoryController::UnregisterMemoryResource(size_t id, Ownership owner)
{
    auto oldMode = Mode();
    auto& targetResource = *SelectResource();

    SetMode(owner);

    for (size_t i = static_cast<size_t>(0); i < targetResource.size(); i++)
    {
        if(targetResource[i].ID == id)
        {
            targetResource[i].Resource.release();
            targetResource.erase(begin(targetResource) + i);
            SetMode(oldMode);
            return;
        }
    }

    SetMode(oldMode);
    throw MemoryControllerException("the resource to be unregisterd could not found");
}

size_t MemoryController::RegisterMemoryMappedRegister(unique_ptr<MemoryMappedRegister>, size_t, Ownership)
{

}

void MemoryController::UnregisterMemoryMappedRegister(size_t, Ownership)
{}

inline void MemoryController::DetectMisfit(MemoryResource* resource, AddressRange range)
{
    if (range.End() - range.Begin() + 1 != resource->Size())
        throw MemoryControllerException("resouce and range misfit");
}

inline void MemoryController::SortResources()
{
    auto& targetResource = *SelectResource();

    sort(begin(targetResource), end(targetResource),
        [] (const RegisteredMemoryResource& resourceA, const RegisteredMemoryResource& resourceB) -> bool 
        {
            return resourceA.Range.Begin() > resourceB.Range.Begin();
        }
    );
}

inline void MemoryController::DetectOverlap(AddressRange range)
{
    auto& targetResource = *SelectResource();
    
    for (auto& [_resource, _range, _id] : targetResource)
    {
        if (range.Begin() < _range.End() && _range.Begin() < range.End())
            throw MemoryControllerException("ranges overlap");
    }
}

std::optional<ResourceIndexAndAddress> MemoryController::CalculateLocalAddress(size_t address)
{
    auto& targetResource = *SelectResource();

    // TODO: Optimize this later
    for (auto i = static_cast<size_t>(0); i < targetResource.size(); i++)
    {
        if (address >= targetResource[i].Range.Begin() && address <= targetResource[i].Range.End())
        {
            return make_optional<ResourceIndexAndAddress>({
                                static_cast<uint8_t>(i), 
                                static_cast<size_t>(address - targetResource[i].Range.Begin())});
        }
    }

    return nullopt;
}

inline std::vector<RegisteredMemoryResource>* MemoryController::SelectResource()
{
    if (_mode == Ownership::System)
        return &_systemResources;
    else
        return &_userResources;
}

}