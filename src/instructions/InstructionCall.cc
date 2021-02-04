#include "InstructionCall.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

void InstructionCall::Decode(uint8_t opcode, __attribute__((unused)) std::optional<uint8_t> preOpcode, interfaces::DecodedInstruction& decodedInstruction)
{
    if (opcode == 0x0CD) 
        DecodeUnconditionalCall(decodedInstruction);
    else  
        DecodeConditionalCall(opcode, decodedInstruction);
}

bool InstructionCall::ConditionallyExecute(std::shared_ptr<interfaces::RegisterBankInterface> registerBank, interfaces::DecodedInstruction& decodedInstruction)
{
    if (decodedInstruction.InstructionExtraOperand == 0xFF)
        return ExecuteUnconditionalCall(registerBank, decodedInstruction);
    else
        return ExecuteConditionalCall(registerBank, decodedInstruction);
}

inline bool InstructionCall::ExecuteUnconditionalCall(std::shared_ptr<interfaces::RegisterBankInterface> registerBank, interfaces::DecodedInstruction& decodedInstruction)
{
    auto sourceValue = registerBank->ReadPair(decodedInstruction.DestinationRegister);
    auto newPCAddress = static_cast<uint16_t>(decodedInstruction.MemoryOperand1 | (decodedInstruction.MemoryOperand2 << 8));
    
    registerBank->WritePair(decodedInstruction.DestinationRegister, newPCAddress);

    decodedInstruction.MemoryResult1 = static_cast<uint8_t>((sourceValue >> 8) & 0xFF);
    decodedInstruction.MemoryResult2 = static_cast<uint8_t>(sourceValue & 0xFF);
    return false;
}

inline bool InstructionCall::ExecuteConditionalCall(std::shared_ptr<interfaces::RegisterBankInterface> registerBank, interfaces::DecodedInstruction& decodedInstruction)
{
    auto condition = decodedInstruction.InstructionExtraOperand;
    auto zFlag = registerBank->ReadFlag(Flag::Z);
    auto cyFlag = registerBank->ReadFlag(Flag::CY);

    if ((condition == 0x00 && zFlag == 0x00) || (condition == 0x01 && zFlag == 0x01) ||
        (condition == 0x02 && cyFlag == 0x00) || (condition == 0x03 && cyFlag == 0x01))
        return ExecuteUnconditionalCall(registerBank, decodedInstruction);
    else
        return true;
}

inline void InstructionCall::DecodeUnconditionalCall(interfaces::DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::call,
        .AddressingMode = AddressingMode::SubRoutineCall,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::NoRegister,
        .DestinationRegister = Register::PC,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0xFF
    };
}

inline void InstructionCall::DecodeConditionalCall(uint8_t opcode, interfaces::DecodedInstruction& decodedInstruction)
{
    auto conditionalFlag = static_cast<uint8_t>((opcode >> 0x03) & 0x03);

    decodedInstruction =
    {
        .Opcode = OpcodeType::call,
        .AddressingMode = AddressingMode::SubRoutineCall,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::NoRegister,
        .DestinationRegister = Register::PC,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = conditionalFlag
    };

}

}