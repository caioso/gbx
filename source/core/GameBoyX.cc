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

    // Convention: At bootup, _fixedUserROM holds the system ROM at bank 0. After the initialization code is complete, Bank 1 will hold the cartridges's fixed bank
    _fixedUserROM = make_shared<ROM>(DefaultROMBankSize); // Fixed ROM Bank 
    _bankedUserROM = make_shared<BankedROM>(MaxDynamicBankROMSize, DefaultROMBankSize); 
    _videoRAM = make_shared<RAM>(VideoRAMPhysicalSize);
    _externalRAM = make_shared<RAM>(ExternalRAMPhysicalSize);
    _workRAMBank0 = make_shared<RAM>(SystemRAMBank0PhysicalSize);
    _workRAMBank1 = make_shared<RAM>(SystemRAMBank1PhysicalSize);
    _mirrorRAM = make_shared<RAM>(MirrorRAMPhysicalSize);
    _IORAM = make_shared<RAM>(IORAMPhysicalSize);
    _HRAM = make_shared<RAM>(HRAMPhysicalSize);
    _IE = make_shared<RAM>(1);

    _alu = make_shared<ArithmeticLogicUnit>();
    _clock = make_shared<Clock>(EngineParameters::GBCClockPeriod);
    _registers = make_shared<RegisterBank>();

    // Initialize Memory Controller
    // After it is available, execute (max 255 instructions) until it jumps to 0x0100, where the system ROM no more visible is, and the user ROM appears fully.
    _memoryController->RegisterMemoryResource(_fixedUserROM, AddressRange(UserFixedROMInitialAddress, UserFixedROMFinalAddress, RangeType::BeginInclusive), Ownership::System);
    _memoryController->RegisterMemoryResource(_bankedUserROM, AddressRange(UserBankedROMInitialAddress, UserBankedROMFinalAddress, RangeType::BeginInclusive), Ownership::System);
    _memoryController->RegisterMemoryResource(_videoRAM, AddressRange(VideoRAMInitialAddress, VideoRAMFinalAddress, RangeType::BeginInclusive), Ownership::System);
    _memoryController->RegisterMemoryResource(_externalRAM, AddressRange(ExternalRAMInitialAddress, ExternalRAMFinalAddress, RangeType::BeginInclusive), Ownership::System);
    _memoryController->RegisterMemoryResource(_workRAMBank0, AddressRange(SystemRAMBank0InitialAddress, SystemRAMBank0FinalAddress, RangeType::BeginInclusive), Ownership::System);
    _memoryController->RegisterMemoryResource(_workRAMBank1, AddressRange(SystemRAMBank1InitialAddress, SystemRAMBank1FinalAddress, RangeType::BeginInclusive), Ownership::System);
    _memoryController->RegisterMemoryResource(_mirrorRAM, AddressRange(MirrorRAMInitialAddress, MirrorRAMFinalAddress, RangeType::BeginInclusive), Ownership::System);
    _memoryController->RegisterMemoryResource(_IORAM, AddressRange(IORAMInitialAddress, IORAMFinalAddress, RangeType::BeginInclusive), Ownership::System);
    _memoryController->RegisterMemoryResource(_HRAM, AddressRange(HRAMInitialAddress, HRAMFinalAddress, RangeType::BeginInclusive), Ownership::System);
    _memoryController->RegisterMemoryResource(_IE, AddressRange(0xFFFF, 0xFFFF, RangeType::AllInclusive), Ownership::System);

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
    FileLoader loader(gameROMName);
    loader.Load();   
    auto [fileBytes, size] = loader.FileData();

    // Load Static Bank (Bank 0)
    LoadStaticROMSection(reinterpret_cast<uint8_t*>(fileBytes.get()), size);

    // Load Dynamic Banks (Bank 1 - N)
    LoadDynamicROMSection(reinterpret_cast<uint8_t*>(fileBytes.get()), size);

    // Restore Bank to 0
    _memoryController->SwitchBank(UserBankedROMInitialAddress, 0llu);
}

void GameBoyX::LoadStaticROMSection(uint8_t* fileBytes, size_t size)
{
    _memoryController->Load(make_shared<uint8_t*>(fileBytes), size < DefaultROMBankSize? size : DefaultROMBankSize, 0x0000, nullopt);
    size -= DefaultROMBankSize;
}

void GameBoyX::LoadDynamicROMSection(uint8_t* fileBytes, size_t size)
{
    for (auto offset = 0llu, bank = 0llu; offset < MaxDynamicBankROMSize && size > 0; offset += DefaultROMBankSize)
    {
        auto pointerAddress = offset + DefaultROMBankSize;
        auto chunckSize = std::min(size, DefaultROMBankSize);

        _memoryController->SwitchBank(UserBankedROMInitialAddress, bank++);
        _memoryController->Load(make_shared<uint8_t*>(fileBytes + pointerAddress), chunckSize, UserBankedROMInitialAddress, nullopt);

        size -= DefaultROMBankSize;
    }
}

variant<uint8_t, uint16_t> GameBoyX::ReadROM(uint16_t address, std::optional<uint16_t> bank, interfaces::MemoryAccessType type)
{
    if (bank != nullopt)
        _memoryController->SwitchBank(address, bank.value());

    return _memoryController->Read(address, type);
}

}