#include "InstructionCcf.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

void InstructionCcf::Decode(__attribute__((unused)) uint8_t opcode, __attribute__((unused)) optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::ccf,
        .AddressingMode = AddressingMode::Register,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::NoRegister,
        .DestinationRegister = Register::NoRegister,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };
}

void InstructionCcf::Execute(__attribute__((unused)) shared_ptr<RegisterBankInterface> registerBank, __attribute__((unused)) DecodedInstruction& decodedInstruction)
{
    auto cyValue = registerBank->ReadFlag(Flag::CY);
    cyValue = cyValue == static_cast<uint8_t>(0)? static_cast<uint8_t>(1) : static_cast<uint8_t>(0);

    registerBank->WriteFlag(Flag::CY, cyValue);
    registerBank->WriteFlag(Flag::H, 0);
    registerBank->WriteFlag(Flag::N, 0);
}

}