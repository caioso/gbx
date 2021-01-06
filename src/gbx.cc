#include <iostream>
#include <sstream>

#include "Version.h"
#include "Logger.h"
#include "GameBoyX.h"

#include "interfaces/MemoryInterface.h"

using namespace std;
using namespace gbx;
using namespace gbx::interfaces;

// Test Only
class GbxDecorator : public GameBoyX
{
public:
    std::shared_ptr<Z80X> GetCPU() { return _cpu; }
    std::shared_ptr<ControlUnit> GetControlUnit() { return _controlUnit; }
    std::shared_ptr<MemoryController> GetMemoryController() { return _memoryController; }
    std::shared_ptr<ROM> GetROM() { return _systemROM; }
    std::shared_ptr<ArithmeticLogicUnit> GetALU() { return _alu; }
    std::shared_ptr<Clock> GetClock() { return _clock; }
};

int main ()
{
    stringstream message;
    message << "GAME BOY X Emulator " << SystemVersion();
    Log::LogLn(message.str().c_str());

    auto gbx = make_unique<GbxDecorator>();

    // SetUp Memory
    uint8_t content[] = 
    {
        static_cast<uint8_t>(0x3E), // LD A, #0x00
        static_cast<uint8_t>(0x00),
        static_cast<uint8_t>(0x06), // LD B, #0x01
        static_cast<uint8_t>(0x01),
        static_cast<uint8_t>(0x21), // LD HL, #0x0100
        static_cast<uint8_t>(0x00),
        static_cast<uint8_t>(0x01),
        static_cast<uint8_t>(0x22), // LD (HL+), A
        static_cast<uint8_t>(0x78), // LD A, B
        static_cast<uint8_t>(0x22), // LD (HL+), A
        static_cast<uint8_t>(0x3E), // LD A, #0x00
        static_cast<uint8_t>(0x00),
        // -----------------
        static_cast<uint8_t>(0x80), // ADD B
        static_cast<uint8_t>(0x4F), // LD C, A
        static_cast<uint8_t>(0x22), // LD (HL+), A
        static_cast<uint8_t>(0x78), // LD A, B
        static_cast<uint8_t>(0x41), // LD B, C
        // -----------------
        static_cast<uint8_t>(0x80), // ADD B
        static_cast<uint8_t>(0x4F), // LD C, A
        static_cast<uint8_t>(0x22), // LD (HL+), A
        static_cast<uint8_t>(0x78), // LD A, B
        static_cast<uint8_t>(0x41), // LD B, C
        // -----------------
        static_cast<uint8_t>(0x80), // ADD B
        static_cast<uint8_t>(0x4F), // LD C, A
        static_cast<uint8_t>(0x22), // LD (HL+), A
        static_cast<uint8_t>(0x78), // LD A, B
        static_cast<uint8_t>(0x41), // LD B, C
        // -----------------
        static_cast<uint8_t>(0x80), // ADD B
        static_cast<uint8_t>(0x4F), // LD C, A
        static_cast<uint8_t>(0x22), // LD (HL+), A
        static_cast<uint8_t>(0x78), // LD A, B
        static_cast<uint8_t>(0x41), // LD B, C
        // -----------------
        static_cast<uint8_t>(0x80), // ADD B
        static_cast<uint8_t>(0x4F), // LD C, A
        static_cast<uint8_t>(0x22), // LD (HL+), A
        static_cast<uint8_t>(0x78), // LD A, B
        static_cast<uint8_t>(0x41), // LD B, C
        // -----------------
        static_cast<uint8_t>(0x80), // ADD B
        static_cast<uint8_t>(0x4F), // LD C, A
        static_cast<uint8_t>(0x22), // LD (HL+), A
        static_cast<uint8_t>(0x78), // LD A, B
        static_cast<uint8_t>(0x41), // LD B, C
        // -----------------
        static_cast<uint8_t>(0x80), // ADD B
        static_cast<uint8_t>(0x4F), // LD C, A
        static_cast<uint8_t>(0x22), // LD (HL+), A
        static_cast<uint8_t>(0x78), // LD A, B
        static_cast<uint8_t>(0x41), // LD B, C
        // -----------------
        static_cast<uint8_t>(0x80), // ADD B
        static_cast<uint8_t>(0x4F), // LD C, A
        static_cast<uint8_t>(0x22), // LD (HL+), A
        static_cast<uint8_t>(0x78), // LD A, B
        static_cast<uint8_t>(0x41), // LD B, C
        // -----------------
        static_cast<uint8_t>(0x80), // ADD B
        static_cast<uint8_t>(0x4F), // LD C, A
        static_cast<uint8_t>(0x22), // LD (HL+), A
        static_cast<uint8_t>(0x78), // LD A, B
        static_cast<uint8_t>(0x41), // LD B, C
        // -----------------
        static_cast<uint8_t>(0x80), // ADD B
        static_cast<uint8_t>(0x4F), // LD C, A
        static_cast<uint8_t>(0x22), // LD (HL+), A
        static_cast<uint8_t>(0x78), // LD A, B
        static_cast<uint8_t>(0x41), // LD B, C
    };

    gbx->GetROM()->Load(make_shared<uint8_t*>(content), 100, 0);

    auto cycleCounter = 0;
    try
    {
        while (true)
        {
            gbx->Run();
            cycleCounter++;
        }
    }    
    catch (const gbx::InstructionException&)
    {
        cout << "Program Terminated at cycle " << cycleCounter << '\n';
    }

    cout << "Memory content (0x0100 ~ 0x010b)" << '\n';
    for (auto i = static_cast<uint8_t>(0); i < static_cast<uint8_t>(12); i++)
    {
        auto value = gbx->GetROM()->Read(static_cast<uint16_t>(0x0100 + i), MemoryAccessType::Byte);

        cout << hex << "0x0" << static_cast<uint16_t>(0x0100 + i) << dec << ": " << static_cast<size_t>(get<uint8_t>(value)) << '\n';
    }
}