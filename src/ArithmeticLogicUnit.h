#pragma once

#include <iostream>
#include <memory>
#include <variant>
#include <sstream>

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
    Execute,
};

enum class ALUState
{
    Idle,
    FetchingPC,
    Decoding,
    Executing,
    Complete
};

enum class InstructionState
{
    NoInstruction,
    ReadyToExecute,
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
    void ExecuteInstruction();
    void EvaluateInstructionDependencies();

    void HandleControlSignalFetchPC();
    void HandleMemoryResponseFetchPC(MemoryMessage message);

    void HandleControlUnitSignalDecode();
    void HandleControlUnitSignalExecute();

    RegisterBank _registers;
    ALUState _state;
    InstructionState _instructionState;
};

}