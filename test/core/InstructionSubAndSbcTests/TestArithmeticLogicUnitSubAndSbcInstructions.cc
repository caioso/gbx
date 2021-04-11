#include <gtest/gtest.h>

#include "CoreTestMocksAndWrappers.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <vector>

#include "ArithmeticLogicUnit.h"
#include "MemoryController.h"
#include "Opcodes.h"
#include "RegisterBank.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace gbxcore::instructions;

TEST(TestSubAndSbc, DecodeSubRegisterMode)
{
    RegisterBank registerBank;
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    for (auto operand : operandList)
    {
        auto rawBinary = 0x90 | RegisterBank::ToInstructionSource(operand);
        alu.DecodeInstruction(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::sub, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(operand, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestSubAndSbc, DecodeSubImmediateMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0xD6;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::sub, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Immediate, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(TestSubAndSbc, DecodeSubRegisterIndirectMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0x96;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::sub, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSource, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(TestSubAndSbc, DecodeSubcRegisterMode)
{
    RegisterBank registerBank;
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
        
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    for (auto operand : operandList)
    {
        auto rawBinary = 0x98 | RegisterBank::ToInstructionSource(operand);
        alu.DecodeInstruction(rawBinary, nullopt);

        EXPECT_EQ(OpcodeType::sbc, alu.GetInstructionData().Opcode);
        EXPECT_EQ(AddressingMode::Register, alu.GetInstructionData().AddressingMode);
        EXPECT_EQ(operand, alu.GetInstructionData().SourceRegister);
        EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
    }
}

TEST(TestSubAndSbc, DecodeSbcImmediateMode)
{
    RegisterBank registerBank;
        
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0xDE;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::sbc, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::Immediate, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::NoRegister, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(TestSubAndSbc, DecodeSbcRegisterIndirectMode)
{
    RegisterBank registerBank;
        
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    auto rawBinary = 0x9E;
    alu.DecodeInstruction(rawBinary, nullopt);

    EXPECT_EQ(OpcodeType::sbc, alu.GetInstructionData().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSource, alu.GetInstructionData().AddressingMode);
    EXPECT_EQ(Register::HL, alu.GetInstructionData().SourceRegister);
    EXPECT_EQ(Register::A, alu.GetInstructionData().DestinationRegister);
}

TEST(TestSubAndSbc, ExecuteSubRegisterMode)
{
    RegisterBank registerBank;
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<int8_t> distribution{-100, 100};
    uniform_int_distribution<uint8_t> operandRegisterDistribution {0, 6};

    for (auto i = 0; i < 1000;  i++)
    {
        auto operand = *(begin(operandList) + operandRegisterDistribution(engine));
        auto aValue = distribution(engine);
        auto operandValue = distribution(engine);
        auto result = static_cast<uint8_t>(0x00);
        
        if (operand == Register::A)
        {
            result = aValue - aValue;
            registerBank.Write(Register::A, aValue);
        }
        else
        {
            result = aValue - operandValue;
            registerBank.Write(Register::A, aValue);            
            registerBank.Write(operand, operandValue);
        }

        auto rawBinary = 0x90 | RegisterBank::ToInstructionSource(operand);
        alu.DecodeInstruction(rawBinary, nullopt);
        alu.Execute();

        EXPECT_EQ(result, registerBank.Read(Register::A));
    }

    // Test Flags
    auto rawBinary = 0x90 | RegisterBank::ToInstructionSource(Register::D);

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x01);
    registerBank.Write(Register::D, 0x01);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x10);
    registerBank.Write(Register::D, 0x01);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x0F, registerBank.Read(Register::A));
    
    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x01);
    registerBank.Write(Register::D, 0x02);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0xFF, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x1);
    registerBank.Write(Register::D, 0xFF);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x02, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x10);
    registerBank.Write(Register::D, 0x20);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0xF0, registerBank.Read(Register::A));
}

TEST(TestSubAndSbc, ExecuteSubImmediateMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<int8_t> distribution{-100, 100};

    for (auto i = 0; i < 1000;  i++)
    {
        auto aValue = distribution(engine);
        auto operandValue = distribution(engine);
        auto result = static_cast<uint8_t>(0x00);
        
        result = aValue - operandValue;
        registerBank.Write(Register::A, aValue);            

        auto rawBinary = 0xD6;
        alu.DecodeInstruction(rawBinary, nullopt);
        alu.GetInstructionData().MemoryOperand1 = operandValue;
        alu.Execute();

        EXPECT_EQ(result, registerBank.Read(Register::A));
    }

    // Test Flags
    auto rawBinary = 0xD6;

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0xEE);
    alu.GetInstructionData().MemoryOperand1 = 0xEE;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x20);
    alu.GetInstructionData().MemoryOperand1 = 0x0F;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x11, registerBank.Read(Register::A));
    
    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0xFD);
    alu.GetInstructionData().MemoryOperand1 = 0xFE;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0xFF, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x05);
    alu.GetInstructionData().MemoryOperand1 = 0xFC;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x09, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x40);
     alu.GetInstructionData().MemoryOperand1 = 0x50;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0xF0, registerBank.Read(Register::A));
}

TEST(TestSubAndSbc, ExecuteSubRegisterIndirectMode)
{
    RegisterBank registerBank;
    
    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<int8_t> distribution{-100, 100};

    for (auto i = 0; i < 1000;  i++)
    {
        auto aValue = distribution(engine);
        auto operandValue = distribution(engine);
        auto result = static_cast<uint8_t>(0x00);
        
        result = aValue - operandValue;
        registerBank.Write(Register::A, aValue);            

        auto rawBinary = 0x96;
        alu.DecodeInstruction(rawBinary, nullopt);
        alu.GetInstructionData().MemoryOperand1 = operandValue;
        alu.Execute();

        EXPECT_EQ(result, registerBank.Read(Register::A));
    }

    // Test Flags
    auto rawBinary = 0x96;

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0xA5);
    alu.GetInstructionData().MemoryOperand1 = 0xA5;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x30);
    alu.GetInstructionData().MemoryOperand1 = 0x11;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x1F, registerBank.Read(Register::A));
    
    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0xF0);
    alu.GetInstructionData().MemoryOperand1 = 0xF1;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0xFF, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x10);
    alu.GetInstructionData().MemoryOperand1 = 0xEE;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x22, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0xA0);
     alu.GetInstructionData().MemoryOperand1 = 0xB0;
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0xF0, registerBank.Read(Register::A));
}


TEST(TestSubAndSbc, ExecuteSbcRegisterMode)
{
    RegisterBank registerBank;
    auto operandList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};

    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<int8_t> distribution{-100, 100};
    uniform_int_distribution<uint8_t> operandRegisterDistribution {0, 6};        


    for (auto i = 0; i < 1000;  i++)
    {
        auto aValue = distribution(engine);
        auto operandValue = distribution(engine);
        auto result = static_cast<uint8_t>(0x00);
        auto operand = *(begin(operandList) + operandRegisterDistribution(engine));
        
        // Randomly set CY flag
        if (distribution(engine)%2 == 0)
            registerBank.SetFlag(Flag::CY);         
        else
            registerBank.ClearFlag(Flag::CY); 

        if (operand == Register::A)
        {
            result = aValue - aValue - registerBank.ReadFlag(Flag::CY);
            registerBank.Write(Register::A, aValue);
        }
        else
        {
            result = aValue - operandValue - registerBank.ReadFlag(Flag::CY);;
            registerBank.Write(Register::A, aValue);            
            registerBank.Write(operand, operandValue);
        }

        auto rawBinary = 0x98 | RegisterBank::ToInstructionSource(operand);
        alu.DecodeInstruction(rawBinary, nullopt);
        alu.Execute();

        EXPECT_EQ(result, registerBank.Read(Register::A));
    }

    // Test Flags
    auto rawBinary = 0x98 | RegisterBank::ToInstructionSource(Register::L);

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x0A);
    registerBank.Write(Register::L, 0x09);
    registerBank.SetFlag(Flag::CY);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x10);
    registerBank.Write(Register::L, 0x08);
    registerBank.SetFlag(Flag::CY);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x07, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x00);
    registerBank.Write(Register::L, 0x00);
    registerBank.SetFlag(Flag::CY);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0xFF, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0xD3);
    registerBank.Write(Register::L, 0x05);
    registerBank.SetFlag(Flag::CY);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0xCD, registerBank.Read(Register::A));
}

TEST(TestSubAndSbc, ExecuteSbcImmediateMode)
{
    RegisterBank registerBank;

    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<int8_t> distribution{-100, 100};

    for (auto i = 0; i < 1000;  i++)
    {
        auto aValue = distribution(engine);
        auto operandValue = distribution(engine);
        auto result = static_cast<uint8_t>(0x00);
        
        // Randomly set CY flag
        if (distribution(engine)%2 == 0)
            registerBank.SetFlag(Flag::CY);         
        else
            registerBank.ClearFlag(Flag::CY); 

        result = aValue - operandValue - registerBank.ReadFlag(Flag::CY);
        registerBank.Write(Register::A, aValue);            

        auto rawBinary = 0xDE;
        alu.DecodeInstruction(rawBinary, nullopt);
        alu.GetInstructionData().MemoryOperand1 = operandValue;
        alu.Execute();

        EXPECT_EQ(result, registerBank.Read(Register::A));
    }

     // Test Flags
    auto rawBinary = 0x98 | RegisterBank::ToInstructionSource(Register::L);

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x0A);
    registerBank.Write(Register::L, 0x09);
    registerBank.SetFlag(Flag::CY);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x10);
    registerBank.Write(Register::L, 0x08);
    registerBank.SetFlag(Flag::CY);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x07, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x00);
    registerBank.Write(Register::L, 0x00);
    registerBank.SetFlag(Flag::CY);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0xFF, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0xD3);
    registerBank.Write(Register::L, 0x05);
    registerBank.SetFlag(Flag::CY);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0xCD, registerBank.Read(Register::A));
}


TEST(TestSubAndSbc, ExecuteSbcRegisterIndirectMode)
{
    RegisterBank registerBank;

    ArithmeticLogicDecorator alu;
    alu.Initialize(&registerBank);
    alu.InitializeRegisters();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<int8_t> distribution{-100, 100};

    for (auto i = 0; i < 1000;  i++)
    {
        auto aValue = distribution(engine);
        auto operandValue = distribution(engine);
        auto result = static_cast<uint8_t>(0x00);
        
        // Randomly set CY flag
        if (distribution(engine)%2 == 0)
            registerBank.SetFlag(Flag::CY);         
        else
            registerBank.ClearFlag(Flag::CY); 

        result = aValue - operandValue - registerBank.ReadFlag(Flag::CY);
        registerBank.Write(Register::A, aValue);            

        auto rawBinary = 0x9E;
        alu.DecodeInstruction(rawBinary, nullopt);
        alu.GetInstructionData().MemoryOperand1 = operandValue;
        alu.Execute();

        EXPECT_EQ(result, registerBank.Read(Register::A));
    }

     // Test Flags
    auto rawBinary = 0x98 | RegisterBank::ToInstructionSource(Register::L);

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x0A);
    registerBank.Write(Register::L, 0x09);
    registerBank.SetFlag(Flag::CY);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x10);
    registerBank.Write(Register::L, 0x08);
    registerBank.SetFlag(Flag::CY);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0x07, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0x00);
    registerBank.Write(Register::L, 0x00);
    registerBank.SetFlag(Flag::CY);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0xFF, registerBank.Read(Register::A));

    alu.DecodeInstruction(rawBinary, nullopt);
    registerBank.Write(Register::A, 0xD3);
    registerBank.Write(Register::L, 0x05);
    registerBank.SetFlag(Flag::CY);
    alu.Execute();

    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registerBank.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registerBank.ReadFlag(Flag::CY));
    EXPECT_EQ(0xCD, registerBank.Read(Register::A));
}