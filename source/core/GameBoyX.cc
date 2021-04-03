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
    _fixedUserROM = make_shared<BankedROM>(FixedBankROMSize, DefaultROMBankSize); // Fixed ROM Bank 

    // WARNING! THIS IS JUST A TEST
    _fixedUserROM->SelectBank(1);

    _bankedUserROM = make_shared<BankedROM>(MBC1DynamicBankROMSize, DefaultROMBankSize); 
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
    // _memoryController->RegisterMemoryResource(_systemROM, AddressRange(SystemROMInitialAddress, SystemROMFinalAddress, RangeType::BeginInclusive)); 
    _memoryController->RegisterMemoryResource(_fixedUserROM, AddressRange(UserFixedROMInitialAddress, UserFixedROMFinalAddress, RangeType::BeginInclusive));
    _memoryController->RegisterMemoryResource(_bankedUserROM, AddressRange(UserBankedROMInitialAddress, UserBankedROMFinalAddress, RangeType::BeginInclusive));
    _memoryController->RegisterMemoryResource(_videoRAM, AddressRange(VideoRAMInitialAddress, VideoRAMFinalAddress, RangeType::BeginInclusive));
    _memoryController->RegisterMemoryResource(_externalRAM, AddressRange(ExternalRAMInitialAddress, ExternalRAMFinalAddress, RangeType::BeginInclusive));
    _memoryController->RegisterMemoryResource(_workRAMBank0, AddressRange(SystemRAMBank0InitialAddress, SystemRAMBank0FinalAddress, RangeType::BeginInclusive));
    _memoryController->RegisterMemoryResource(_workRAMBank1, AddressRange(SystemRAMBank1InitialAddress, SystemRAMBank1FinalAddress, RangeType::BeginInclusive));
    _memoryController->RegisterMemoryResource(_mirrorRAM, AddressRange(MirrorRAMInitialAddress, MirrorRAMFinalAddress, RangeType::BeginInclusive));
    _memoryController->RegisterMemoryResource(_IORAM, AddressRange(IORAMInitialAddress, IORAMFinalAddress, RangeType::BeginInclusive));
    _memoryController->RegisterMemoryResource(_HRAM, AddressRange(HRAMInitialAddress, HRAMFinalAddress, RangeType::BeginInclusive));
    _memoryController->RegisterMemoryResource(_IE, AddressRange(0xFFFF, 0xFFFF, RangeType::AllInclusive));

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

    // Load Bank 0
    _memoryController->Load(make_shared<uint8_t*>(fileBytes.get()), DefaultROMBankSize, 0x0000, 0x0000);
}

variant<uint8_t, uint16_t> GameBoyX::ReadROM(uint16_t address, uint16_t bank, interfaces::MemoryAccessType type)
{
    return _memoryController->Read(address, type);
}

}