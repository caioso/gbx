#pragma once

#include <iostream>
#include <memory>
#include <variant>
#include <sstream>

#include "Channel.h"
#include "GBXExceptions.h"
#include "instructions/Instruction.h"
#include "MemoryController.h"
#include "RegisterBank.h"

namespace gbx
{

enum class ALUMessage
{
    Fetch,
    Decode,
    Execute,
    WriteBack,
    Acquire,
    
    ReadyToDecode,
    ReadyToExecute,
    ReadyToWriteBack,
    ReadyToFetch,
    ReadyToAcquire
};

enum class ALUState
{
    Idle,
    FetchingPC,
    Decoding,
    FethcingOperand1,
    FetchingOperand2,
    Executing,
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
    void DecideToAcquireOrExecute();
    void DecideToWriteBackOrFetchPC();

    void HandleControlSignalFetch();
    void HandleMemoryResponseFetchPC(MemoryMessage message);
    void HandleMemoryResponseFetchOperand1(MemoryMessage message);

    void HandleControlUnitSignalDecode();
    void HandleControlUnitSignalExecute();
    void HandleControlUnitSignalAcquire();

    DecodedInstruction Decode(uint8_t opcode);

    RegisterBank _registers;
    ALUState _state;
    DecodedInstruction _currentInstruction;
};

}