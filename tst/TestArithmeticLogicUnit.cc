#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cmath>
#include <thread>
#include <variant>

#include <memory>

#include "../src/ArithmeticLogicUnit.h"
#include "../src/MemoryControllerInterface.h"
#include "../src/MemoryController.h"
#include "../src/RegisterBank.h"

using namespace std;
using namespace gbx;
using ::testing::Return;

class ALUWrapperForTests : public ArithmeticLogicUnit
{
public:
    shared_ptr<RegisterBank> GetRegisterBank()
    {
        return this->_registers;
    }
};

class MemoryControllerMock : public MemoryControllerInterface
{
public:
    virtual ~MemoryControllerMock() = default;
    MOCK_METHOD((std::variant<uint8_t, uint16_t>), Read, (uint16_t a, MemoryAccessType b));
    MOCK_METHOD(void, Write, ((std::variant<uint8_t, uint16_t>), uint16_t));
    MOCK_METHOD(void, Load, ((std::shared_ptr<uint8_t*>), size_t, uint16_t, (std::optional<size_t>)));
    MOCK_METHOD(void, RegisterMemoryResource, ((std::shared_ptr<Memory>), AddressRange));
    MOCK_METHOD(void, UnregisterMemoryResource, ((std::shared_ptr<Memory>)));
};

TEST(TestArithmeticLogicUnit, FetchPCMessage)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x47)));
    
    alu->RunCycle();

    auto instructionRegister = alu->GetRegisterBank()->Read(Register::IR);
    auto programCounter = alu->GetRegisterBank()->ReadPair(Register::PC);

    EXPECT_EQ(0x47, instructionRegister);
    EXPECT_EQ(0x0001, programCounter);
}

TEST(TestArithmeticLogicUnit, TestExecutingUnknownInstructions)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    EXPECT_THROW(alu->RunCycle(), InstructionException);
}

TEST(TestArithmeticLogicUnit, TestAcquireSingleImmediateOperand)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);
    
    // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be 0x26 0xAA which translates to LD H, 0xAA (load H immediate with 0xAA)
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x26)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xAA)));
    
    alu->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0xAA, alu->GetRegisterBank()->Read(Register::H));
}

TEST(TestArithmeticLogicUnit, TestAcquireSingleRegisterIndirectOperand)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    // Initialize HL
    alu->GetRegisterBank()->WritePair(Register::HL, 0xAABB);
    
    // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be OPCODE: 0x7E  (HL): 0xCC where HL holds address 0xAABB
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    EXPECT_CALL((*mockPointer), Read(0xAABB, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xCC)));
   
    alu->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0xCC, alu->GetRegisterBank()->Read(Register::A));
}

TEST(TestArithmeticLogicUnit, TestAcquireSingleRegisterIndirectBCOperand)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    // Initialize HL
    alu->GetRegisterBank()->WritePair(Register::BC, 0x1234);

    // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be OPCODE: 0x0A  (BC): 0xDD where BC holds address 0x1234
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0A)));
    EXPECT_CALL((*mockPointer), Read(0x1234, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xDD)));
    
    alu->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0xDD, alu->GetRegisterBank()->Read(Register::A));
}

TEST(TestArithmeticLogicUnit, TestAcquireSingleRegisterIndirectDEOperand)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    // Initialize HL
    alu->GetRegisterBank()->WritePair(Register::DE, 0x6789);

     // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be OPCODE: 0x0A  (BC): 0xDD where BC holds address 0x1234
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x1A)));
    EXPECT_CALL((*mockPointer), Read(0x6789, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x22)));
    
    alu->RunCycle();

    EXPECT_EQ(0x22, alu->GetRegisterBank()->Read(Register::A));
}

TEST(TestArithmeticLogicUnit, TestAcquireSingleRegisterIndirectDestinationHL)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    // Initialize HL
    alu->GetRegisterBank()->WritePair(Register::HL, 0xAABB);
    alu->GetRegisterBank()->Write(Register::A, 0xCC);

    // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Write A (0xCC) to the location pointed by HL (0xAABB);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x77)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xCC)), static_cast<uint16_t>(0xAABB)));
    
    alu->RunCycle();
}

TEST(TestArithmeticLogicUnit, TestAcquireSingleRegisterIndirectDestinationDE)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    // Initialize HL
    alu->GetRegisterBank()->WritePair(Register::DE, 0x1234);
    alu->GetRegisterBank()->Write(Register::A, 0x99);

    // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be OPCODE: 0x77  write A to (HL) (A value will be 0xEE, HL will point to 0xAABB)
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x12)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x99)), static_cast<uint16_t>(0x1234)));

    alu->RunCycle();
}

TEST(TestArithmeticLogicUnit, TestAcquireSingleRegisterIndirectDestinationBC)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    // Initialize HL
    alu->GetRegisterBank()->WritePair(Register::BC, 0x9967);
    alu->GetRegisterBank()->Write(Register::A, 0x12);

    // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be OPCODE: 0x77  write A to (HL) (A value will be 0xEE, HL will point to 0xAABB)
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x02)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x12)), static_cast<uint16_t>(0x9967)));
    
    alu->RunCycle();
}

TEST(TestArithmeticLogicUnit, TestIndexedSourceOpcodeIX)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    alu->GetRegisterBank()->WritePair(Register::IX, 0x0100);
    
    // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // IX will have value 0x0100 so the final address will be 0x01AA, which will hold value 0x11
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xDD)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0A)));
    EXPECT_CALL((*mockPointer), Read(0x010A, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x11)));
    
    alu->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0x11, alu->GetRegisterBank()->Read(Register::A));
}

TEST(TestArithmeticLogicUnit, TestIndexedSourceOpcodeIY)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    alu->GetRegisterBank()->WritePair(Register::IY, 0x0A00);

    // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // IY will have value 0x0A00 so the final address will be 0x0A09, which will hold value 0xCA
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFD)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x09)));
    EXPECT_CALL((*mockPointer), Read(0x0A09, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xCA)));

    alu->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0xCA, alu->GetRegisterBank()->Read(Register::A));
}

TEST(TestArithmeticLogicUnit, TestIndexedSourceOpcodeIXNegativeDisplacement)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    alu->GetRegisterBank()->WritePair(Register::IY, 0x0200);

    // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // IY will have value 0x0200 so the final address will be 0x01FF, which will hold value 0x10
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFD)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    EXPECT_CALL((*mockPointer), Read(0x01FF, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x10)));

    alu->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0x10, alu->GetRegisterBank()->Read(Register::A));
}

TEST(TestArithmeticLogicUnit, TestIndexedDestinantionOpcodeIX)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    alu->GetRegisterBank()->Write(Register::A, 0x88);
    alu->GetRegisterBank()->WritePair(Register::IX, 0x0100);
    
    // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // IX will have value 0x0100 so the final address will be 0x0110, which will hold value 0x88
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xDD)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x77)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x10)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x88)), static_cast<uint16_t>(0x110)));
    
    alu->RunCycle();
}

TEST(TestArithmeticLogicUnit, TestExtendedSource)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Register A will be loaded with the content stored in address 0xDAAD (0x9E)
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFA)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xAD)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xDA)));
    EXPECT_CALL((*mockPointer), Read(0xDAAD, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x9E)));
    
    alu->RunCycle();

    EXPECT_EQ(0x9E, alu->GetRegisterBank()->Read(Register::A));
}

TEST(TestArithmeticLogicUnit, TestExtendedDestination)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    alu->GetRegisterBank()->Write(Register::A, 0xF8);

    // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Content of Register A will be soted in address 0xBABE
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xEA)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xBE)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xBA)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xF8)), static_cast<uint16_t>(0xBABE)));
    
    alu->RunCycle();
}

TEST(TestArithmeticLogicUnit, TestImmediateRegisterIndirect)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    alu->GetRegisterBank()->WritePair(Register::HL, 0xF5D2);

    // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Content of the Operand 1 will be stored in the address held by HL.
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x36)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xCC)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xCC)), static_cast<uint16_t>(0xF5D2)));
    
    alu->RunCycle();
}

TEST(TestArithmeticLogicUnit, TestRegisterIndirectSourceIncrement)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    alu->GetRegisterBank()->WritePair(Register::HL, 0x99AA);

    // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Content of the Operand 1 will be stored in the address held by HL.
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x2A)));
    EXPECT_CALL((*mockPointer), Read(0x99AA, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xAB)));
    
    alu->RunCycle();

    EXPECT_EQ(0xAB, alu->GetRegisterBank()->Read(Register::A));
    EXPECT_EQ(0x99AB, alu->GetRegisterBank()->ReadPair(Register::HL));
}

TEST(TestArithmeticLogicUnit, TestRegisterIndirectSourceDecrement)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    alu->GetRegisterBank()->WritePair(Register::HL, 0x88BB);

    // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Content of the Operand 1 will be stored in the address held by HL.
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x3A)));
    EXPECT_CALL((*mockPointer), Read(0x88BB, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x45)));
    
    alu->RunCycle();

    EXPECT_EQ(0x45, alu->GetRegisterBank()->Read(Register::A));
    EXPECT_EQ(0x88BA, alu->GetRegisterBank()->ReadPair(Register::HL));
}

TEST(TestArithmeticLogicUnit, TestRegisterIndirectDestinationIncrement)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    alu->GetRegisterBank()->Write(Register::A, 0x66);
    alu->GetRegisterBank()->WritePair(Register::HL, 0x1234);

    // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Content of the Operand 1 will be stored in the address held by HL.
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x22)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x66)), static_cast<uint16_t>(0x1234)));
    
    alu->RunCycle();

    EXPECT_EQ(0x1235, alu->GetRegisterBank()->ReadPair(Register::HL));
}

TEST(TestArithmeticLogicUnit, TestRegisterIndirectDestinationDecrement)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    alu->GetRegisterBank()->Write(Register::A, 0x41);
    alu->GetRegisterBank()->WritePair(Register::HL, 0x764A);

    // First trigger to ALU. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Content of the Operand 1 will be stored in the address held by HL.
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x32)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x41)), static_cast<uint16_t>(0x764A)));
    
    alu->RunCycle();

    EXPECT_EQ(0x7649, alu->GetRegisterBank()->ReadPair(Register::HL));
}