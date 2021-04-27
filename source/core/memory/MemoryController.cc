#include "MemoryController.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::interfaces;

namespace gbxcore::memory
{

std::variant<uint8_t, uint16_t> MemoryController::Read(size_t address, MemoryAccessType accessType)
{
    if (auto reg = IsRegisterAddress(address);
        reg != SelectRegisterSource()->end())
    {
        return reg->second.Register->Read();
    }
    else
    {
        auto localAddress = CalculateLocalAddress(address);
        auto& targetResource = *SelectResource();
        if (localAddress == nullopt)
            throw MemoryControllerException("requested address to write to does not fall into any resource");

        return targetResource[localAddress.value().ResourceIndex].Resource.get()->Read(localAddress.value().LocalAddress, accessType);
    }
}

void MemoryController::Write(std::variant<uint8_t, uint16_t> value, size_t address)
{
    if (auto reg = IsRegisterAddress(address);
        reg != SelectRegisterSource()->end())
    {
        return reg->second.Register->Write(value);
    }
    else
    {
        auto localAddress = CalculateLocalAddress(address);
        auto& targetResource = *SelectResource();

        if (localAddress == nullopt)
            throw MemoryControllerException("requested address to read from does not fall into any resource");

        targetResource[localAddress.value().ResourceIndex].Resource.get()->Write(value, localAddress.value().LocalAddress);
    }
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

void MemoryController::SetSecurityLevel(gbxcore::SecurityLevel level)
{
    _level = level;
}

PrivilegeMode MemoryController::SecurityLevel()
{
    return _level;
}

size_t MemoryController::RegisterMemoryResource(std::unique_ptr<MemoryResource> resource, AddressRange range, PrivilegeMode owner)
{
    auto oldMode = SecurityLevel();
    SetSecurityLevel(owner);

        DetectOverlap(range);
        DetectMisfit(resource.get(), range);

        auto targetID = _resourcesID++;
        SelectResource()->push_back({std::move(resource), range, targetID});
        SortResources();
    
    SetSecurityLevel(oldMode);
    return targetID;
}

void MemoryController::UnregisterMemoryResource(size_t id, PrivilegeMode owner)
{
    auto oldMode = SecurityLevel();
    auto& targetResource = *SelectResource();

    SetSecurityLevel(owner);

    for (size_t i = static_cast<size_t>(0); i < targetResource.size(); i++)
    {
        if(targetResource[i].ID == id)
        {
            targetResource[i].Resource.release();
            targetResource.erase(begin(targetResource) + i);
            SetSecurityLevel(oldMode);
            return;
        }
    }

    SetSecurityLevel(oldMode);
    throw MemoryControllerException("the resource to be unregisterd could not found");
}

void MemoryController::RegisterMemoryMappedRegister(unique_ptr<MemoryMappedRegister> memoryMappedRegister, size_t address, PrivilegeMode owner)
{
    RegisteredMemoryMappedRegister reg = 
    {
        .Register = std::move(memoryMappedRegister),
        .Address = address
    };

    if (owner == PrivilegeMode::System && _systemRegisters.find(address) == _systemRegisters.end())
        _systemRegisters.insert({address, std::move(reg)});
    else if (owner == PrivilegeMode::User && _userRegisters.find(address) == _userRegisters.end())
        _userRegisters.insert({address, std::move(reg)});
    else if (owner == PrivilegeMode::Both && 
             _bothRegisters.find(address) == _bothRegisters.end() &&
             _systemRegisters.find(address) == _systemRegisters.end() && 
             _userRegisters.find(address) == _userRegisters.end())
        _bothRegisters.insert({address, std::move(reg)});
    else
    {
        stringstream ss;
        ss << "Register '" << address << "' has already been registered";
        throw MemoryControllerException(ss.str());
    }
}

void MemoryController::UnregisterMemoryMappedRegister(size_t address, PrivilegeMode owner)
{
    if (auto reg = GetRegisterSource(owner)->find(address);
        reg != GetRegisterSource(owner)->end())
        GetRegisterSource(owner)->erase(reg);
    else
    {
        stringstream ss;
        ss << "Register '" << address << "' has not been registered";
        throw MemoryControllerException(ss.str());
    }
}

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
    if (_level == PrivilegeMode::System)
        return &_systemResources;
    else
        return &_userResources;
}

inline std::map<uint16_t, RegisteredMemoryMappedRegister>* MemoryController::SelectRegisterSource()
{
    if (_level == PrivilegeMode::System)
        return &_systemRegisters;
    else if (_level == PrivilegeMode::User)
        return &_userRegisters;
    else
        return &_bothRegisters;
}

inline std::map<uint16_t, RegisteredMemoryMappedRegister>* MemoryController::GetRegisterSource(PrivilegeMode owner)
{
    if (owner == PrivilegeMode::System)
        return &_systemRegisters;
    else if (owner == PrivilegeMode::User)
        return &_userRegisters;
    else
        return &_bothRegisters;
}


inline std::map<uint16_t, RegisteredMemoryMappedRegister>::iterator MemoryController::IsRegisterAddress(size_t address)
{
    if (auto position = _bothRegisters.find(address);
        position != _bothRegisters.end())
        return position;

    if (_level == PrivilegeMode::System)
        return _systemRegisters.find(address);
    else
        return _userRegisters.find(address);    
}

}