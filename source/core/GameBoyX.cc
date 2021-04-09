#include "GameBoyX.h"

using namespace gbxcore::constants;
using namespace gbxcore::interfaces;
using namespace gbxcore::memory;
using namespace gbxcommons;
using namespace std;

namespace gbxcore
{

GameBoyX::GameBoyX()
{
    _cpu = make_shared<Z80X>();
    _controlUnit = make_shared<ControlUnit>();
    _memoryController = make_shared<MemoryController>();
    _alu = make_shared<ArithmeticLogicUnit>();
    _clock = make_shared<Clock>(EngineParameters::GBCClockPeriod);
    _registers = make_shared<RegisterBank>();

    // GBX System Mode memory map
    auto systemFixedROMBank = make_unique<ROM>(GBXSystemROMPhysicalSize);
    auto systemFixedRAMBank = make_unique<RAM>(GBXSystemRAMPhysicalSize);

    // DMGBC User Mode memory map
    auto userFixedROMBank = make_unique<ROM>(DMGBCROMBankSize);
    auto userDynamicBank = make_unique<BankedROM>(DMGBCMaxDynamicROMSize, DMGBCROMBankSize); 
    auto userVideoRAM = make_unique<RAM>(DMGBCVideoRAMPhysicalSize);
    auto userExternalRAM = make_unique<RAM>(DMGBCExternalRAMPhysicalSize);
    auto userWorkRAMBank0 = make_unique<RAM>(DMGBCSystemRAMBank0PhysicalSize);
    auto userWorkRAMBank1 = make_unique<RAM>(DMGBCSystemRAMBank1PhysicalSize);
    auto userMirrorRAM = make_unique<RAM>(DMGBCMirrorRAMPhysicalSize);
    auto userIORAM = make_unique<RAM>(DMGBCIORAMPhysicalSize);
    auto userHRAM = make_unique<RAM>(DMGBCHRAMPhysicalSize);

    _memoryController->RegisterMemoryResource(std::move(systemFixedROMBank), AddressRange(GBXSystemROMInitialAddress, GBXSystemROMFinalAddress, RangeType::BeginInclusive), Ownership::System);
    _memoryController->RegisterMemoryResource(std::move(systemFixedRAMBank), AddressRange(GBXSystemRAMInitialAddress, GBXSystemRAMFinalAddress, RangeType::BeginInclusive), Ownership::System);

    _memoryController->RegisterMemoryResource(std::move(userFixedROMBank), AddressRange(DMGBCFixedROMInitialAddress, DMGBCFixedROMFinalAddress, RangeType::BeginInclusive), Ownership::User);
    _memoryController->RegisterMemoryResource(std::move(userDynamicBank), AddressRange(DMGBCBankedROMInitialAddress, DMGBCBankedROMFinalAddress, RangeType::BeginInclusive), Ownership::User);
    _memoryController->RegisterMemoryResource(std::move(userVideoRAM), AddressRange(DMGBCVideoRAMInitialAddress, DMGBCVideoRAMFinalAddress, RangeType::BeginInclusive), Ownership::User);
    _memoryController->RegisterMemoryResource(std::move(userExternalRAM), AddressRange(DMGBCExternalRAMInitialAddress, DMGBCExternalRAMFinalAddress, RangeType::BeginInclusive), Ownership::User);
    _memoryController->RegisterMemoryResource(std::move(userWorkRAMBank0), AddressRange(DMGBCSystemRAMBank0InitialAddress, DMGBCSystemRAMBank0FinalAddress, RangeType::BeginInclusive), Ownership::User);
    _memoryController->RegisterMemoryResource(std::move(userWorkRAMBank1), AddressRange(DMGBCSystemRAMBank1InitialAddress, DMGBCSystemRAMBank1FinalAddress, RangeType::BeginInclusive), Ownership::User);
    _memoryController->RegisterMemoryResource(std::move(userMirrorRAM), AddressRange(DMGBCMirrorRAMInitialAddress, DMGBCMirrorRAMFinalAddress, RangeType::BeginInclusive), Ownership::User);
    _memoryController->RegisterMemoryResource(std::move(userIORAM), AddressRange(DMGBCIORAMInitialAddress, DMGBCIORAMFinalAddress, RangeType::BeginInclusive), Ownership::User);
    _memoryController->RegisterMemoryResource(std::move(userHRAM), AddressRange(DMGBCHRAMInitialAddress, DMGBCHRAMFinalAddress, RangeType::BeginInclusive), Ownership::User);

    // Set Memory Controller to run in System Mode
    _memoryController->SetMode(Mode::System);
    
    // Initialize Z80X CPU
    _cpu->Initialize(_controlUnit, _clock, _alu, _memoryController, _registers);
}

void GameBoyX::Run()
{
    _cpu->Run();
}

variant<uint8_t, uint16_t> GameBoyX::ReadRegister(interfaces::Register reg)
{
    if (RegisterBankInterface::IsPair(reg))
        return _registers->ReadPair(reg);
    else
        return _registers->Read(reg);
}

void GameBoyX::WriteRegister(interfaces::Register reg, variant<uint8_t, uint16_t> value)
{
    if (RegisterBankInterface::IsPair(reg))
        return _registers->WritePair(reg, get<uint16_t>(value));
    else
        return _registers->Write(reg, get<uint8_t>(value));
}

void GameBoyX::LoadGame(string gameROMName)
{
    auto oldMode = Mode();
    SetMode(Mode::User);

    try
    {
        // TODO: Make this RAII
        LoadROMBinary(gameROMName);
        SetMode(oldMode);
    }
    catch (MemoryControllerException e)
    {
        SetMode(oldMode);
        throw e;
    }
}

void GameBoyX::LoadBIOS(string BIOSROMName)
{
    auto oldMode = Mode();
    SetMode(Mode::System);

    try
    {
        // TODO: Make this RAII
        LoadBIOSBinary(BIOSROMName);
        SetMode(oldMode);
    }
    catch (MemoryControllerException e)
    {
        SetMode(oldMode);    
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
    _memoryController->SwitchBank(DMGBCBankedROMInitialAddress, 0llu);
}

void GameBoyX::LoadBIOSBinary(std::string ROMName)
{
    FileLoader loader(ROMName);
    loader.Load();   
    auto [fileBytes, size] = loader.FileData();

    if (size > GBXSystemROMPhysicalSize)
        throw MemoryControllerException("BIOS file larger than 32KB");

    auto ptr = make_unique<uint8_t*>(reinterpret_cast<uint8_t*>(fileBytes.get()));
    _memoryController->Load(std::move(ptr), size < GBXSystemROMPhysicalSize? size : GBXSystemROMPhysicalSize, 0x0000, nullopt);
}

void GameBoyX::LoadStaticROMSection(uint8_t* fileBytes, size_t size)
{
    auto ptr = make_unique<uint8_t*>(fileBytes);
    _memoryController->Load(std::move(ptr), size < DMGBCROMBankSize? size : DMGBCROMBankSize, 0x0000, nullopt);
    size -= DMGBCROMBankSize;
}

void GameBoyX::LoadDynamicROMSection(uint8_t* fileBytes, size_t size)
{
    for (auto offset = 0llu, bank = 0llu; offset < DMGBCMaxDynamicROMSize && size > 0; offset += DMGBCROMBankSize)
    {
        auto pointerAddress = offset + DMGBCROMBankSize;
        auto chunckSize = std::min(size, DMGBCROMBankSize);
        auto ptr = make_unique<uint8_t*>(fileBytes + pointerAddress);

        _memoryController->SwitchBank(DMGBCBankedROMInitialAddress, bank++);
        _memoryController->Load(std::move(ptr), chunckSize, DMGBCBankedROMInitialAddress, nullopt);

        size -= DMGBCROMBankSize;
    }
}

variant<uint8_t, uint16_t> GameBoyX::ReadROM(uint16_t address, std::optional<uint16_t> bank, interfaces::MemoryAccessType type)
{
    if (bank != nullopt)
        _memoryController->SwitchBank(address, bank.value());

    return _memoryController->Read(address, type);
}

gbxcore::Mode GameBoyX::Mode()
{
    return _mode;
}

void GameBoyX::SetMode(gbxcore::Mode mode)
{
    _mode = mode;
    _memoryController->SetMode(_mode);
}

}