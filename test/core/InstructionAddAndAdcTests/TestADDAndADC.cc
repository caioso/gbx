#include <gtest/gtest.h>

#include "CoreTestMocksAndWrappers.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <vector>

#include "MemoryController.h"
#include "RegisterBank.h"
#include "ArithmeticLogicUnit.h"
#include "Opcodes.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace gbxcore::instructions;

TEST(CoreTests_ADDAndADC, DecodeAddRegisterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto binaryBase = 0x80;
    auto sourceList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};

    for (auto source : sourceList)
    {
        auto rawBinary = binaryBase | RegisterBank::ToInstructionSource(source);
        alu.DecodeInstruction(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::add, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(source, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(CoreTests_ADDAndADC, DecodeAddImmediateMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0xC6;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::add, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Immediate, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(CoreTests_ADDAndADC, DecodeAddRegisterIndirectMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0x86;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::add, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSource, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(CoreTests_ADDAndADC, DecodeAdcRegisterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto sourceList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};

    for (auto source : sourceList)
    {
        auto rawBinary =  0x88 | RegisterBank::ToInstructionSource(source);
        alu.DecodeInstruction(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::adc, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(source, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(CoreTests_ADDAndADC, DecodeAdcRegisterImmediate)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto rawBinary =  0xCE;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::adc, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Immediate, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(CoreTests_ADDAndADC, DecodeAdcRegisterIndirect)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto rawBinary =  0x8E;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::adc, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSource, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(CoreTests_ADDAndADC, ExecuteAddRegisterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto sourceList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<int8_t> distribution{-100, 100};
    vector<int8_t> registerAValues;
    vector<int8_t> registerSourceValues;

    for (auto i = 0; i < 7; i++)
        registerAValues.push_back(distribution(engine));

    for (auto i = 0; i < 7; i++)
        registerSourceValues.push_back(distribution(engine));

    auto counter = static_cast<size_t>(0);
    for (auto source : sourceList)
    {
        auto aValue = registerAValues[counter];
        auto sourceValue = registerSourceValues[counter];
        auto result = static_cast<uint8_t>(0);

         if(source != Register::A)
         {
            result = aValue + sourceValue;
            registerBank->Write(Register::A, aValue);
            registerBank->Write(source, sourceValue);
         }
         else
         {
            result = aValue + aValue;
            registerBank->Write(Register::A, aValue);
         }

        auto rawBinary = 0x80 | RegisterBank::ToInstructionSource(source);
        alu.DecodeInstruction(rawBinary, nullopt);
        alu.Execute();

        EXPECT_EQ(registerBank->Read(Register::A), static_cast<uint8_t>(result));
        counter++;
    }

    // Test Flags
    registerBank->Write(Register::A, 0xFF);
    registerBank->Write(Register::B, 0x01);

    auto rawBinary = 0x80 | RegisterBank::ToInstructionSource(Register::B);

    alu.DecodeInstruction(rawBinary, nullopt);
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank->Read(Register::A));

    registerBank->Write(Register::A, 0x0F);
    registerBank->Write(Register::C, 0x01);

    rawBinary = 0x80 | RegisterBank::ToInstructionSource(Register::C);

    alu.DecodeInstruction(rawBinary, nullopt);
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x10, registerBank->Read(Register::A));

    registerBank->Write(Register::A, 0xF0);
    registerBank->Write(Register::D, 0x80);

    rawBinary = 0x80 | RegisterBank::ToInstructionSource(Register::D);

    alu.DecodeInstruction(rawBinary, nullopt);
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x70, registerBank->Read(Register::A));

    registerBank->Write(Register::A, 0x00);
    registerBank->Write(Register::E, 0x00);

    rawBinary = 0x80 | RegisterBank::ToInstructionSource(Register::E);

    alu.DecodeInstruction(rawBinary, nullopt);
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank->Read(Register::A));

    registerBank->Write(Register::A, 0x01);
    registerBank->Write(Register::H, 0x01);

    rawBinary = 0x80 | RegisterBank::ToInstructionSource(Register::H);

    alu.DecodeInstruction(rawBinary, nullopt);
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x02, registerBank->Read(Register::A));
}

TEST(CoreTests_ADDAndADC, ExecuteAddImmediateMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<int8_t> distribution{-100, 100};
    vector<int8_t> registerAValues;
    vector<int8_t> memoryOperandValues;

    for (auto i = 0; i < 7; i++)
        registerAValues.push_back(distribution(engine));

    for (auto i = 0; i < 7; i++)
        memoryOperandValues.push_back(distribution(engine));

    for (auto i = 0; i < 7; i++)
    {
        auto aValue = registerAValues[i];
        auto memoryOperandValue = memoryOperandValues[i];
        auto result = static_cast<uint8_t>(0);

        result = aValue + memoryOperandValue;

        auto rawBinary = 0xC6;
        alu.DecodeInstruction(rawBinary, nullopt);
        registerBank->Write(Register::A, aValue);
        alu.GetInstructionData().MemoryOperand1 = memoryOperandValue;
        alu.Execute();

        EXPECT_EQ(registerBank->Read(Register::A), static_cast<uint8_t>(result));
    }

    // Test Flaga
    auto rawBinary = 0xC6;

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->Write(Register::A, 0xFE);
    alu.GetInstructionData().MemoryOperand1 = 0x02;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->Write(Register::A, 0x0F);
    alu.GetInstructionData().MemoryOperand1 = 0x01;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x10, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->Write(Register::A, 0xF0);
    alu.GetInstructionData().MemoryOperand1 = 0x80;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x70, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->Write(Register::A, 0x00);
    alu.GetInstructionData().MemoryOperand1 = 0x00;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->Write(Register::A, 0xFE);
    alu.GetInstructionData().MemoryOperand1 = 0x01;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0xFF, registerBank->Read(Register::A));
}

TEST(CoreTests_ADDAndADC, ExecuteAddRegisterIndirectMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<int8_t> distribution{-100, 100};

    for (auto i = 0; i < 1000; i++)
    {
        auto aValue = distribution(engine);
        auto memoryOperandValue = distribution(engine);
        auto result = static_cast<uint8_t>(0);
        result = aValue + memoryOperandValue;

        auto rawBinary = 0x86;
        alu.DecodeInstruction(rawBinary, nullopt);
        registerBank->Write(Register::A, aValue);
        alu.GetInstructionData().MemoryOperand1 = memoryOperandValue;
        alu.Execute();

        EXPECT_EQ(registerBank->Read(Register::A), static_cast<uint8_t>(result));
    }

    // Test Flaga
    auto rawBinary = 0x86;

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->Write(Register::A, 0xFE);
    alu.GetInstructionData().MemoryOperand1 = 0x02;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->Write(Register::A, 0x0F);
    alu.GetInstructionData().MemoryOperand1 = 0x01;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x10, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->Write(Register::A, 0xF0);
    alu.GetInstructionData().MemoryOperand1 = 0x80;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x70, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->Write(Register::A, 0x00);
    alu.GetInstructionData().MemoryOperand1 = 0x00;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->Write(Register::A, 0xFE);
    alu.GetInstructionData().MemoryOperand1 = 0x01;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0xFF, registerBank->Read(Register::A));
}


TEST(CoreTests_ADDAndADC, ExecuteAdcRegisterMode)
{
    auto sourceList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<int8_t> distribution{-100, 100};
    uniform_int_distribution<uint8_t> sourceRegisterDistribution {0, 6};

    for (auto i = 0; i < 1000; i++)
    {
        auto aValue = distribution(engine);
        auto operandValue = distribution(engine);
        auto result = static_cast<uint8_t>(0);

        // randomly choose to add carry
        auto carryValue = distribution(engine) % 2 == 0? 0 : 1;

        if (carryValue)
            registerBank->SetFlag(Flag::CY);
        else
            registerBank->ClearFlag(Flag::CY);

        auto source = *(begin(sourceList) + sourceRegisterDistribution(engine));
        auto rawBinary = 0x88 | RegisterBank::ToInstructionSource(source);

        alu.DecodeInstruction(rawBinary, nullopt);

        if(source != Register::A)
        {
            result = aValue + operandValue + carryValue;
            registerBank->Write(Register::A, aValue);
            registerBank->Write(source, operandValue);
        }
        else
        {
            result = aValue + aValue + carryValue;
            registerBank->Write(Register::A, aValue);
        }

        alu.Execute();

        EXPECT_EQ(registerBank->Read(Register::A), static_cast<uint8_t>(result));
    }

    // Test Flags
    auto rawBinary = 0x88 | RegisterBank::ToInstructionSource(Register::H);

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->SetFlag(Flag::CY);
    registerBank->Write(Register::A, 0x1E);
    registerBank->Write(Register::H, 0x01);
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x20, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->ClearFlag(Flag::CY);
    registerBank->Write(Register::A, 0x1E);
    registerBank->Write(Register::H, 0x01);
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x1F, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->SetFlag(Flag::CY);
    registerBank->Write(Register::A, 0xFE);
    registerBank->Write(Register::H, 0x01);
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->ClearFlag(Flag::CY);
    registerBank->Write(Register::A, 0xFE);
    registerBank->Write(Register::H, 0x01);
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0xFF, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->SetFlag(Flag::CY);
    registerBank->Write(Register::A, 0xF0);
    registerBank->Write(Register::H, 0x10);
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->ClearFlag(Flag::CY);
    registerBank->Write(Register::A, 0xF0);
    registerBank->Write(Register::H, 0x10);
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank->Read(Register::A));
}

TEST(CoreTests_ADDAndADC, ExecuteAdcImmediateMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<int8_t> distribution{-100, 100};

    for (auto i = 0; i < 1000; i++)
    {
        auto aValue = distribution(engine);
        auto operandValue = distribution(engine);
        auto result = static_cast<uint8_t>(0);

        // randomly choose to add carry
        auto carryValue = distribution(engine) % 2 == 0? 0 : 1;

        if (carryValue)
            registerBank->SetFlag(Flag::CY);
        else
            registerBank->ClearFlag(Flag::CY);

        auto rawBinary = 0xCE;

        alu.DecodeInstruction(rawBinary, nullopt);

        result = aValue + operandValue + carryValue;
        registerBank->Write(Register::A, aValue);
        alu.GetInstructionData().MemoryOperand1 = operandValue;

        alu.Execute();

        EXPECT_EQ(registerBank->Read(Register::A), static_cast<uint8_t>(result));
    }

    // Test Flags
    auto rawBinary = 0xCE;

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->SetFlag(Flag::CY);
    registerBank->Write(Register::A, 0x1E);
    alu.GetInstructionData().MemoryOperand1 = 0x01;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x20, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->ClearFlag(Flag::CY);
    registerBank->Write(Register::A, 0x1E);
    alu.GetInstructionData().MemoryOperand1 = 0x01;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x1F, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->SetFlag(Flag::CY);
    registerBank->Write(Register::A, 0xFE);
    alu.GetInstructionData().MemoryOperand1 = 0x01;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->ClearFlag(Flag::CY);
    registerBank->Write(Register::A, 0xFE);
    alu.GetInstructionData().MemoryOperand1 = 0x01;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0xFF, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->SetFlag(Flag::CY);
    registerBank->Write(Register::A, 0xF0);
    alu.GetInstructionData().MemoryOperand1 = 0x10;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->ClearFlag(Flag::CY);
    registerBank->Write(Register::A, 0xF0);
    alu.GetInstructionData().MemoryOperand1 = 0x10;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank->Read(Register::A));
}

TEST(CoreTests_ADDAndADC, ExecuteAdcRegisterIndirectMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<int8_t> distribution{-100, 100};

    for (auto i = 0; i < 1000; i++)
    {
        auto aValue = distribution(engine);
        auto operandValue = distribution(engine);
        auto result = static_cast<uint8_t>(0);

        // randomly choose to add carry
        auto carryValue = distribution(engine) % 2 == 0? 0 : 1;

        if (carryValue)
            registerBank->SetFlag(Flag::CY);
        else
            registerBank->ClearFlag(Flag::CY);

        auto rawBinary = 0x8E;

        alu.DecodeInstruction(rawBinary, nullopt);

        result = aValue + operandValue + carryValue;
        registerBank->Write(Register::A, aValue);
        alu.GetInstructionData().MemoryOperand1 = operandValue;

        alu.Execute();

        EXPECT_EQ(registerBank->Read(Register::A), static_cast<uint8_t>(result));
    }

    // Test Flags
    auto rawBinary = 0x8E;

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->SetFlag(Flag::CY);
    registerBank->Write(Register::A, 0x1E);
    alu.GetInstructionData().MemoryOperand1 = 0x01;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x20, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->ClearFlag(Flag::CY);
    registerBank->Write(Register::A, 0x1E);
    alu.GetInstructionData().MemoryOperand1 = 0x01;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x1F, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->SetFlag(Flag::CY);
    registerBank->Write(Register::A, 0xFE);
    alu.GetInstructionData().MemoryOperand1 = 0x01;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->ClearFlag(Flag::CY);
    registerBank->Write(Register::A, 0xFE);
    alu.GetInstructionData().MemoryOperand1 = 0x01;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0xFF, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->SetFlag(Flag::CY);
    registerBank->Write(Register::A, 0xF0);
    alu.GetInstructionData().MemoryOperand1 = 0x10;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registerBank->Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->ClearFlag(Flag::CY);
    registerBank->Write(Register::A, 0xF0);
    alu.GetInstructionData().MemoryOperand1 = 0x10;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank->Read(Register::A));
}

TEST(CoreTests_ADDAndADC, DecodeAddRegisterPairMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto binaryBase = 0x09;
    auto sourceList = { Register::BC, Register::DE, Register::HL, Register::SP };

    for (auto source : sourceList)
    {
        auto rawBinary = binaryBase | (RegisterBank::ToInstructionRegisterPair(source) << 4);
        alu.DecodeInstruction(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::add, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::RegisterPair, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(source, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(Register::HL, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(CoreTests_ADDAndADC, ExecuteAddRegisterPairMode)
{
    auto sourceList = { Register::BC, Register::DE, Register::HL, Register::SP };
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<int16_t> distribution{-32768, 32767};
    uniform_int_distribution<uint8_t> sourceRegisterDistribution {0, 3};

    for (auto i = 0; i < 1000; i++)
    {
        auto hlValue = distribution(engine);
        auto operandValue = distribution(engine);
        auto result = static_cast<uint16_t>(0);

        auto source = *(begin(sourceList) + sourceRegisterDistribution(engine));
        auto rawBinary = 0x09 | (RegisterBank::ToInstructionRegisterPair(source) << 0x04);

        alu.DecodeInstruction(rawBinary, nullopt);

        if(source != Register::HL)
        {
            result = hlValue + operandValue;
            registerBank->WritePair(Register::HL, hlValue);
            registerBank->WritePair(source, operandValue);
        }
        else
        {
            result = hlValue + hlValue;
            registerBank->WritePair(Register::HL, hlValue);
        }

        alu.Execute();

        EXPECT_EQ(registerBank->ReadPair(Register::HL), static_cast<uint16_t>(result));
    }

    // Test Flags
    auto rawBinary =  0x09 | (RegisterBank::ToInstructionRegisterPair(Register::DE) << 0x04);

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->WritePair(Register::HL, 0x1FFF);
    registerBank->WritePair(Register::DE, 0x0001);
    registerBank->SetFlag(Flag::Z); // ADD HL, DE must preserve flag Z
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x2000, registerBank->ReadPair(Register::HL));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->WritePair(Register::HL, 0xFFFF);
    registerBank->WritePair(Register::DE, 0x0001);
    registerBank->ClearFlag(Flag::Z); // ADD HL, DE must preserve flag Z
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x0000, registerBank->ReadPair(Register::HL));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->WritePair(Register::HL, 0xF000);
    registerBank->WritePair(Register::DE, 0x1000);
    registerBank->ClearFlag(Flag::Z); // ADD HL, DE must preserve flag Z
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x0000, registerBank->ReadPair(Register::HL));
    
    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->WritePair(Register::HL, 0xFF88);
    registerBank->WritePair(Register::DE, 0xFF89);
    registerBank->SetFlag(Flag::Z); // ADD HL, DE must preserve flag Z
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0xFF11, registerBank->ReadPair(Register::HL));
    
    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->WritePair(Register::HL, 0xFF88);
    registerBank->WritePair(Register::DE, 0x0077);
    registerBank->SetFlag(Flag::Z); // ADD HL, DE must preserve flag Z
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0xFFFF, registerBank->ReadPair(Register::HL));
}

TEST(CoreTests_ADDAndADC, DecodeAddToSPImmediaterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0xE8;

    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::add, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::SingleImmediatePair, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::SP, alu.GetInstructionData().DestinationRegister);
}

TEST(CoreTests_ADDAndADC, ExecuteAddToSPImmediaterMode)
{
    auto registerBank = make_shared<RegisterBank>();
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint16_t> distribution{0, 65535};
    uniform_int_distribution<int8_t> distributionImmediate{-128, 127};

    for (auto i = 0; i < 1000; i++)
    {
        auto SPValue = distribution(engine);
        auto operandValue = distributionImmediate(engine);
        auto result = static_cast<uint16_t>(0);

        auto rawBinary = 0xE8;

        alu.DecodeInstruction(rawBinary, nullopt);

        result = SPValue + operandValue;
        registerBank->WritePair(Register::SP, SPValue);
        alu.GetInstructionData().MemoryOperand1 = operandValue;

        alu.Execute();

        EXPECT_EQ(registerBank->ReadPair(Register::SP), static_cast<uint16_t>(result));
    }
    // Test Flags
    auto rawBinary =  0xE8;

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->WritePair(Register::SP, 0x0000);
    alu.GetInstructionData().MemoryOperand1 = 0x01;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x0001, registerBank->ReadPair(Register::SP));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->WritePair(Register::SP, 0x0000);
    alu.GetInstructionData().MemoryOperand1 = 0xFF;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0xFFFF, registerBank->ReadPair(Register::SP));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->WritePair(Register::SP, 0x0001);
    alu.GetInstructionData().MemoryOperand1 = 0xFE;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0xFFFF, registerBank->ReadPair(Register::SP));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->WritePair(Register::SP, 0xFFFF);
    alu.GetInstructionData().MemoryOperand1 = 0x01;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x0000, registerBank->ReadPair(Register::SP));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank->WritePair(Register::SP, 0x00FF);
    alu.GetInstructionData().MemoryOperand1 = 0x01;
    alu.Execute();

    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank->ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank->ReadFlag(Flag::CY));
    EXPECT_EQ(0x0100, registerBank->ReadPair(Register::SP));
}