#pragma once

#include <algorithm>
#include <memory>
#include <variant>

#include "ArithmeticLogicUnit.h"
#include "BankedROM.h"
#include "Clock.h"
#include "ControlUnit.h"
#include "EngineParameters.h"
#include "FileLoader.h"
#include "MemoryController.h"
#include "RAM.h"
#include "RegisterBank.h"
#include "ROM.h"
#include "Runtime.h"
#include "SystemConstants.h"
#include "Z80X.h"

namespace gbxcore
{

class GameBoyX : public interfaces::Runtime
{
public:
    GameBoyX();
    virtual ~GameBoyX() = default;
    void Run() override;
    void LoadGame(std::string) override;
    
    std::variant<uint8_t, uint16_t> ReadRegister(interfaces::Register) override;
    std::variant<uint8_t, uint16_t> ReadROM(uint16_t, std::optional<uint16_t>, interfaces::MemoryAccessType) override;
    void WriteRegister(interfaces::Register, std::variant<uint8_t, uint16_t>) override;
    
protected:
    bool IsPair(interfaces::Register);

    std::shared_ptr<Z80X> _cpu;
    std::shared_ptr<ControlUnit> _controlUnit;
    std::shared_ptr<memory::MemoryController> _memoryController;
    
    // Cartrige ROM (16 KB [bank 0]) - 0x0000 - 0x3FFF
    std::shared_ptr<memory::ROM> _fixedUserROM;
    // Cartrige ROM (16KB [N bank (MBC-dependent)]) - 0x4000 - 0x7FFF
    std::shared_ptr<memory::BankedROM> _bankedUserROM;
    // Video RAM (8KB [bank 0] + 8KB [GBC only, bank 1] - 0x8000, 0x9FFF)
    std::shared_ptr<memory::RAM> _videoRAM;
    // External RAM (in-Cartridge RAM) (8KB [bank 0] + 8KB [bank n] - 0xA000, 0xBFFF)
    std::shared_ptr<memory::RAM> _externalRAM;
    // Work RAM 0 (system RAM) (4KB - bank 0)
    std::shared_ptr<memory::RAM> _workRAMBank0;
    // Work RAM 1 (system RAM) (4KB - bank 1)
    std::shared_ptr<memory::RAM> _workRAMBank1;
    // Unused (Mirror) Ram (7679 Bytes following the content of the Work Ram)
    std::shared_ptr<memory::RAM> _mirrorRAM;
    // I/O Registers (128 byte)
    std::shared_ptr<memory::RAM> _IORAM;
    // High RAM (STACK) (127 bytes)
    std::shared_ptr<memory::RAM> _HRAM;
    // Interrupt enabled
    std::shared_ptr<memory::RAM> _IE;
        
    
    std::shared_ptr<ArithmeticLogicUnit> _alu;
    std::shared_ptr<Clock> _clock;
    std::shared_ptr<RegisterBank> _registers;
};

}