#pragma once

#include <iostream>
#include <memory>
#include <variant>

#include "Channel.h"
#include "GBXExceptions.h"
#include "MemoryController.h"
#include "RegisterBank.h"

namespace gbx
{

enum class ALUMessage
{
    FetchPC,
    Decode,
    Complete
};

enum class ALUState
{
    Idle,
    FetchingPC,
    Decoding
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

    void DecodeInstruction();

    void HandleControlSignalFetchPC();
    void HandleMemoryResponseFetchPC(MemoryMessage message);

    void HandleControlUnitSignalDecode();

    RegisterBank _registers;
    ALUState _state;
};

}