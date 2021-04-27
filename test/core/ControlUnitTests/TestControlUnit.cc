#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cmath>
#include <memory>
#include <thread>
#include <variant>
#include <random>

#include "CoreTestMocksAndWrappers.h"

#include "ArithmeticLogicUnit.h"
#include "ControlUnit.h"
#include "RegisterBank.h"
#include "MemoryControllerInterface.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::interfaces;
using ::testing::Return;

uint8_t ImmediateOpcode(Register source)
{
    return RegisterBank::ToInstructionRegisterPair(source) << 4 | 0x01;
}

TEST(CoreTests_ControlUnit, FetchPCMessage)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x47)));
    
    controlUnit->RunCycle();

    auto instructionRegister = registerBank.Read(Register::IR);
    auto programCounter = registerBank.ReadPair(Register::PC);

    EXPECT_EQ(0x47, instructionRegister);
    EXPECT_EQ(0x0001, programCounter);
}

TEST(CoreTests_ControlUnit, TestAcquireSingleImmediateOperand)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);
    
    // First trigger to controlUnit. 
    
    // The returned operation bytes will be 0x26 0xAA which translates to ArithmeticLogicUnitInterfaceogicUnit H, 0xAA (load H immediate with 0xAA)
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x26)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xAA)));
    
    controlUnit->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0xAA, registerBank.Read(Register::H));
}

TEST(CoreTests_ControlUnit, TestAcquireSingleRegisterIndirectOperand)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Initialize HL
    registerBank.WritePair(Register::HL, 0xAABB);
    
    // First trigger to controlUnit. 
    
    // The returned operation bytes will be OPCODE: 0x7E  (HL): 0xCC where HL holds address 0xAABB
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    EXPECT_CALL(memoryController, Read(0xAABB, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xCC)));
   
    controlUnit->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0xCC, registerBank.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestAcquireSingleRegisterIndirectBCOperand)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Initialize HL
    registerBank.WritePair(Register::BC, 0x1234);

    // First trigger to controlUnit. 
    
    // The returned operation bytes will be OPCODE: 0x0A  (BC): 0xDD where BC holds address 0x1234
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0A)));
    EXPECT_CALL(memoryController, Read(0x1234, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xDD)));
    
    controlUnit->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0xDD, registerBank.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestAcquireSingleRegisterIndirectDEOperand)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Initialize HL
    registerBank.WritePair(Register::DE, 0x6789);

     // First trigger to controlUnit. 
    
    // The returned operation bytes will be OPCODE: 0x0A  (BC): 0xDD where BC holds address 0x1234
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x1A)));
    EXPECT_CALL(memoryController, Read(0x6789, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x22)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x22, registerBank.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestAcquireSingleRegisterIndirectDestinationHL)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Initialize HL
    registerBank.WritePair(Register::HL, 0xAABB);
    registerBank.Write(Register::A, 0xCC);

    // First trigger to controlUnit. 
    
    // Write A (0xCC) to the location pointed by HL (0xAABB);
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x77)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xCC)), static_cast<uint16_t>(0xAABB)));
    
    controlUnit->RunCycle();
}

TEST(CoreTests_ControlUnit, TestAcquireSingleRegisterIndirectDestinationDE)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Initialize HL
    registerBank.WritePair(Register::DE, 0x1234);
    registerBank.Write(Register::A, 0x99);

    // First trigger to controlUnit. 
    
    // The returned operation bytes will be OPCODE: 0x77  write A to (HL) (A value will be 0xEE, HL will point to 0xAABB)
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x12)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x99)), static_cast<uint16_t>(0x1234)));

    controlUnit->RunCycle();
}

TEST(CoreTests_ControlUnit, TestAcquireSingleRegisterIndirectDestinationBC)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Initialize HL
    registerBank.WritePair(Register::BC, 0x9967);
    registerBank.Write(Register::A, 0x12);

    // First trigger to controlUnit. 
    
    // The returned operation bytes will be OPCODE: 0x77  write A to (HL) (A value will be 0xEE, HL will point to 0xAABB)
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x02)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x12)), static_cast<uint16_t>(0x9967)));
    
    controlUnit->RunCycle();
}

TEST(CoreTests_ControlUnit, TestIndexedSourceOpcodeIX)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registerBank.WritePair(Register::IX, 0x0100);
    
    // First trigger to controlUnit. 
    
    // IX will have value 0x0100 so the final address will be 0x01AA, which will hold value 0x11
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xDD)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0A)));
    EXPECT_CALL(memoryController, Read(0x010A, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x11)));
    
    controlUnit->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0x11, registerBank.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestIndexedSourceOpcodeIY)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registerBank.WritePair(Register::IY, 0x0A00);

    // First trigger to controlUnit. 
    
    // IY will have value 0x0A00 so the final address will be 0x0A09, which will hold value 0xCA
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFD)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x09)));
    EXPECT_CALL(memoryController, Read(0x0A09, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xCA)));

    controlUnit->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0xCA, registerBank.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestIndexedSourceOpcodeIXNegativeDisplacement)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registerBank.WritePair(Register::IY, 0x0200);

    // First trigger to controlUnit. 
    
    // IY will have value 0x0200 so the final address will be 0x01FF, which will hold value 0x10
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFD)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    EXPECT_CALL(memoryController, Read(0x01FF, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x10)));

    controlUnit->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0x10, registerBank.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestIndexedDestinantionOpcodeIX)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registerBank.Write(Register::A, 0x88);
    registerBank.WritePair(Register::IX, 0x0100);
    
    // First trigger to controlUnit. 
    
    // IX will have value 0x0100 so the final address will be 0x0110, which will hold value 0x88
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xDD)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x77)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x10)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x88)), static_cast<uint16_t>(0x110)));
    
    controlUnit->RunCycle();
}

TEST(CoreTests_ControlUnit, TestExtendedSource)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // First trigger to controlUnit. 
    
    // Register A will be loaded with the content stored in address 0xDAAD (0x9E)
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFA)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xAD)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xDA)));
    EXPECT_CALL(memoryController, Read(0xDAAD, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x9E)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x9E, registerBank.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestExtendedDestination)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registerBank.Write(Register::A, 0xF8);

    // First trigger to controlUnit. 
    
    // Content of Register A will be soted in address 0xBABE
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xEA)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xBE)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xBA)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xF8)), static_cast<uint16_t>(0xBABE)));
    
    controlUnit->RunCycle();
}

TEST(CoreTests_ControlUnit, TestImmediateRegisterIndirect)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registerBank.WritePair(Register::HL, 0xF5D2);

    // First trigger to controlUnit. 
    
    // Content of the Operand 1 will be stored in the address held by HL.
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x36)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xCC)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xCC)), static_cast<uint16_t>(0xF5D2)));
    
    controlUnit->RunCycle();
}

TEST(CoreTests_ControlUnit, TestRegisterIndirectSourceIncrement)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registerBank.WritePair(Register::HL, 0x99AA);

    // First trigger to controlUnit. 
    
    // Content of the Operand 1 will be stored in the address held by HL.
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x2A)));
    EXPECT_CALL(memoryController, Read(0x99AA, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xAB)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0xAB, registerBank.Read(Register::A));
    EXPECT_EQ(0x99AB, registerBank.ReadPair(Register::HL));
}

TEST(CoreTests_ControlUnit, TestRegisterIndirectSourceDecrement)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registerBank.WritePair(Register::HL, 0x88BB);

    // First trigger to controlUnit. 
    
    // Content of the Operand 1 will be stored in the address held by HL.
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x3A)));
    EXPECT_CALL(memoryController, Read(0x88BB, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x45)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x45, registerBank.Read(Register::A));
    EXPECT_EQ(0x88BA, registerBank.ReadPair(Register::HL));
}

TEST(CoreTests_ControlUnit, TestRegisterIndirectDestinationIncrement)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registerBank.Write(Register::A, 0x66);
    registerBank.WritePair(Register::HL, 0x1234);

    // First trigger to controlUnit. 
    
    // Content of the Operand 1 will be stored in the address held by HL.
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x22)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x66)), static_cast<uint16_t>(0x1234)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x1235, registerBank.ReadPair(Register::HL));
}

TEST(CoreTests_ControlUnit, TestRegisterIndirectDestinationDecrement)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registerBank.Write(Register::A, 0x41);
    registerBank.WritePair(Register::HL, 0x764A);

    // First trigger to controlUnit. 
    
    // Content of the Operand 1 will be stored in the address held by HL.
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x32)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x41)), static_cast<uint16_t>(0x764A)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x7649, registerBank.ReadPair(Register::HL));
}

TEST(CoreTests_ControlUnit, TestImplicitRegisterSource)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registerBank.Write(Register::C, 0x55);

    // First trigger to controlUnit. 
    
    // Register A will hold content pointed by [C + 0xFF00]
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xF2)));
    EXPECT_CALL(memoryController, Read(0xFF55, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xD0)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0xD0, registerBank.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestImplicitRegisterDestination)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registerBank.Write(Register::A, 0xA6);
    registerBank.Write(Register::C, 0xF4);

    // First trigger to controlUnit. 
    
    // [C + 0xFF00] will hold the content od register A
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xE2)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xA6)), static_cast<uint16_t>(0xFFF4)));
    
    controlUnit->RunCycle();
}

TEST(CoreTests_ControlUnit, TestImplicitImmediateSource)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // First trigger to controlUnit. 
    
    // Register A will hold content pointed by [C + 0xFF00]
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xF0)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x60)));
    EXPECT_CALL(memoryController, Read(0xFF60, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x09)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x09, registerBank.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestImplicitImmediateDestination)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registerBank.Write(Register::A, 0xD2);

    // First trigger to controlUnit. 
    
    // Register A will hold content pointed by [C + 0xFF00]
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xE0)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x06)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xD2)), static_cast<uint16_t>(0xFF06)));
    
    controlUnit->RunCycle();
}

TEST(CoreTests_ControlUnit, TestImmediatePair)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // First trigger to controlUnit. 
    
    // Register A will hold content pointed by [C + 0xFF00]
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(ImmediateOpcode(Register::BC))));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x4A)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x98)));
    EXPECT_CALL(memoryController, Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(ImmediateOpcode(Register::DE))));
    EXPECT_CALL(memoryController, Read(0x0004, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xBB)));
    EXPECT_CALL(memoryController, Read(0x0005, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x1A)));
    EXPECT_CALL(memoryController, Read(0x0006, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(ImmediateOpcode(Register::HL))));
    EXPECT_CALL(memoryController, Read(0x0007, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x99)));
    EXPECT_CALL(memoryController, Read(0x0008, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x50)));
    EXPECT_CALL(memoryController, Read(0x0009, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(ImmediateOpcode(Register::SP))));
    EXPECT_CALL(memoryController, Read(0x000A, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x24)));
    EXPECT_CALL(memoryController, Read(0x000B, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xCE)));

    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x984A, registerBank.ReadPair(Register::BC));
    EXPECT_EQ(0x1ABB, registerBank.ReadPair(Register::DE));
    EXPECT_EQ(0x5099, registerBank.ReadPair(Register::HL));
    EXPECT_EQ(0xCE24, registerBank.ReadPair(Register::SP));
}

TEST(CoreTests_ControlUnit, TestTransferToSP)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registerBank.WritePair(Register::HL, 0x65A1);

    // First trigger to controlUnit. 
    
    // Register A will hold content pointed by [C + 0xFF00]
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xF9)));
    
    controlUnit->RunCycle();
    EXPECT_EQ(0x65A1, registerBank.ReadPair(Register::SP));
}

TEST(CoreTests_ControlUnit, TestAddRegister)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // ADD A, C
    registerBank.Write(Register::C, 0x45);
    registerBank.Write(Register::A, 0x82);

    auto rawBinary = 0x80 | RegisterBank::ToInstructionSource(Register::C);

    // First trigger to controlUnit. 
    
    // Register A will hold content pointed by [C + 0xFF00]
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(rawBinary)));
    
    controlUnit->RunCycle();
    EXPECT_EQ(0xC7, registerBank.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestAddImmediate)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // ADD A, #73
    registers.Write(Register::A, 0x24);

    auto rawBinary = 0xC6;

    // First trigger to controlUnit. 
    
    // Register A will hold content pointed by [C + 0xFF00]
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(rawBinary)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x73)));
    
    controlUnit->RunCycle();
    EXPECT_EQ(0x97, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestAddRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // ADD A, (HL)
    // where HL = 0x75E1 => 0x40
    registers.Write(Register::A, 0x60);
    registers.WritePair(Register::HL, 0x75E1);

    auto rawBinary = 0x86;

    // First trigger to controlUnit. 
    
    // Register A will hold content pointed by [C + 0xFF00]
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(rawBinary)));
    EXPECT_CALL(memoryController, Read(0x75E1, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x40)));
    
    controlUnit->RunCycle();
    EXPECT_EQ(0xA0, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestAdcRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // ADC A, B ; CY = 1 : A = 0x00
    // ADC A, D ; CY = 0 : A = 0x10
    // ADC A, E ; CY = 0 : A = 0x1A 
    registers.Write(Register::A, 0xFF);
    registers.Write(Register::B, 0x01);
    registers.Write(Register::D, 0x0F);
    registers.Write(Register::E, 0x0A);

    auto instruction1 = 0x88 | RegisterBank::ToInstructionSource(Register::B);
    auto instruction2 = 0x88 | RegisterBank::ToInstructionSource(Register::D);
    auto instruction3 = 0x88 | RegisterBank::ToInstructionSource(Register::E);

    // First trigger to controlUnit. 
    
    // Run three instructions to evaluate the effects of running adc with and without carry.
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction2)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction3)));
    
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x1A, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestAdcImmediate)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();


    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // ADC A, #0xFF ; CY = 1 : A = 0x00
    // ADC A, #0x0E ; CY = 0 : A = 0x0F
    // ADC A, #0x01 ; CY = 0 : A = 0x10 
    registers.Write(Register::A, 0x01);

    auto instruction1 = 0xCE;

    // First trigger to controlUnit. 
    
    // Run three instructions to evaluate the effects of running adc with and without carry.
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0E)));
    EXPECT_CALL(memoryController, Read(0x0004, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0005, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x01)));
    
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x10, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestAdcRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // ADC A, (HL) ; HL = #45DA -> 0x4C A -> 0x04 CY = 0
    // ADC A, (HL) ; HL = #3BFF -> 0xB0 A -> 0x00 CY = 1
    // ADC A, (HL) ; HL = #814A -> 0x0F A -> 0x10 CY = 0
    registers.Write(Register::A, 0x04);
    registers.WritePair(Register::HL, 0x45DC);

    auto instruction1 = 0x8E;

    // First trigger to controlUnit. 
    
    // Run three instructions to evaluate the effects of running adc with and without carry.
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x45DC, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x4C)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x3BFF, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xB0)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x814A, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0F)));
    
    controlUnit->RunCycle();
    registers.WritePair(Register::HL, 0x3BFF);
    controlUnit->RunCycle();
    registers.WritePair(Register::HL, 0x814A);
    controlUnit->RunCycle();

    EXPECT_EQ(0x10, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestSubRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // SUB A, L
    registers.Write(Register::L, 0x56);
    registers.Write(Register::A, 0x16);

    auto rawBinary = 0x90 | RegisterBank::ToInstructionSource(Register::L);

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(rawBinary)));
    
    controlUnit->RunCycle();
    EXPECT_EQ(0xC0, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestSubImmediate)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // SUB A, #FF
    registers.Write(Register::A, 0x01);

    auto rawBinary = 0xD6;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(rawBinary)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    
    controlUnit->RunCycle();
    EXPECT_EQ(0x02, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestSubRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // SUB A, (HL)
    // HL = 0x65EE => 0xAA
    registers.Write(Register::A, 0xAA);
    registers.WritePair(Register::HL, 0x65EE);

    auto rawBinary = 0x96;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(rawBinary)));
    EXPECT_CALL(memoryController, Read(0x65EE, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xAA)));
    
    controlUnit->RunCycle();
    EXPECT_EQ(0x00, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestSbcRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // SBC A, C ; CY is 1
    registers.Write(Register::A, 0x00);
    registers.Write(Register::C, 0x01);
    registers.Write(Register::D, 0xFE);

    auto instruction1 = 0x98 | RegisterBank::ToInstructionSource(Register::C);
    auto instruction2 = 0x98 | RegisterBank::ToInstructionSource(Register::D);

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction2)));
    
    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestSbcImmediate)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registers.Write(Register::A, 0x10);

    auto instruction1 = 0xDE;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x08)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x08)));

    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestSbcRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registers.Write(Register::A, 0x09);
    registers.WritePair(Register::HL, 0x67DD);

    auto instruction1 = 0x9E;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x67DD, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x09)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x87D1, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x01)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x7EEE, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x01)));

    controlUnit->RunCycle();
    registers.WritePair(Register::HL, 0x87D1);
    controlUnit->RunCycle();
    registers.WritePair(Register::HL, 0x7EEE);
    controlUnit->RunCycle();

    EXPECT_EQ(0xFD, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestAndRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // AND A, C
    registers.Write(Register::A, 0xF1);
    registers.Write(Register::C, 0x8F);

    auto instruction1 = 0xA0 | RegisterBank::ToInstructionSource(Register::C);

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x81, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestAndImmediate)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // AND A, #0A
    registers.Write(Register::A, 0xAA);

    auto instruction1 = 0xE6;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0A)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x0A, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestAndRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // AND A, (HL)
    registers.Write(Register::A, 0xFF);
    registers.WritePair(Register::HL, 0x8A5D);

    auto instruction1 = 0xA6;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x8A5D, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestOrRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // OR A, C
    registers.Write(Register::A, 0xF0);
    registers.Write(Register::C, 0x0F);

    auto instruction1 = 0xB0 | RegisterBank::ToInstructionSource(Register::C);

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0xFF, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestOrImmediate)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // OR A, #0x55
    registers.Write(Register::A, 0xAA);

    auto instruction1 = 0xF6;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x55)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0xFF, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestOrRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // OR A, (HL))
    registers.Write(Register::A, 0x80);
    registers.WritePair(Register::HL, 0x1111);

    auto instruction1 = 0xB6;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x1111, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0F)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x8F, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestXorRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // XOR A, D
    registers.Write(Register::A, 0x45);
    registers.Write(Register::D, 0x45);

    auto instruction1 = 0xA8 | RegisterBank::ToInstructionSource(Register::D);

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestXorImmediate)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // XOR A, #79
    registers.Write(Register::A, 0xFF);

    auto instruction1 = 0xEE;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x79)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x86, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestXorRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // XOR A, (HL)
    registers.Write(Register::A, 0x81);
    registers.WritePair(Register::HL, 0x0914);

    auto instruction1 = 0xAE;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0914, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0xFF, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestCpRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // CP A, D
    registers.Write(Register::A, 0xA1);
    registers.Write(Register::D, 0xA0);

    auto instruction1 = 0xB8 | RegisterBank::ToInstructionSource(Register::D);

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0xA1, registers.Read(Register::A));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z)); // A is different then D
}

TEST(CoreTests_ControlUnit, TestCpImmediate)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // CP A, #01
    registers.Write(Register::A, 0x10);

    auto instruction1 = 0xFE;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x01)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x11)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x10, registers.Read(Register::A));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::H)); // A i biggern than 0x01
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::CY));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x10, registers.Read(Register::A));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::H)); // A i smaller than 0x011
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
}

TEST(CoreTests_ControlUnit, TestCpRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // CP A, (HL))
    registers.Write(Register::A, 0x05);
    registers.WritePair(Register::HL, 0x50D3);

    auto instruction1 = 0xBE;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x50D3, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x05, registers.Read(Register::A));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z)); 
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::H)); // A is smaller then D
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
}

TEST(CoreTests_ControlUnit, TestIncRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // INC H
    registers.Write(Register::H, 0x08);

    auto instruction1 = 0x04 | (RegisterBank::ToInstructionSource(Register::H)) << 0x03;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x09, registers.Read(Register::H));
}

TEST(CoreTests_ControlUnit, TestIncRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // INC (HL)
    registers.WritePair(Register::HL, 0x6589);

    auto instruction1 = 0x34;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x6589, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x56)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x57)), static_cast<uint16_t>(0x6589)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(registers.ReadPair(Register::HL), 0x6589);
    EXPECT_EQ(registers.ReadPair(Register::PC), 0x0001);
    
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x6589, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x57)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x58)), static_cast<uint16_t>(0x6589)));

    controlUnit->RunCycle();

    EXPECT_EQ(registers.ReadPair(Register::HL), 0x6589);
    EXPECT_EQ(registers.ReadPair(Register::PC), 0x0002);
}

TEST(CoreTests_ControlUnit, TestDecRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // INC H
    registers.Write(Register::H, 0x4E);

    auto instruction1 = 0x05 | (RegisterBank::ToInstructionSource(Register::H)) << 0x03;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x4D, registers.Read(Register::H));
}

TEST(CoreTests_ControlUnit, TestDecRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // INC (HL)
    registers.WritePair(Register::HL, 0x66A1);

    auto instruction1 = 0x35;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x66A1, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x70)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x6F)), static_cast<uint16_t>(0x66A1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(registers.ReadPair(Register::HL), 0x66A1);
    EXPECT_EQ(registers.ReadPair(Register::PC), 0x0001);
    
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x66A1, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xFF)), static_cast<uint16_t>(0x66A1)));

    controlUnit->RunCycle();

    EXPECT_EQ(registers.ReadPair(Register::HL), 0x66A1);
    EXPECT_EQ(registers.ReadPair(Register::PC), 0x0002);

}

TEST(CoreTests_ControlUnit, TestPush)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // set stackPoiner
    auto spValue = static_cast<uint16_t>(0x8000);
    registers.WritePair(Register::SP, spValue);

    auto instruction0 = (0x03 << 0x06) | (RegisterBank::ToInstructionRegisterPushPair(Register::BC)) << 0x04 | 0x05;
    auto instruction1 = (0x03 << 0x06) | (RegisterBank::ToInstructionRegisterPushPair(Register::DE)) << 0x04 | 0x05;
    auto instruction2 = (0x03 << 0x06) | (RegisterBank::ToInstructionRegisterPushPair(Register::HL)) << 0x04 | 0x05;
    auto instruction3 = (0x03 << 0x06) | (RegisterBank::ToInstructionRegisterPushPair(Register::AF)) << 0x04 | 0x05;
    registers.WritePair(Register::BC, 0x65DD);
    registers.WritePair(Register::DE, 0xEE11);
    registers.WritePair(Register::HL, 0x0814);
    registers.WritePair(Register::AF, 0xAAB1);

    auto pcValue = 0x0000;

    for (auto i = 0; i < 1000; i++)
    {
        // First trigger to controlUnit. 
        
        EXPECT_CALL(memoryController, Read(static_cast<uint16_t>(pcValue++), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction0)));
        EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x65)), static_cast<uint16_t>(spValue - 1)));
        EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xDD)), static_cast<uint16_t>(spValue - 2)));

        controlUnit->RunCycle();
        spValue -= 2;
        EXPECT_EQ(spValue, registers.ReadPair(Register::SP));

        EXPECT_CALL(memoryController, Read(static_cast<uint16_t>(pcValue++), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
        EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xEE)), static_cast<uint16_t>(spValue - 1)));
        EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x11)), static_cast<uint16_t>(spValue - 2)));
        
        controlUnit->RunCycle();
        spValue -= 2;
        EXPECT_EQ(spValue, registers.ReadPair(Register::SP));

        EXPECT_CALL(memoryController, Read(static_cast<uint16_t>(pcValue++), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction2)));
        EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x08)), static_cast<uint16_t>(spValue - 1)));
        EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x14)), static_cast<uint16_t>(spValue - 2)));
        
        controlUnit->RunCycle();
        spValue -= 2;
        EXPECT_EQ(spValue, registers.ReadPair(Register::SP));

        EXPECT_CALL(memoryController, Read(static_cast<uint16_t>(pcValue++), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction3)));
        EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xAA)), static_cast<uint16_t>(spValue - 1)));
        EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xB1)), static_cast<uint16_t>(spValue - 2)));
        
        controlUnit->RunCycle();
        spValue -= 2;
        EXPECT_EQ(spValue, registers.ReadPair(Register::SP));
    }
}


TEST(CoreTests_ControlUnit, TestPop)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // set stackPoiner
    auto spValue = static_cast<uint16_t>(0x8000);
    registers.WritePair(Register::SP, spValue);

    auto instruction0 = (0x03 << 0x06) | (RegisterBank::ToInstructionRegisterPushPair(Register::BC)) << 0x04 | 0x01;
    auto instruction1 = (0x03 << 0x06) | (RegisterBank::ToInstructionRegisterPushPair(Register::DE)) << 0x04 | 0x01;
    auto instruction2 = (0x03 << 0x06) | (RegisterBank::ToInstructionRegisterPushPair(Register::HL)) << 0x04 | 0x01;
    auto instruction3 = (0x03 << 0x06) | (RegisterBank::ToInstructionRegisterPushPair(Register::AF)) << 0x04 | 0x01;

    auto pcValue = 0x0000;

    for (auto i = 0; i < 1000; i++)
    {
        random_device randomDevice;
        mt19937 engine{randomDevice()};
        uniform_int_distribution<uint16_t> distribution{0, 65535};

        auto operandValue0 = distribution(engine);
        auto operandValue1 = distribution(engine);
        auto operandValue2 = distribution(engine);
        auto operandValue3 = distribution(engine);

        // First trigger to controlUnit. 
        
        EXPECT_CALL(memoryController, Read(static_cast<uint16_t>(pcValue++), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction0)));
        EXPECT_CALL(memoryController, Read(static_cast<uint16_t>(spValue), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>((operandValue0 & 0xFF))));
        EXPECT_CALL(memoryController, Read(static_cast<uint16_t>(spValue + 1), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>((operandValue0 >> 0x08) & 0xFF)));

        controlUnit->RunCycle();
        spValue += 2;
        EXPECT_EQ(spValue, registers.ReadPair(Register::SP));
        EXPECT_EQ(operandValue0, registers.ReadPair(Register::BC));

        EXPECT_CALL(memoryController, Read(static_cast<uint16_t>(pcValue++), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
        EXPECT_CALL(memoryController, Read(static_cast<uint16_t>(spValue), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>((operandValue1 & 0xFF))));
        EXPECT_CALL(memoryController, Read(static_cast<uint16_t>(spValue + 1), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>((operandValue1 >> 0x08) & 0xFF)));

        controlUnit->RunCycle();
        spValue += 2;
        EXPECT_EQ(spValue, registers.ReadPair(Register::SP));
        EXPECT_EQ(operandValue1, registers.ReadPair(Register::DE));

        EXPECT_CALL(memoryController, Read(static_cast<uint16_t>(pcValue++), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction2)));
        EXPECT_CALL(memoryController, Read(static_cast<uint16_t>(spValue), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>((operandValue2 & 0xFF))));
        EXPECT_CALL(memoryController, Read(static_cast<uint16_t>(spValue+ 1), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>((operandValue2 >> 0x08) & 0xFF)));

        controlUnit->RunCycle();
        spValue += 2;
        EXPECT_EQ(spValue, registers.ReadPair(Register::SP));
        EXPECT_EQ(operandValue2, registers.ReadPair(Register::HL));


        EXPECT_CALL(memoryController, Read(static_cast<uint16_t>(pcValue++), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction3)));
        EXPECT_CALL(memoryController, Read(static_cast<uint16_t>(spValue + 1), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>((operandValue3 >> 0x08) & 0xFF)));
        EXPECT_CALL(memoryController, Read(static_cast<uint16_t>(spValue), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>((operandValue3 & 0xFF))));

        controlUnit->RunCycle();
        spValue += 2;
        EXPECT_EQ(spValue, registers.ReadPair(Register::SP));
        EXPECT_EQ(operandValue3, registers.ReadPair(Register::AF));
    }
}

TEST(CoreTests_ControlUnit, TestLdhlRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // INC H
    registers.WritePair(Register::SP, 0x31DA);

    auto instruction1 = 0xF8;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x06)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x31E0, registers.ReadPair(Register::HL));
}

TEST(CoreTests_ControlUnit, TestSPTransferToMemory)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // INC H
    registers.WritePair(Register::SP, 0x87A1);

    auto instruction1 = 0x08;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xA1)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x67)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xA1)), static_cast<uint16_t>(0x67A1)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x87)), static_cast<uint16_t>(0x67A2)));
    
    controlUnit->RunCycle();
}

TEST(CoreTests_ControlUnit, TestSPImmediate)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registers.WritePair(Register::SP, 0xFFFE);

    auto instruction1 = 0xE8;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x01)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    EXPECT_CALL(memoryController, Read(0x0004, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0005, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x02)));
    

    controlUnit->RunCycle();

    EXPECT_EQ(0xFFFF, registers.ReadPair(Register::SP));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0xFFFE, registers.ReadPair(Register::SP));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x0000, registers.ReadPair(Register::SP));
}

TEST(CoreTests_ControlUnit, TestIncRegisterPair)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // INC BC
    registers.WritePair(Register::BC, 0x80FF);

    auto instruction1 = 0x03 | (RegisterBank::ToInstructionRegisterPair(Register::BC)) << 0x04;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0004, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0005, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x8105, registers.ReadPair(Register::BC));
}


TEST(CoreTests_ControlUnit, TestDecRegisterPair)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // INC BC
    registers.WritePair(Register::DE, 0x0010);

    auto instruction1 = 0x0B | (RegisterBank::ToInstructionRegisterPair(Register::DE)) << 0x04;

    // First trigger to controlUnit. 
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0004, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0005, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0006, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0007, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x0008, registers.ReadPair(Register::DE));
}

TEST(CoreTests_ControlUnit, TestRlca)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // rlca
    registers.Write(Register::A, 0x7F);

    auto instruction1 = 0x07;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0xFE, registers.Read(Register::A));

    EXPECT_EQ(0x00, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestRla)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // rla
    registers.SetFlag(Flag::CY);
    registers.Write(Register::A, 0x95);

    auto instruction1 = 0x17;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x2B, registers.Read(Register::A));

    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestRrca)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // rrca
    registers.Write(Register::A, 0x3B);

    auto instruction1 = 0x0F;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x9D, registers.Read(Register::A));

    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestRra)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // rra
    registers.Write(Register::A, 0x81);

    auto instruction1 = 0x1F;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x40, registers.Read(Register::A));

    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestRlcRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // RLC E
    registers.Write(Register::E, 0x85);

    auto instruction1 = 0xCB;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x03)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x0B, registers.Read(Register::E));

    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestRlcRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // RLC (HL) ; HL = 0xA099 [0xA099] = 0xFE
    registers.WritePair(Register::HL, 0xA099);

    auto instruction1 = 0xCB;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x06)));
    EXPECT_CALL(memoryController, Read(0xA099, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7F)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xFE)), static_cast<uint16_t>(0xA099)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestRlRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // RL L
    registers.Write(Register::L, 0x80);
    registers.SetFlag(Flag::Z);

    auto instruction1 = 0xCB;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x15)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers.Read(Register::L));

    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestRlRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // RL (HL) ; HL = 0xEE12 [0xEE12] = 0x11
    registers.WritePair(Register::HL, 0xEE12);

    auto instruction1 = 0xCB;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x16)));
    EXPECT_CALL(memoryController, Read(0xEE12, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x11)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x22)), static_cast<uint16_t>(0xEE12)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestRrcRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // RRC L
    registers.Write(Register::C, 0x01);
    registers.ClearFlag(Flag::Z);

    auto preOpcode = 0xCB;
    auto opcode = 0x09;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x80, registers.Read(Register::C));

    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestRrcRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // RRC (HL) -> HL = 8891, [8891] = 0xFE
    registers.WritePair(Register::HL, 0x8891);
    auto preOpcode = 0xCB;
    auto opcode = 0x0E;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x8891, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFE)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x7F)), static_cast<uint16_t>(0x8891)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestRrRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // RR L
    registers.Write(Register::A, 0x01);
    registers.ClearFlag(Flag::Z);

    auto preOpcode = 0xCB;
    auto opcode = 0x1F;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers.Read(Register::A));

    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestRrRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // RR (HL) -> HL 0xAAAA, [0xAAAA] = 0x8A
    registers.WritePair(Register::HL, 0xAAAA);

    auto preOpcode = 0xCB;
    auto opcode = 0x1E;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0xAAAA, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x8A)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x45)), static_cast<uint16_t>(0xAAAA)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestSlaRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Sla D
    registers.Write(Register::D, 0x80);

    auto preOpcode = 0xCB;
    auto opcode = 0x22;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers.Read(Register::D));

    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestSlaRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Sla (HL) -> HL = 0x12EE, [0x12EE] = 0xFF
    registers.WritePair(Register::HL, 0x12EE);

    auto preOpcode = 0xCB;
    auto opcode = 0x26;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x12EE, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xFE)), static_cast<uint16_t>(0x12EE)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestSraRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Sra A
    registers.Write(Register::A, 0x8A);

    auto preOpcode = 0xCB;
    auto opcode = 0x2F;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));

    controlUnit->RunCycle();

    EXPECT_EQ(0xC5, registers.Read(Register::A));

    EXPECT_EQ(0x00, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestSraRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Sla (HL) -> HL = 0x010A, [0x010A] = 0x01
    registers.WritePair(Register::HL, 0x010A);

    auto preOpcode = 0xCB;
    auto opcode = 0x2E;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x010A, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x01)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x00)), static_cast<uint16_t>(0x010A)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestSrlRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Srl A
    registers.Write(Register::A, 0x81);

    auto preOpcode = 0xCB;
    auto opcode = 0x3F;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x40, registers.Read(Register::A));

    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestSrlRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Srl (HL) -> HL = 0x999F, [0x999F] = 0xFF
    registers.WritePair(Register::HL, 0x999F);

    auto preOpcode = 0xCB;
    auto opcode = 0x3E;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x999F, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x7F)), static_cast<uint16_t>(0x999F)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestSwapRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Swap E
    registers.Write(Register::E, 0x00);

    auto preOpcode = 0xCB;
    auto opcode = 0x33;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers.Read(Register::E));

    EXPECT_EQ(0x00, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestSwapRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Swap (HL) -> HL = 0x0005, [0x0005] = 0xF0
    registers.WritePair(Register::HL, 0x0005);

    auto preOpcode = 0xCB;
    auto opcode = 0x36;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x0005, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xF0)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x0F)), static_cast<uint16_t>(0x0005)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestBitRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Bit 7, A
    registers.Write(Register::A, 0x80);
    registers.Write(Register::L, 0xEF);

    auto preOpcode = 0xCB;
    auto opcode = 0x40 | (0x07 << 0x03) | 0x07;
    auto opcode2 = 0x40 | (0x04 << 0x03) | 0x05;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode2)));

    registers.WriteFlag(Flag::CY, 0x01);
    controlUnit->RunCycle();

    EXPECT_EQ(0x80, registers.Read(Register::A));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
    
    registers.WriteFlag(Flag::CY, 0x00);
    controlUnit->RunCycle();

    EXPECT_EQ(0xEF, registers.Read(Register::L));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestBitRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Bit r, (HL); HL = 0xADF0 l [0xADF0] = 0xFE
    registers.WritePair(Register::HL, 0xADF0);

    auto preOpcode = 0xCB;
    auto opcode = 0x40 | (0x00 << 0x03) | 0x06;
    auto opcode2 = 0x40 | (0x01 << 0x03) | 0x06;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode2)));
    EXPECT_CALL(memoryController, Read(0xADF0, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFE)));

    registers.WriteFlag(Flag::CY, 0x01);
    controlUnit->RunCycle();

    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::Z));
    
    EXPECT_CALL(memoryController, Read(0xADF0, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFE)));

    registers.WriteFlag(Flag::CY, 0x00);
    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestSetRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // SET 3, A
    // SET 4, L
    registers.Write(Register::A, 0x80);
    registers.Write(Register::L, 0x3B);

    auto preOpcode = 0xCB;
    auto opcode = 0xC0 | (0x03 << 0x03) | 0x07;
    auto opcode2 = 0xC0 | (0x07 << 0x03) | 0x05;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode2)));

    registers.WriteFlag(Flag::CY, 0x01);
    registers.WriteFlag(Flag::Z, 0x0);
    registers.WriteFlag(Flag::H, 0x01);
    registers.WriteFlag(Flag::N, 0x01);
    controlUnit->RunCycle();

    EXPECT_EQ(0x88, registers.Read(Register::A));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
    
    registers.WriteFlag(Flag::CY, 0x00);
    registers.WriteFlag(Flag::Z, 0x01);
    registers.WriteFlag(Flag::H, 0x00);
    registers.WriteFlag(Flag::N, 0x01);
    controlUnit->RunCycle();

    EXPECT_EQ(0xBB, registers.Read(Register::L));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestSetRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Srl (HL) -> HL = 0x7800, [0x7800] = 0x00
    registers.WritePair(Register::HL, 0x7800);

    auto preOpcode = 0xCB;
    auto opcode = 0xC0 | (0x03 << 0x03) | 0x06;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x7800, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x08)), static_cast<uint16_t>(0x7800)));

    registers.WriteFlag(Flag::CY, 0x01);
    registers.WriteFlag(Flag::Z, 0x01);
    registers.WriteFlag(Flag::H, 0x01);
    registers.WriteFlag(Flag::N, 0x01);
    controlUnit->RunCycle();

    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestResRegister)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // RES 7, A
    // RES 1, L
    registers.Write(Register::A, 0x80);
    registers.Write(Register::L, 0x3B);

    auto preOpcode = 0xCB;
    auto opcode = 0x80 | (0x07 << 0x03) | 0x07;
    auto opcode2 = 0x80 | (0x01 << 0x03) | 0x05;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode2)));

    registers.WriteFlag(Flag::CY, 0x01);
    registers.WriteFlag(Flag::Z, 0x0);
    registers.WriteFlag(Flag::H, 0x01);
    registers.WriteFlag(Flag::N, 0x01);
    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers.Read(Register::A));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
    
    registers.WriteFlag(Flag::CY, 0x00);
    registers.WriteFlag(Flag::Z, 0x01);
    registers.WriteFlag(Flag::H, 0x00);
    registers.WriteFlag(Flag::N, 0x01);
    controlUnit->RunCycle();

    EXPECT_EQ(0x39, registers.Read(Register::L));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestResRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // Srl (HL) -> HL = 0x9111, [0x9111] = 0x00
    registers.WritePair(Register::HL, 0x9111);

    auto preOpcode = 0xCB;
    auto opcode = 0x80 | (0x03 << 0x03) | 0x06;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x9111, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xF7)), static_cast<uint16_t>(0x9111)));

    registers.WriteFlag(Flag::CY, 0x01);
    registers.WriteFlag(Flag::Z, 0x01);
    registers.WriteFlag(Flag::H, 0x01);
    registers.WriteFlag(Flag::N, 0x01);
    controlUnit->RunCycle();

    EXPECT_EQ(0x01, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestJpUnconditional)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    auto opcode = 0xC3;
    // Multiple jumps unconditional will be performed in this test
    
    // JP 0xFF00
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    // JP 0xEE12
    EXPECT_CALL(memoryController, Read(0xFF00, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0xFF01, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x12)));
    EXPECT_CALL(memoryController, Read(0xFF02, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xEE)));
    // JP 0x8732
    EXPECT_CALL(memoryController, Read(0xEE12, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0xEE13, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x32)));
    EXPECT_CALL(memoryController, Read(0xEE14, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x87)));
    // JP 0x0E00
    EXPECT_CALL(memoryController, Read(0x8732, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x8733, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    EXPECT_CALL(memoryController, Read(0x8734, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0E)));

    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x0E00, registers.ReadPair(Register::PC));
}

TEST(CoreTests_ControlUnit, TestJpConditional)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // LD A, 0x01  -> 0x3E
    //             -> 0x01
    // DEC A       -> 0x3D
    // JP.Z 0x8000 -> 0xCA
    //             -> 0x00
    //             -> 0x80
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x3E)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x01)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x3D)));
    EXPECT_CALL(memoryController, Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xCA)));
    EXPECT_CALL(memoryController, Read(0x0004, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    EXPECT_CALL(memoryController, Read(0x0005, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x80)));

    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x8000, registers.ReadPair(Register::PC));
}

TEST(CoreTests_ControlUnit, TestJrUnconditional)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    auto opcode = 0x18;
    // Multiple relative jumps unconditional will be performed in this test
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    // Note: The assembler will decrement 2 from the offset, so the original assembly instruction for this Jump is:
    // JR 0x72 (offest = 114 -> binary will become offset = 112).
    // Thats because the addition of the offser is done after the PC has been incremented (after acquiring the 1-byte immediate)
    // so the sum is made with PC + 2.
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x70)));
    
    controlUnit->RunCycle();
    EXPECT_EQ(0x0072, registers.ReadPair(Register::PC));

    EXPECT_CALL(memoryController, Read(0x0072, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    // Note: The assembler will decrement 2 from the offset, so the original assembly instruction for this Jump is:
    // JR 0xD8 (offest = -40 -> binary will become offset = -38).
    // Thats because the addition of the offser is done after the PC has been incremented (after acquiring the 1-byte immediate)
    // so the sum is made with PC + 2.
    EXPECT_CALL(memoryController, Read(0x0073, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xD6)));

    controlUnit->RunCycle();
    EXPECT_EQ(0x004A, registers.ReadPair(Register::PC));

    EXPECT_CALL(memoryController, Read(0x004A, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    // Note: The assembler will decrement 2 from the offset, so the original assembly instruction for this Jump is:
    // JR 0x02 (offest = 2 -> binary will become offset = 2).
    // Thats because the addition of the offser is done after the PC has been incremented (after acquiring the 1-byte immediate)
    // so the sum is made with PC + 2.
    EXPECT_CALL(memoryController, Read(0x004B, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));

    controlUnit->RunCycle();
    EXPECT_EQ(0x004C, registers.ReadPair(Register::PC));

    EXPECT_CALL(memoryController, Read(0x004C, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    // Note: The assembler will decrement 2 from the offset, so the original assembly instruction for this Jump is:
    // JR 0xB4 (offest = -76 -> binary will become offset = -74).
    // Thats because the addition of the offser is done after the PC has been incremented (after acquiring the 1-byte immediate)
    // so the sum is made with PC + 2.
    EXPECT_CALL(memoryController, Read(0x004D, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xB2)));

    controlUnit->RunCycle();
    EXPECT_EQ(0x0000, registers.ReadPair(Register::PC));
}


TEST(CoreTests_ControlUnit, TestJrConditional)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // LD A, 0x80  -> 0x3E
    //             -> 0x80
    // BIT 7, A    -> 0xCB
    //             -> 0xFF
    // JR Z, 0x10  -> 0x28
    //             -> 0x0E
    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x3E)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x80)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xCB)));
    EXPECT_CALL(memoryController, Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    EXPECT_CALL(memoryController, Read(0x0004, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x28)));
    EXPECT_CALL(memoryController, Read(0x0005, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0E)));

    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x0006, registers.ReadPair(Register::PC));
}

TEST(CoreTests_ControlUnit, TestJpUnconditionalRegisterIndirect)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // JP (HL) -> HL = 0x40FF,  (0x40FF) = 0x6755;
    auto opcode = 0xE9;

    registers.WritePair(Register::HL, 0x40FF);

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x40FF, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x55)));
    EXPECT_CALL(memoryController, Read(0x4100, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x67)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x6755, registers.ReadPair(Register::PC));
}

TEST(CoreTests_ControlUnit, TestUnconditionalCall)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // CALL 0x8000
    // ...
    auto opcode = 0xCD;
    // ; At address 0x7000
    // LD A, 0xFF
    auto opcode2 = 0x3E;
    // INC A
    auto opcode3 = 0x3C;

    registers.WritePair(Register::PC, 0x8000);
    registers.WritePair(Register::SP, 0xFFFE);

    
    EXPECT_CALL(memoryController, Read(0x8000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x8001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    EXPECT_CALL(memoryController, Read(0x8002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x70)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x80)), static_cast<uint16_t>(0xFFFD)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x03)), static_cast<uint16_t>(0xFFFC)));
    EXPECT_CALL(memoryController, Read(0x7000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode2)));
    EXPECT_CALL(memoryController, Read(0x7001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    EXPECT_CALL(memoryController, Read(0x7002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode3)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x7000, registers.ReadPair(Register::PC));
    EXPECT_EQ(0xFFFC, registers.ReadPair(Register::SP));

    controlUnit->RunCycle();

    EXPECT_EQ(0xFF, registers.Read(Register::A));
   
    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers.Read(Register::A));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestConditionalCall)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // CALL NZ 0x7000
    // ...
    auto opcode = 0xC0 | 0x00 | 0x04;
    // ; At address 0x7000
    // CALL CY 0x9000
    auto opcode2 = 0xC0 | (0x03 << 0x03) | 0x04;
    // LD A, 0xFF
    auto opcode3 = 0x3E;

    registers.WritePair(Register::PC, 0x8000);
    registers.WritePair(Register::SP, 0xFFFE);
    registers.WriteFlag(Flag::Z, 0x00);
    registers.WriteFlag(Flag::CY, 0x00);

    
    EXPECT_CALL(memoryController, Read(0x8000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x8001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    EXPECT_CALL(memoryController, Read(0x8002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x70)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x80)), static_cast<uint16_t>(0xFFFD)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x03)), static_cast<uint16_t>(0xFFFC)));
    EXPECT_CALL(memoryController, Read(0x7000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode2)));
    EXPECT_CALL(memoryController, Read(0x7001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    EXPECT_CALL(memoryController, Read(0x7002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x90)));
    EXPECT_CALL(memoryController, Read(0x7003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode3)));
    EXPECT_CALL(memoryController, Read(0x7004, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x7000, registers.ReadPair(Register::PC));
    EXPECT_EQ(0xFFFC, registers.ReadPair(Register::SP));

    controlUnit->RunCycle();

    EXPECT_EQ(0x7003, registers.ReadPair(Register::PC));
   
    controlUnit->RunCycle();

    EXPECT_EQ(0xFF, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestRet)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // At address 0x8000    
    // CALL 0x7000
    auto opcode = 0xCD;
    // At address 0x7000
        // LD A, 0xFE
        auto opcode2 = 0x3E;
        // Ret
        auto opcode3 = 0xC9;
    // Back at address 0x8004
    // INC A
    auto opcode4 = 0x3C;
    
    registers.WritePair(Register::PC, 0x8000);
    registers.WritePair(Register::SP, 0xFFFE);
    
    
    EXPECT_CALL(memoryController, Read(0x8000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x8001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    EXPECT_CALL(memoryController, Read(0x8002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x70)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x80)), static_cast<uint16_t>(0xFFFD)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x03)), static_cast<uint16_t>(0xFFFC)));
    EXPECT_CALL(memoryController, Read(0x7000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode2)));
    EXPECT_CALL(memoryController, Read(0x7001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFE)));
    EXPECT_CALL(memoryController, Read(0x7002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode3)));
    EXPECT_CALL(memoryController, Read(0xFFFC, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x03)));
    EXPECT_CALL(memoryController, Read(0xFFFD, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x80)));
    EXPECT_CALL(memoryController, Read(0x8003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode4)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x7000, registers.ReadPair(Register::PC));
    EXPECT_EQ(0xFFFC, registers.ReadPair(Register::SP));

    controlUnit->RunCycle();

    EXPECT_EQ(0x7002, registers.ReadPair(Register::PC));
    EXPECT_EQ(0xFE, registers.Read(Register::A));
   
    controlUnit->RunCycle();

    EXPECT_EQ(0xFFFE, registers.ReadPair(Register::SP));
    EXPECT_EQ(0x8003, registers.ReadPair(Register::PC));

    controlUnit->RunCycle();

    EXPECT_EQ(0xFF, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestConditionalRet)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // At address 0x8000    
    // CALL 0x7000
    auto opcode = 0xCD;
    // At address 0x7000
    // LD A, 0x01
    auto opcode2 = 0x3E;
    // INC A
    auto opcode3 = 0x3C;
    // RET Z
    auto opcode4 = 0xC0 | (0x01 << 0x03) | 0x00;
    // INC A
    auto opcode5 = 0x3C;

    registers.WritePair(Register::PC, 0x8000);
    registers.WritePair(Register::SP, 0xFFFE);
    
    
    EXPECT_CALL(memoryController, Read(0x8000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x8001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    EXPECT_CALL(memoryController, Read(0x8002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x70)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x80)), static_cast<uint16_t>(0xFFFD)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x03)), static_cast<uint16_t>(0xFFFC)));
    EXPECT_CALL(memoryController, Read(0x7000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode2)));
    EXPECT_CALL(memoryController, Read(0x7001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    EXPECT_CALL(memoryController, Read(0x7002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode3)));
    EXPECT_CALL(memoryController, Read(0xFFFC, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x03)));
    EXPECT_CALL(memoryController, Read(0xFFFD, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x80)));
    EXPECT_CALL(memoryController, Read(0x7003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode4)));
    EXPECT_CALL(memoryController, Read(0x7004, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode5)));

    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x02, registers.Read(Register::A));
    EXPECT_EQ(0x7005, registers.ReadPair(Register::PC));
}

TEST(CoreTests_ControlUnit, TestReti)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // At address 0x8000    
    // CALL 0x7000
    auto opcode = 0xCD;
    // At address 0x7000
        // LD A, 0xFE
        auto opcode2 = 0x3E;
        // Reti
        auto opcode3 = 0xD9;
    // Back at address 0x8004
    // INC A
    auto opcode4 = 0x3C;
    
    registers.WritePair(Register::PC, 0x8000);
    registers.WritePair(Register::SP, 0xFFFE);
    
    
    EXPECT_CALL(memoryController, Read(0x8000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x8001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    EXPECT_CALL(memoryController, Read(0x8002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x70)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x80)), static_cast<uint16_t>(0xFFFD)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x03)), static_cast<uint16_t>(0xFFFC)));
    EXPECT_CALL(memoryController, Read(0x7000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode2)));
    EXPECT_CALL(memoryController, Read(0x7001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFE)));
    EXPECT_CALL(memoryController, Read(0x7002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode3)));
    EXPECT_CALL(memoryController, Read(0xFFFC, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x03)));
    EXPECT_CALL(memoryController, Read(0xFFFD, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x80)));
    EXPECT_CALL(memoryController, Read(0x8003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode4)));

    controlUnit->RunCycle();

    EXPECT_EQ(0x7000, registers.ReadPair(Register::PC));
    EXPECT_EQ(0xFFFC, registers.ReadPair(Register::SP));
    EXPECT_FALSE(arithmeticLogicUnit.ClearInterruptStatusSignal());

    controlUnit->RunCycle();

    EXPECT_EQ(0x7002, registers.ReadPair(Register::PC));
    EXPECT_EQ(0xFE, registers.Read(Register::A));
    EXPECT_FALSE(arithmeticLogicUnit.ClearInterruptStatusSignal());
   
    controlUnit->RunCycle();

    EXPECT_EQ(0xFFFE, registers.ReadPair(Register::SP));
    EXPECT_EQ(0x8003, registers.ReadPair(Register::PC));
    EXPECT_TRUE(arithmeticLogicUnit.ClearInterruptStatusSignal());

    controlUnit->RunCycle();

    EXPECT_EQ(0xFF, registers.Read(Register::A));
    EXPECT_FALSE(arithmeticLogicUnit.ClearInterruptStatusSignal());
}

TEST(CoreTests_ControlUnit, TestRst)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // LD HL, 0x5000
    auto opcode1 = 0x21;
    // LD SP, HL
    auto opcode2 = 0xF9;
    // RST 0x11
    auto opcode3 = 0xDF;

    registers.WritePair(Register::PC, 0xAAA0);

    
    EXPECT_CALL(memoryController, Read(0xAAA0, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode1)));
    EXPECT_CALL(memoryController, Read(0xAAA1, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    EXPECT_CALL(memoryController, Read(0xAAA2, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x50)));
    EXPECT_CALL(memoryController, Read(0xAAA3, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode2)));
    EXPECT_CALL(memoryController, Read(0xAAA4, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode3)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xAA)), static_cast<uint16_t>(0x4FFF)));
    EXPECT_CALL(memoryController, Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xA5)), static_cast<uint16_t>(0x4FFE)));

    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x4FFE, registers.ReadPair(Register::SP));
    EXPECT_EQ(0x0018, registers.ReadPair(Register::PC));
}

TEST(CoreTests_ControlUnit, TestDaa)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // LD A, 0x45   0x3E
    //              0x45
    // LD B, 0x38   0x06
    //              0x38
    // ADD A, B     0x80
    // DAA          0x27
    // SUB A, B     0x90
    // DAA          0x27
    auto opcode1 = 0x3E;
    auto opcode2 = 0x06;
    auto opcode3 = 0x80;
    auto opcode4 = 0x27;
    auto opcode5 = 0x90;
    auto opcode6 = 0x27;

    registers.WritePair(Register::PC, 0x0000);

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x45)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode2)));
    EXPECT_CALL(memoryController, Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x38)));
    EXPECT_CALL(memoryController, Read(0x0004, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode3)));
    EXPECT_CALL(memoryController, Read(0x0005, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode4)));
    EXPECT_CALL(memoryController, Read(0x0006, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode5)));
    EXPECT_CALL(memoryController, Read(0x0007, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode6)));

    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x45, registers.Read(Register::A));
    EXPECT_EQ(0x38, registers.Read(Register::B));
    
    controlUnit->RunCycle();
    
    EXPECT_EQ(static_cast<uint8_t>(0x45 + 0x38), registers.Read(Register::A));
    
    controlUnit->RunCycle();
    
    EXPECT_EQ(0x83, registers.Read(Register::A));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::CY));
    
    controlUnit->RunCycle();
    
    EXPECT_EQ(0x4B, registers.Read(Register::A));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::N));
    
    controlUnit->RunCycle();
    
    EXPECT_EQ(0x45, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestCpl)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // LD A, 0x35
    // CPL A
    auto opcode1 = 0x3E;
    auto opcode2 = 0x2F;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x035)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode2)));

    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0xCA, registers.Read(Register::A));
}

TEST(CoreTests_ControlUnit, TestNop)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registers.WriteFlag(Flag::Z, 0x00);
    registers.WriteFlag(Flag::N, 0x00);
    registers.WriteFlag(Flag::H, 0x00);
    registers.WriteFlag(Flag::CY, 0x00);

    // LD A, 0xFF
    // CPL A
    // INC A
    auto opcode1 = 0x3E;
    auto opcode2 = 0x00;
    auto opcode3 = 0x3C;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode1)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode2)));
    EXPECT_CALL(memoryController, Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode3)));

    controlUnit->RunCycle();
    controlUnit->RunCycle();
    
    EXPECT_EQ(0xFF, registers.Read(Register::A));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::CY));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers.Read(Register::A));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::Z));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::N));
    EXPECT_EQ(0x01, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0x00, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0x0004, registers.ReadPair(Register::PC));
}

TEST(CoreTests_ControlUnit, TestHalt)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // HALT
    auto opcode = 0x76;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));

    controlUnit->RunCycle();

    EXPECT_TRUE(arithmeticLogicUnit.HaltSignal());
}

TEST(CoreTests_ControlUnit, TestStop)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // STOP
    auto opcode = 0x10;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));

    controlUnit->RunCycle();

    EXPECT_TRUE(arithmeticLogicUnit.StopSignal());
}

TEST(CoreTests_ControlUnit, TestEi)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // EI
    auto opcode = 0xFB;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));

    controlUnit->RunCycle();

    EXPECT_TRUE(arithmeticLogicUnit.InterruptMasterEnable());
}

TEST(CoreTests_ControlUnit, TestDi)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // DI
    auto opcode = 0xF3;

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));

    controlUnit->RunCycle();

    EXPECT_FALSE(arithmeticLogicUnit.InterruptMasterEnable());
}

TEST(CoreTests_ControlUnit, TestCcf)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // CCF
    auto opcode = 0x3F;

    registers.WriteFlag(Flag::CY, 1);
    registers.WriteFlag(Flag::H, 1);
    registers.WriteFlag(Flag::N, 1);
    registers.WriteFlag(Flag::Z, 1);

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));

    controlUnit->RunCycle();

    EXPECT_EQ(0, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0, registers.ReadFlag(Flag::N));
    EXPECT_EQ(1, registers.ReadFlag(Flag::Z));
}

TEST(CoreTests_ControlUnit, TestScf)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    // SCF
    auto opcode = 0x37;

    registers.WriteFlag(Flag::CY, 0);
    registers.WriteFlag(Flag::H, 1);
    registers.WriteFlag(Flag::N, 1);
    registers.WriteFlag(Flag::Z, 1);

    
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));

    controlUnit->RunCycle();

    EXPECT_EQ(1, registers.ReadFlag(Flag::CY));
    EXPECT_EQ(0, registers.ReadFlag(Flag::H));
    EXPECT_EQ(0, registers.ReadFlag(Flag::N));
    EXPECT_EQ(1, registers.ReadFlag(Flag::Z));
}

// [GBX ONLY] System Jump instruction
TEST(CoreTests_ControlUnit, TestJpu)
{
    RegisterBank registers;
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    arithmeticLogicUnit.Initialize(&registers);
    arithmeticLogicUnit.InitializeRegisters();

    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    auto preOpcode = 0xFC;
    auto opcode = 0xC3;
    // Perform unconditional jump to 16-bit user-space memory address
    
    // JP 0x0100
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(preOpcode)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(opcode)));
    EXPECT_CALL(memoryController, Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    EXPECT_CALL(memoryController, Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x01)));
    
    EXPECT_CALL(memoryController, SetSecurityLevel(PrivilegeMode::User));
    controlUnit->RunCycle();

    EXPECT_EQ(0x0100, registers.ReadPair(Register::PC));
}

TEST(CoreTests_ControlUnit, TestLDUWithRegisterIndirectSourceIncrement)
{
    MemoryControllerMock memoryController;
    ArithmeticLogicDecorator arithmeticLogicUnit;
    RegisterBank registerBank;
    arithmeticLogicUnit.Initialize(&registerBank);
    arithmeticLogicUnit.InitializeRegisters();
    auto controlUnit = make_shared<ControlUnit>();
         controlUnit->Initialize(&memoryController, &arithmeticLogicUnit);

    registerBank.WritePair(Register::DE, 0x0104);

    // First trigger to controlUnit. 
    
    // Content of the Operand 1 will be stored in the address held by HL.
    EXPECT_CALL(memoryController, Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFC)));
    EXPECT_CALL(memoryController, Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x1A)));
    EXPECT_CALL(memoryController, Read(0x0104, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xCC)));
    
    EXPECT_CALL(memoryController, SecurityLevel()).WillRepeatedly(Return(PrivilegeMode::User));
    EXPECT_CALL(memoryController, SetSecurityLevel(PrivilegeMode::User)).Times(2);
    controlUnit->RunCycle();

    EXPECT_EQ(0xCC, registerBank.Read(Register::A));
    EXPECT_EQ(0x0104, registerBank.ReadPair(Register::DE));
}