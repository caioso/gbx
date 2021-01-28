#include "InstructionJr.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

void InstructionJr::Decode(__attribute__((unused)) uint8_t opcode, __attribute__((unused)) std::optional<uint8_t> preOpcode, interfaces::DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::jr,
        .AddressingMode = AddressingMode::Immediate,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::NoRegister,
        .DestinationRegister = Register::PC,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };
}

void InstructionJr::Execute(std::shared_ptr<interfaces::RegisterBankInterface> registerBank, interfaces::DecodedInstruction& decodedInstruction) 
{
    auto lsByte = decodedInstruction.MemoryOperand1;
    auto msByte = decodedInstruction.MemoryOperand2;
    registerBank->WritePair(Register::PC, lsByte | (msByte << 0x08));
    
}

}