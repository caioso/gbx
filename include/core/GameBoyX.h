#pragma once

#include <algorithm>
#include <memory>
#include <variant>

#include "ArithmeticLogicUnit.h"
#include "BankedROM.h"
#include "Clock.h"
#include "ControlUnit.h"
#include "FileLoader.h"
#include "MemoryController.h"
#include "OpenGLVideoOutput.h"
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
    void LoadBIOS(std::string) override;

    gbxcore::SecurityLevel SecurityLevel() override;
    void SetSecurityLevel(gbxcore::SecurityLevel) override;

    std::variant<uint8_t, uint16_t> ReadROM(uint16_t, std::optional<uint16_t>, interfaces::MemoryAccessType) override;

    void WriteRegister(interfaces::Register, std::variant<uint8_t, uint16_t>) override;
    std::variant<uint8_t, uint16_t> ReadRegister(interfaces::Register) override;

private:
    void LoadROMBinary(std::string);
    void LoadBIOSBinary(std::string);

    void LoadStaticROMSection(uint8_t*, size_t);
    void LoadDynamicROMSection(uint8_t*, size_t);

    gbxcore::SecurityLevel _level{};    

protected:
    bool IsPair(interfaces::Register);

    Z80X _cpu;
    memory::MemoryController* _memoryControllerPtr;
    RegisterBank* _registersPtr;
};

}