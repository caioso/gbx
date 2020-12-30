#pragma once

#include <memory>
#include <optional>
#include <variant>
#include <sstream>

#include <iostream>

#include "Channel.h"
#include "GBXExceptions.h"
#include "MemoryController.h"
#include "RegisterBank.h"
#include "instructions/Instruction.h"
#include "instructions/LD.h"

namespace gbx
{

enum class ALUMessage
{
    Fetch,
    FetchOpcode,
    Decode,
    Execute,
    WriteBack,
    Acquire,
    
    ReadyToDecode,
    ReadyToExecute,
    ReadyToWriteBack,
    ReadyToFetch,
    ReadyToAcquire,
    ReadyToReadRealOpcode
};

enum class ALUState
{
    Idle,
    FetchingPC,
    Decoding,
    FetchingRealOpcode,
    FetchingOperand1,
    FetchingOperand2,
    Executing,
    WritingBack,
};

class ArithmeticLogicUnit : public std::enable_shared_from_this<ArithmeticLogicUnit>
{
public:
    ArithmeticLogicUnit();
    ~ArithmeticLogicUnit() = default;

    // Channels
    std::shared_ptr<Channel<ALUMessage>> ALUControlUnitChannel;
    std::shared_ptr<Channel<MemoryMessage>> ALUMemoryControllerChannel;

protected:
    inline void InitializeRegisters();
    inline void OnControlUnitMessage(ALUMessage);
    inline void OnMemoryControllerMessage(MemoryMessage);

    inline void DecodeInstruction();
    inline void ExecuteInstruction();
    inline void WriteBackResults();
    inline void DecideToAcquireOrExecute();
    inline void DecideToWriteBackOrFetchPC();

    inline void HandleControlSignalFetch();
    inline void HandleControlSignalFetchRealOpcode();
    inline void HandleMemoryResponseFetchPC(MemoryMessage);
    inline void HandleMemoryResponseFetchRealOpcode(MemoryMessage);
    inline void HandleMemoryResponseFetchOperand1(MemoryMessage);
    inline void HandleMemoryResponseFetchOperand2(MemoryMessage);
    inline void HandleMemoryResponseWriteBack(MemoryMessage);

    inline void HandleControlUnitSignalDecode();
    inline void HandleControlUnitSignalExecute();
    inline void HandleControlUnitSignalAcquire();
    inline void HandleControlUnitSignalWriteBack();

    inline void Decode(uint8_t);

    ALUState _state;
    std::shared_ptr<RegisterBank> _registers;
    std::unique_ptr<Instruction> _currentInstruction;
    std::optional<uint8_t> _preOpcode;
};

}