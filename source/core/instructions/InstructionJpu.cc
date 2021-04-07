#include "InstructionJpu.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionJpu::Decode([[maybe_unused]] uint8_t opcode, [[maybe_unused]] std::optional<uint8_t> preOpcode, interfaces::DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::jpu,
        .AddressingMode = AddressingMode::ImmediatePair,
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

void InstructionJpu::Execute(std::shared_ptr<interfaces::RegisterBankInterface> registerBank, interfaces::DecodedInstruction& decodedInstruction) 
{
    auto lsByte = decodedInstruction.MemoryOperand1;
    auto msByte = decodedInstruction.MemoryOperand2;
    registerBank->WritePair(Register::PC, lsByte | (msByte << 0x08));
}

}