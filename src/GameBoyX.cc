#include "GameBoyX.h"

using namespace gbx::constants;
using namespace std;

namespace gbx
{

GameBoyX::GameBoyX()
{
    _cpu = make_shared<Z80X>();
    _controlUnit = make_shared<ControlUnit>();
    _memoryController = make_shared<MemoryController>();
    
    _userROM = make_shared<ROM>(UserROMPhysicalSize); 
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
    _memoryController->RegisterMemoryResource(_userROM, AddressRange(UserROMInitialAddress, UserROMFinalAddress, RangeType::BeginInclusive));
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

}