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
#include "SystemMode.h"
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

    gbxcore::Mode Mode() override;
    void SetMode(gbxcore::Mode) override;

    std::variant<uint8_t, uint16_t> ReadROM(uint16_t, std::optional<uint16_t>, interfaces::MemoryAccessType) override;

    void WriteRegister(interfaces::Register, std::variant<uint8_t, uint16_t>) override;
    std::variant<uint8_t, uint16_t> ReadRegister(interfaces::Register) override;

private:
    void LoadStaticROMSection(uint8_t*, size_t);
    void LoadDynamicROMSection(uint8_t*, size_t);

    gbxcore::Mode _mode{};    

protected:
    bool IsPair(interfaces::Register);

    std::shared_ptr<Z80X> _cpu;
    std::shared_ptr<ControlUnit> _controlUnit;
    std::shared_ptr<memory::MemoryController> _memoryController;      
    std::shared_ptr<ArithmeticLogicUnit> _alu;
    std::shared_ptr<Clock> _clock;
    std::shared_ptr<RegisterBank> _registers;
};

}