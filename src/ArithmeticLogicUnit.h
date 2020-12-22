#pragma once

#include <iostream>
#include <memory>
#include <variant>

#include "Channel.h"
#include "MemoryController.h"
#include "RegisterBank.h"

namespace gbx
{

enum class ALUMessage
{
    FetchPC
};

class ArithmeticLogicUnit
{
public:
    ArithmeticLogicUnit();
    ~ArithmeticLogicUnit() = default;

    // Channels
    std::shared_ptr<Channel<ALUMessage>> ALUControlUnitChannel;
    std::shared_ptr<Channel<MemoryMessage>> ALUMemoryControllerChannel;

protected:
    void InitializeRegisters();
    void OnControlUnitMessage(ALUMessage message);
    void OnMemoryControllerMessage(MemoryMessage message);

    RegisterBank _registers;
};

}