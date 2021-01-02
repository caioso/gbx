#pragma once

#include <memory>
#include <optional>
#include <variant>
#include <sstream>

#include <iostream>

#include "ArithmeticLogicUnitInterface.h"
#include "GBXExceptions.h"
#include "MemoryController.h"
#include "RegisterBank.h"
#include "instructions/Instruction.h"
#include "instructions/LD.h"

namespace gbx
{
class ArithmeticLogicUnit : public std::enable_shared_from_this<ArithmeticLogicUnit>, public ArithmeticLogicUnitInterface
{
public:
    ArithmeticLogicUnit();
    virtual ~ArithmeticLogicUnit() = default;

    virtual void RunCycle() override;

    void Initialize(std::shared_ptr<MemoryControllerInterface>);

protected:
    inline void Fetch();
    inline void FetchAgain();
    inline void Decode();
    inline void Execute();
    inline void AcquireOperand1();
    inline void AcquireOperand2();
    inline void AcquireOperand3();
    inline void WriteBack();

    inline uint8_t ReadAtRegister(Register);

    inline void InitializeRegisters();
    inline void DecodeInstruction();
    inline void ExecuteInstruction();
    inline void WriteBackResults();
    inline void DecideToAcquireOrExecute();
    inline void DecideToWriteBackOrFetchPC();
    inline void InstantiateInstruction(uint8_t);
    inline void IncrementPC();
    inline void CompleteFetchPC(uint8_t);
    inline void CompletePreOpcodeFetch(uint8_t);

    std::shared_ptr<RegisterBank> _registers;
    std::unique_ptr<Instruction> _currentInstruction;
    std::optional<uint8_t> _preOpcode;
    std::shared_ptr<MemoryControllerInterface> _memoryController;
    bool _shouldAcquireOperand;
    bool _shouldWriteBack;

private:
    struct AddressModeFormat
    {
        bool WriteBack;
    };
};

}