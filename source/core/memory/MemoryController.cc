#include "MemoryController.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::interfaces;

namespace gbxcore::memory
{

std::variant<uint8_t, uint16_t> MemoryController::Read(uint16_t address, MemoryAccessType accessType)
{
    auto localAddress = CalculateLocalAddress(address);

    if (localAddress == nullopt)
        throw MemoryControllerException("requested address to write to does not fall into any resource");

    return SelectResource()[localAddress.value().ResourceIndex].Resource.get()->Read(localAddress.value().LocalAddress, accessType);
}

void MemoryController::Write(std::variant<uint8_t, uint16_t> value, uint16_t address)
{
    auto localAddress = CalculateLocalAddress(address);

    if (localAddress == nullopt)
        throw MemoryControllerException("requested address to read from does not fall into any resource");

    SelectResource()[localAddress.value().ResourceIndex].Resource.get()->Write(value, localAddress.value().LocalAddress);
}

void MemoryController::Load(std::shared_ptr<uint8_t*> dataPointer, size_t size, uint16_t address, optional<size_t> offset)
{
    auto localAddress = CalculateLocalAddress(address);

    if (localAddress == nullopt)
        throw MemoryControllerException("requested address to load data to does not fall into any resource");

    SelectResource()[localAddress.value().ResourceIndex].Resource.get()->Load(dataPointer, size, offset);
}

void MemoryController::SwitchBank(uint16_t address, size_t bank)
{
    auto localAddress = CalculateLocalAddress(address);
    // Test for Banked ROM or Banked RAM, otherwise, throw
    if (dynamic_pointer_cast<BankedROM>(SelectResource()[localAddress.value().ResourceIndex].Resource) != nullptr)
        dynamic_pointer_cast<BankedROM>(SelectResource()[localAddress.value().ResourceIndex].Resource)->SelectBank(bank);
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

void MemoryController::RegisterMemoryResource(std::shared_ptr<MemoryInterface> resource, AddressRange range, Ownership owner)
{
    auto oldMode = Mode();
    SetMode(owner);

        DetectOverlap(range);
        DetectMisfit(resource, range);

        SelectResource().push_back({resource, range});
        SortResources();
    
    SetMode(oldMode);
}

void MemoryController::UnregisterMemoryResource(std::shared_ptr<MemoryInterface> resource, Ownership owner)
{
    auto oldMode = Mode();
    SetMode(owner);

    for (size_t i = static_cast<size_t>(0); i < SelectResource().size(); i++)
    {
        if(SelectResource()[i].Resource.get() == resource.get())
        {
            SelectResource().erase(begin(SelectResource()) + i);
            SetMode(oldMode);
            return;
        }
    }

    SetMode(oldMode);
    throw MemoryControllerException("the resource to be unregisterd could not found");
}

inline void MemoryController::DetectMisfit(std::shared_ptr<MemoryInterface> resource, AddressRange range)
{
    if (range.End() - range.Begin() + 1 != resource.get()->Size())
        throw MemoryControllerException("resouce and range misfit");
}

inline void MemoryController::SortResources()
{
    sort(begin(SelectResource()), end(SelectResource()),
        [] (const MemoryResource& resourceA, const MemoryResource& resourceB) -> bool 
        {
            return resourceA.Range.Begin() > resourceB.Range.Begin();
        }
    );
}

inline void MemoryController::DetectOverlap(AddressRange range)
{
    for (auto [_resource, _range] : SelectResource())
    {
        if (range.Begin() < _range.End() && _range.Begin() < range.End())
            throw MemoryControllerException("ranges overlap");
    }
}

std::optional<ResourceIndexAndAddress> MemoryController::CalculateLocalAddress(uint16_t address)
{
    // TODO: Optimize this later
    for (auto i = static_cast<size_t>(0); i < SelectResource().size(); i++)
    {
        if (address >= SelectResource()[i].Range.Begin() && address <=SelectResource()[i].Range.End())
        {
            return make_optional<ResourceIndexAndAddress>({
                                static_cast<uint8_t>(i), 
                                static_cast<uint16_t>(address - SelectResource()[i].Range.Begin())});
        }
    }

    return nullopt;
}

inline std::vector<MemoryResource>& MemoryController::SelectResource()
{
    if (_mode == Ownership::System)
        return _systemResources;
    else
        return _userResources;
}

}