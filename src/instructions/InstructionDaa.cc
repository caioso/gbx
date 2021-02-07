#include "InstructionDaa.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{
void InstructionDaa::Decode(__attribute__((unused)) uint8_t opcode, __attribute__((unused)) optional<uint8_t> preOpcode, DecodedInstruction& decodedInstruction)
{
    decodedInstruction =
    {
        .Opcode = OpcodeType::daa,
        .AddressingMode = AddressingMode::Register,
        .MemoryOperand1 = 0x00,
        .MemoryOperand2 = 0x00,
        .MemoryOperand3 = 0x00,
        .SourceRegister = Register::A,
        .DestinationRegister = Register::NoRegister,
        .MemoryResult1 = 0x00,
        .MemoryResult2 = 0x00,
        .InstructionExtraOperand = 0x00
    };
}

void InstructionDaa::Execute(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto result = static_cast<uint8_t>(0x00);

    if (registerBank->ReadFlag(Flag::N) == 0x00)
        result = ExecuteAdditionDaa(registerBank, decodedInstruction);
    else
        result = ExecuteSubtractionDaa(registerBank, decodedInstruction);

    if (result == 0x00)
        registerBank->SetFlag(Flag::Z);
    else
        registerBank->ClearFlag(Flag::Z);

    registerBank->Write(decodedInstruction.SourceRegister, result);
}

uint8_t InstructionDaa::ExecuteAdditionDaa(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto cyFlag = registerBank->ReadFlag(Flag::CY);
    auto hFlag = registerBank->ReadFlag(Flag::H);
    auto lowNibble = static_cast<uint8_t>((registerBank->Read(decodedInstruction.SourceRegister) & 0x0F));
    auto highNibble = static_cast<uint8_t>(((registerBank->Read(decodedInstruction.SourceRegister) >> 0x04) & 0x0F));
    auto result = static_cast<uint8_t>(0x00);

    if (cyFlag == 0x00 && hFlag == 0x00)
    {
        if (lowNibble <= 0x09 && highNibble <= 0x09)
        {
            result = highNibble << 0x04 | lowNibble;
            registerBank->ClearFlag(Flag::CY);    
        }
        else if (lowNibble >= 0x0A && highNibble <= 0x08)
        {
            result = highNibble << 0x04 | lowNibble;
            result += 0x06;
            registerBank->ClearFlag(Flag::CY);    
        }
        else if (lowNibble <= 0x09 && highNibble >= 0x0A)
        {
            result = highNibble << 0x04 | lowNibble;
            result += 0x60;
            registerBank->SetFlag(Flag::CY);    
        }
        else if (lowNibble >= 0x0A && highNibble >= 0x09)
        {
            result = highNibble << 0x04 | lowNibble;
            result += 0x66;
            registerBank->SetFlag(Flag::CY);    
        }
    }
    else if (cyFlag == 0x00 && hFlag == 0x01)
    {
        if (lowNibble <= 0x03 && highNibble <= 0x09)
        {
            result = highNibble << 0x04 | lowNibble;
            result += 0x06;
            registerBank->ClearFlag(Flag::CY);    
        }
        else if (lowNibble <= 0x03 && highNibble >= 0x0A)
        {
            result = highNibble << 0x04 | lowNibble;
            result += 0x66;
            registerBank->SetFlag(Flag::CY);    
        }
    }
    else if (cyFlag == 0x01 && hFlag == 0x00)
    {
        if (lowNibble <= 0x09 && highNibble <= 0x02)
        {
            result = highNibble << 0x04 | lowNibble;
            result += 0x60;
            registerBank->SetFlag(Flag::CY);    
        }
        else if (lowNibble >= 0x0A && highNibble <= 0x02)
        {
            result = highNibble << 0x04 | lowNibble;
            result += 0x66;
            registerBank->SetFlag(Flag::CY);    
        }
    }
    else if (cyFlag == 0x01 && hFlag == 0x01)
    {
        if (lowNibble <= 0x03 && highNibble <= 0x03)
        {
            result = highNibble << 0x04 | lowNibble;
            result += 0x66;
            registerBank->SetFlag(Flag::CY);    
        }
    }

    return result;
}

uint8_t InstructionDaa::ExecuteSubtractionDaa(shared_ptr<RegisterBankInterface> registerBank, DecodedInstruction& decodedInstruction)
{
    auto cyFlag = registerBank->ReadFlag(Flag::CY);
    auto hFlag = registerBank->ReadFlag(Flag::H);
    auto lowNibble = static_cast<uint8_t>((registerBank->Read(decodedInstruction.SourceRegister) & 0x0F));
    auto highNibble = static_cast<uint8_t>(((registerBank->Read(decodedInstruction.SourceRegister) >> 0x04) & 0x0F));
    auto result = static_cast<uint8_t>(0x00);

    if (cyFlag == 0x00 && hFlag == 0x00)
    {
        if (lowNibble <= 0x09 && highNibble <= 0x09)
        {
            result = highNibble << 0x04 | lowNibble;
            registerBank->ClearFlag(Flag::CY);    
        }
    }
    else if (cyFlag == 0x00 && hFlag == 0x01)
    {
        if (lowNibble >= 0x06 && highNibble <= 0x08)
        {
            result = highNibble << 0x04 | lowNibble;
            result += 0xFA;
            registerBank->ClearFlag(Flag::CY);    
        }
    }
    else if (cyFlag == 0x01 && hFlag == 0x00)
    {
        if (lowNibble <= 0x09 && highNibble >= 0x07)
        {
            result = highNibble << 0x04 | lowNibble;
            result += 0xA0;
            registerBank->SetFlag(Flag::CY);    
        }
    }
    else if (cyFlag == 0x01 && hFlag == 0x01)
    {
        if (lowNibble >= 0x06 && highNibble >= 0x06)
        {
            result = highNibble << 0x04 | lowNibble;
            result += 0x9A;
            registerBank->SetFlag(Flag::CY);    
        }
    }
    
    return result;
}

}