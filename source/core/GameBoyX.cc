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
    auto _fixedUserROM = make_unique<ROM>(DefaultROMBankSize); // Fixed ROM Bank 
    auto _bankedUserROM = make_unique<BankedROM>(MaxDynamicBankROMSize, DefaultROMBankSize); 
    auto _videoRAM = make_unique<RAM>(VideoRAMPhysicalSize);
    auto _externalRAM = make_unique<RAM>(ExternalRAMPhysicalSize);
    auto _workRAMBank0 = make_unique<RAM>(SystemRAMBank0PhysicalSize);
    auto _workRAMBank1 = make_unique<RAM>(SystemRAMBank1PhysicalSize);
    auto _mirrorRAM = make_unique<RAM>(MirrorRAMPhysicalSize);
    auto _IORAM = make_unique<RAM>(IORAMPhysicalSize);
    auto _HRAM = make_unique<RAM>(HRAMPhysicalSize);
    auto _IE = make_unique<RAM>(1);

    auto _alu = make_shared<ArithmeticLogicUnit>();
    auto _clock = make_shared<Clock>(EngineParameters::GBCClockPeriod);
    auto _registers = make_shared<RegisterBank>();

    // Initialize Memory Controller
    // After it is available, execute (max 255 instructions) until it jumps to 0x0100, where the system ROM no more visible is, and the user ROM appears fully.
    _memoryController->RegisterMemoryResource(std::move(_fixedUserROM), AddressRange(UserFixedROMInitialAddress, UserFixedROMFinalAddress, RangeType::BeginInclusive), Ownership::User);
    _memoryController->RegisterMemoryResource(std::move(_bankedUserROM), AddressRange(UserBankedROMInitialAddress, UserBankedROMFinalAddress, RangeType::BeginInclusive), Ownership::User);
    _memoryController->RegisterMemoryResource(std::move(_videoRAM), AddressRange(VideoRAMInitialAddress, VideoRAMFinalAddress, RangeType::BeginInclusive), Ownership::User);
    _memoryController->RegisterMemoryResource(std::move(_externalRAM), AddressRange(ExternalRAMInitialAddress, ExternalRAMFinalAddress, RangeType::BeginInclusive), Ownership::User);
    _memoryController->RegisterMemoryResource(std::move(_workRAMBank0), AddressRange(SystemRAMBank0InitialAddress, SystemRAMBank0FinalAddress, RangeType::BeginInclusive), Ownership::User);
    _memoryController->RegisterMemoryResource(std::move(_workRAMBank1), AddressRange(SystemRAMBank1InitialAddress, SystemRAMBank1FinalAddress, RangeType::BeginInclusive), Ownership::User);
    _memoryController->RegisterMemoryResource(std::move(_mirrorRAM), AddressRange(MirrorRAMInitialAddress, MirrorRAMFinalAddress, RangeType::BeginInclusive), Ownership::User);
    _memoryController->RegisterMemoryResource(std::move(_IORAM), AddressRange(IORAMInitialAddress, IORAMFinalAddress, RangeType::BeginInclusive), Ownership::User);
    _memoryController->RegisterMemoryResource(std::move(_HRAM), AddressRange(HRAMInitialAddress, HRAMFinalAddress, RangeType::BeginInclusive), Ownership::User);
    _memoryController->RegisterMemoryResource(std::move(_IE), AddressRange(0xFFFF, 0xFFFF, RangeType::AllInclusive), Ownership::User);

    // Initialize Z80X CPU
    _cpu->Initialize(_controlUnit, _clock, _alu, _memoryController, _registers);
    
    // Set Memory Controller to run in System Mode
    _memoryController->SetMode(Mode::System);
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
    auto ptr = make_unique<uint8_t*>(fileBytes);
    _memoryController->Load(std::move(ptr), size < DefaultROMBankSize? size : DefaultROMBankSize, 0x0000, nullopt);
    size -= DefaultROMBankSize;
}

void GameBoyX::LoadDynamicROMSection(uint8_t* fileBytes, size_t size)
{
    for (auto offset = 0llu, bank = 0llu; offset < MaxDynamicBankROMSize && size > 0; offset += DefaultROMBankSize)
    {
        auto pointerAddress = offset + DefaultROMBankSize;
        auto chunckSize = std::min(size, DefaultROMBankSize);
        auto ptr = make_unique<uint8_t*>(fileBytes + pointerAddress);

        _memoryController->SwitchBank(UserBankedROMInitialAddress, bank++);
        _memoryController->Load(std::move(ptr), chunckSize, UserBankedROMInitialAddress, nullopt);

        size -= DefaultROMBankSize;
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