#include "GameBoyX.h"

using namespace gbxcore::constants;
using namespace gbxcore::interfaces;
using namespace gbxcore::memory;
using namespace gbxcore::video;
using namespace gbxcommons;
using namespace std;

namespace gbxcore
{

GameBoyX::GameBoyX()
{
    auto controlUnit = make_unique<ControlUnit>();
    auto memoryController = make_unique<MemoryController>();
         _memoryControllerPtr = memoryController.get();
    
    auto registers = make_unique<RegisterBank>();
         _registersPtr = registers.get();

    auto alu = make_unique<ArithmeticLogicUnit>();
    auto clock = make_unique<Clock>(GBCClockPeriod);

    // GBX System Mode memory map
    auto systemFixedROMBank = make_unique<ROM>(GBXSystemROMPhysicalSize);
    auto systemFixedRAMBank = make_unique<RAM>(GBXSystemRAMPhysicalSize);

    // DMGBC User Mode memory map
    auto userFixedROMBank = make_unique<ROM>(DMGBCROMBankSize);
    auto userDynamicBank = make_unique<BankedROM>(DMGBCMaxDynamicROMSize, DMGBCROMBankSize); 
    auto userVideoRAM = make_unique<RAM>(DMGBCVideoRAMPhysicalSize);
    auto userVideoRAMPointer = userVideoRAM.get();
    auto userExternalRAM = make_unique<RAM>(DMGBCExternalRAMPhysicalSize);
    auto userWorkRAMBank0 = make_unique<RAM>(DMGBCSystemRAMBank0PhysicalSize);
    auto userWorkRAMBank1 = make_unique<RAM>(DMGBCSystemRAMBank1PhysicalSize);
    auto userMirrorRAM = make_unique<RAM>(DMGBCMirrorRAMPhysicalSize);
    auto userIORAM = make_unique<RAM>(DMGBCIORAMPhysicalSize);
    auto userHRAM = make_unique<RAM>(DMGBCHRAMPhysicalSize);

    memoryController->RegisterMemoryResource(std::move(systemFixedROMBank), AddressRange(GBXSystemROMInitialAddress, GBXSystemROMFinalAddress, RangeType::BeginInclusive), PrivilegeMode::System);
    memoryController->RegisterMemoryResource(std::move(systemFixedRAMBank), AddressRange(GBXSystemRAMInitialAddress, GBXSystemRAMFinalAddress, RangeType::BeginInclusive), PrivilegeMode::System);

    memoryController->RegisterMemoryResource(std::move(userFixedROMBank), AddressRange(DMGBCFixedROMInitialAddress, DMGBCFixedROMFinalAddress, RangeType::BeginInclusive), PrivilegeMode::User);
    memoryController->RegisterMemoryResource(std::move(userDynamicBank), AddressRange(DMGBCBankedROMInitialAddress, DMGBCBankedROMFinalAddress, RangeType::BeginInclusive), PrivilegeMode::User);
    memoryController->RegisterMemoryResource(std::move(userVideoRAM), AddressRange(DMGBCVideoRAMInitialAddress, DMGBCVideoRAMFinalAddress, RangeType::BeginInclusive), PrivilegeMode::User);
    memoryController->RegisterMemoryResource(std::move(userExternalRAM), AddressRange(DMGBCExternalRAMInitialAddress, DMGBCExternalRAMFinalAddress, RangeType::BeginInclusive), PrivilegeMode::User);
    memoryController->RegisterMemoryResource(std::move(userWorkRAMBank0), AddressRange(DMGBCSystemRAMBank0InitialAddress, DMGBCSystemRAMBank0FinalAddress, RangeType::BeginInclusive), PrivilegeMode::User);
    memoryController->RegisterMemoryResource(std::move(userWorkRAMBank1), AddressRange(DMGBCSystemRAMBank1InitialAddress, DMGBCSystemRAMBank1FinalAddress, RangeType::BeginInclusive), PrivilegeMode::User);
    memoryController->RegisterMemoryResource(std::move(userMirrorRAM), AddressRange(DMGBCMirrorRAMInitialAddress, DMGBCMirrorRAMFinalAddress, RangeType::BeginInclusive), PrivilegeMode::User);
    memoryController->RegisterMemoryResource(std::move(userIORAM), AddressRange(DMGBCIORAMInitialAddress, DMGBCIORAMFinalAddress, RangeType::BeginInclusive), PrivilegeMode::User);
    memoryController->RegisterMemoryResource(std::move(userHRAM), AddressRange(DMGBCHRAMInitialAddress, DMGBCHRAMFinalAddress, RangeType::BeginInclusive), PrivilegeMode::User);

    // Set Memory Controller to run in System Mode
    memoryController->SetSecurityLevel(PrivilegeMode::System);
    
    auto videoOutput = make_unique<OpenGLVideoOutput>(userVideoRAMPointer);
    videoOutput->Initialize();

    // Initialize Z80X CPU 
    // ADD VideoController Here
    _cpu.Initialize(std::move(controlUnit), std::move(clock), std::move(alu), std::move(memoryController), std::move(registers), std::move(videoOutput));
}

void GameBoyX::Run()
{
    _cpu.Run();
}

variant<uint8_t, uint16_t> GameBoyX::ReadRegister(interfaces::Register reg)
{
    if (RegisterBankInterface::IsPair(reg))
        return _registersPtr->ReadPair(reg);
    else
        return _registersPtr->Read(reg);
}

void GameBoyX::WriteRegister(interfaces::Register reg, variant<uint8_t, uint16_t> value)
{
    if (RegisterBankInterface::IsPair(reg))
        return _registersPtr->WritePair(reg, get<uint16_t>(value));
    else
        return _registersPtr->Write(reg, get<uint8_t>(value));
}

void GameBoyX::LoadGame(string gameROMName)
{
    auto oldMode = SecurityLevel();
    SetSecurityLevel(PrivilegeMode::User);

    try
    {
        // TODO: Make this RAII
        LoadROMBinary(gameROMName);
        SetSecurityLevel(oldMode);
    }
    catch (MemoryControllerException e)
    {
        SetSecurityLevel(oldMode);
        throw e;
    }
}

void GameBoyX::LoadBIOS(string BIOSROMName)
{
    auto oldMode = SecurityLevel();
    SetSecurityLevel(PrivilegeMode::System);

    try
    {
        // TODO: Make this RAII
        LoadBIOSBinary(BIOSROMName);
        SetSecurityLevel(oldMode);
    }
    catch (MemoryControllerException e)
    {
        SetSecurityLevel(oldMode);    
        throw e;
    }
}

void GameBoyX::LoadROMBinary(std::string ROMName)
{
    FileLoader loader(ROMName);
    loader.Load();   
    auto [fileBytes, size] = loader.FileData();

    if (size > DMGBCMaxDynamicROMSize + DMGBCROMBankSize)
        throw MemoryControllerException("Game ROM file larger than 2MB");

    // Load Static Bank (Bank 0)
    LoadStaticROMSection(reinterpret_cast<uint8_t*>(fileBytes.get()), size);

    // Load Dynamic Banks (Bank 1 - N)
    LoadDynamicROMSection(reinterpret_cast<uint8_t*>(fileBytes.get()), size);

    // Restore Bank to 0
    _memoryControllerPtr->SwitchBank(DMGBCBankedROMInitialAddress, 0llu);
}

void GameBoyX::LoadBIOSBinary(std::string ROMName)
{
    FileLoader loader(ROMName);
    loader.Load();   
    auto [fileBytes, size] = loader.FileData();

    if (size > GBXSystemROMPhysicalSize)
        throw MemoryControllerException("BIOS file larger than 32KB");

    auto ptr = make_unique<uint8_t*>(reinterpret_cast<uint8_t*>(fileBytes.get()));
    _memoryControllerPtr->Load(std::move(ptr), size < GBXSystemROMPhysicalSize? size : GBXSystemROMPhysicalSize, 0x0000, nullopt);
}

void GameBoyX::LoadStaticROMSection(uint8_t* fileBytes, size_t size)
{
    auto ptr = make_unique<uint8_t*>(fileBytes);
    _memoryControllerPtr->Load(std::move(ptr), size < DMGBCROMBankSize? size : DMGBCROMBankSize, 0x0000, nullopt);
    size -= DMGBCROMBankSize;
}

void GameBoyX::LoadDynamicROMSection(uint8_t* fileBytes, size_t size)
{
    for (auto offset = 0llu, bank = 0llu; offset < DMGBCMaxDynamicROMSize && size > 0; offset += DMGBCROMBankSize)
    {
        auto pointerAddress = offset + DMGBCROMBankSize;
        auto chunckSize = std::min(size, DMGBCROMBankSize);
        auto ptr = make_unique<uint8_t*>(fileBytes + pointerAddress);

        _memoryControllerPtr->SwitchBank(DMGBCBankedROMInitialAddress, bank++);
        _memoryControllerPtr->Load(std::move(ptr), chunckSize, DMGBCBankedROMInitialAddress, nullopt);

        size -= DMGBCROMBankSize;
    }
}

variant<uint8_t, uint16_t> GameBoyX::ReadROM(uint16_t address, std::optional<uint16_t> bank, interfaces::MemoryAccessType type)
{
    if (bank != nullopt)
        _memoryControllerPtr->SwitchBank(address, bank.value());

    return _memoryControllerPtr->Read(address, type);
}

gbxcore::SecurityLevel GameBoyX::SecurityLevel()
{
    return _memoryControllerPtr->SecurityLevel();
}

void GameBoyX::SetSecurityLevel(gbxcore::SecurityLevel level)
{
    _level = level;
    _memoryControllerPtr->SetSecurityLevel(_level);
}

}