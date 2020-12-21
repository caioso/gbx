#pragma once

#include <iostream>
#include <memory>
#include <variant>

#include "Channel.h"
#include "RegisterBank.h"
#include "ROM.h"

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
    std::shared_ptr<Channel<ROMMessage>> ALUROMChannel;

protected:
    void OnControlUnitMessage(ALUMessage message);
    void OnROMMessage(ROMMessage message);

    RegisterBank _registers;
};

}