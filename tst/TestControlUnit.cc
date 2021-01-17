#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cmath>
#include <memory>
#include <thread>
#include <variant>
#include <random>

#include "../src/ArithmeticLogicUnit.h"
#include "../src/ControlUnit.h"
#include "../src/MemoryController.h"
#include "../src/RegisterBank.h"
#include "../src/interfaces/MemoryControllerInterface.h"

using namespace std;
using namespace gbx;
using namespace gbx::interfaces;
using ::testing::Return;

uint8_t ImmediateOpcode(Register source)
{
    return RegisterBank::ToInstructionRegisterPair(source) << 4 | 0x01;
}

class ControlUnitDecorator : public ControlUnit
{};

class ArithmeticLogicDecorator : public ArithmeticLogicUnit
{
public:
    shared_ptr<RegisterBankInterface> GetRegisterBank()
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
    MOCK_METHOD(void, RegisterMemoryResource, ((std::shared_ptr<MemoryInterface>), AddressRange));
    MOCK_METHOD(void, UnregisterMemoryResource, ((std::shared_ptr<MemoryInterface>)));
};

TEST(TestControlUnit, FetchPCMessage)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x47)));
    
    controlUnit->RunCycle();

    auto instructionRegister = registerBank->Read(Register::IR);
    auto programCounter = registerBank->ReadPair(Register::PC);

    EXPECT_EQ(0x47, instructionRegister);
    EXPECT_EQ(0x0001, programCounter);
}

TEST(TestControlUnit, TestExecutingUnknownInstructions)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    EXPECT_THROW(controlUnit->RunCycle(), InstructionException);
}

TEST(TestControlUnit, TestAcquireSingleImmediateOperand)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);
    
    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be 0x26 0xAA which translates to ArithmeticLogicUnitInterfaceogicUnit H, 0xAA (load H immediate with 0xAA)
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x26)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xAA)));
    
    controlUnit->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0xAA, registerBank->Read(Register::H));
}

TEST(TestControlUnit, TestAcquireSingleRegisterIndirectOperand)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // Initialize HL
    registerBank->WritePair(Register::HL, 0xAABB);
    
    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be OPCODE: 0x7E  (HL): 0xCC where HL holds address 0xAABB
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    EXPECT_CALL((*mockPointer), Read(0xAABB, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xCC)));
   
    controlUnit->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0xCC, registerBank->Read(Register::A));
}

TEST(TestControlUnit, TestAcquireSingleRegisterIndirectBCOperand)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // Initialize HL
    registerBank->WritePair(Register::BC, 0x1234);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be OPCODE: 0x0A  (BC): 0xDD where BC holds address 0x1234
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0A)));
    EXPECT_CALL((*mockPointer), Read(0x1234, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xDD)));
    
    controlUnit->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0xDD, registerBank->Read(Register::A));
}

TEST(TestControlUnit, TestAcquireSingleRegisterIndirectDEOperand)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // Initialize HL
    registerBank->WritePair(Register::DE, 0x6789);

     // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be OPCODE: 0x0A  (BC): 0xDD where BC holds address 0x1234
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x1A)));
    EXPECT_CALL((*mockPointer), Read(0x6789, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x22)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x22, registerBank->Read(Register::A));
}

TEST(TestControlUnit, TestAcquireSingleRegisterIndirectDestinationHL)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // Initialize HL
    registerBank->WritePair(Register::HL, 0xAABB);
    registerBank->Write(Register::A, 0xCC);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Write A (0xCC) to the location pointed by HL (0xAABB);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x77)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xCC)), static_cast<uint16_t>(0xAABB)));
    
    controlUnit->RunCycle();
}

TEST(TestControlUnit, TestAcquireSingleRegisterIndirectDestinationDE)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // Initialize HL
    registerBank->WritePair(Register::DE, 0x1234);
    registerBank->Write(Register::A, 0x99);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be OPCODE: 0x77  write A to (HL) (A value will be 0xEE, HL will point to 0xAABB)
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x12)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x99)), static_cast<uint16_t>(0x1234)));

    controlUnit->RunCycle();
}

TEST(TestControlUnit, TestAcquireSingleRegisterIndirectDestinationBC)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // Initialize HL
    registerBank->WritePair(Register::BC, 0x9967);
    registerBank->Write(Register::A, 0x12);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be OPCODE: 0x77  write A to (HL) (A value will be 0xEE, HL will point to 0xAABB)
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x02)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x12)), static_cast<uint16_t>(0x9967)));
    
    controlUnit->RunCycle();
}

TEST(TestControlUnit, TestIndexedSourceOpcodeIX)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    registerBank->WritePair(Register::IX, 0x0100);
    
    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // IX will have value 0x0100 so the final address will be 0x01AA, which will hold value 0x11
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xDD)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0A)));
    EXPECT_CALL((*mockPointer), Read(0x010A, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x11)));
    
    controlUnit->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0x11, registerBank->Read(Register::A));
}

TEST(TestControlUnit, TestIndexedSourceOpcodeIY)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    registerBank->WritePair(Register::IY, 0x0A00);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // IY will have value 0x0A00 so the final address will be 0x0A09, which will hold value 0xCA
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFD)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x09)));
    EXPECT_CALL((*mockPointer), Read(0x0A09, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xCA)));

    controlUnit->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0xCA, registerBank->Read(Register::A));
}

TEST(TestControlUnit, TestIndexedSourceOpcodeIXNegativeDisplacement)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    registerBank->WritePair(Register::IY, 0x0200);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // IY will have value 0x0200 so the final address will be 0x01FF, which will hold value 0x10
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFD)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    EXPECT_CALL((*mockPointer), Read(0x01FF, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x10)));

    controlUnit->RunCycle();

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0x10, registerBank->Read(Register::A));
}

TEST(TestControlUnit, TestIndexedDestinantionOpcodeIX)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    registerBank->Write(Register::A, 0x88);
    registerBank->WritePair(Register::IX, 0x0100);
    
    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // IX will have value 0x0100 so the final address will be 0x0110, which will hold value 0x88
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xDD)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x77)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x10)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x88)), static_cast<uint16_t>(0x110)));
    
    controlUnit->RunCycle();
}

TEST(TestControlUnit, TestExtendedSource)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Register A will be loaded with the content stored in address 0xDAAD (0x9E)
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFA)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xAD)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xDA)));
    EXPECT_CALL((*mockPointer), Read(0xDAAD, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x9E)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x9E, registerBank->Read(Register::A));
}

TEST(TestControlUnit, TestExtendedDestination)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    registerBank->Write(Register::A, 0xF8);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Content of Register A will be soted in address 0xBABE
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xEA)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xBE)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xBA)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xF8)), static_cast<uint16_t>(0xBABE)));
    
    controlUnit->RunCycle();
}

TEST(TestControlUnit, TestImmediateRegisterIndirect)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    registerBank->WritePair(Register::HL, 0xF5D2);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Content of the Operand 1 will be stored in the address held by HL.
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x36)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xCC)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xCC)), static_cast<uint16_t>(0xF5D2)));
    
    controlUnit->RunCycle();
}

TEST(TestControlUnit, TestRegisterIndirectSourceIncrement)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    registerBank->WritePair(Register::HL, 0x99AA);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Content of the Operand 1 will be stored in the address held by HL.
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x2A)));
    EXPECT_CALL((*mockPointer), Read(0x99AA, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xAB)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0xAB, registerBank->Read(Register::A));
    EXPECT_EQ(0x99AB, registerBank->ReadPair(Register::HL));
}

TEST(TestControlUnit, TestRegisterIndirectSourceDecrement)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    registerBank->WritePair(Register::HL, 0x88BB);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Content of the Operand 1 will be stored in the address held by HL.
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x3A)));
    EXPECT_CALL((*mockPointer), Read(0x88BB, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x45)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x45, registerBank->Read(Register::A));
    EXPECT_EQ(0x88BA, registerBank->ReadPair(Register::HL));
}

TEST(TestControlUnit, TestRegisterIndirectDestinationIncrement)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    registerBank->Write(Register::A, 0x66);
    registerBank->WritePair(Register::HL, 0x1234);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Content of the Operand 1 will be stored in the address held by HL.
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x22)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x66)), static_cast<uint16_t>(0x1234)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x1235, registerBank->ReadPair(Register::HL));
}

TEST(TestControlUnit, TestRegisterIndirectDestinationDecrement)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    registerBank->Write(Register::A, 0x41);
    registerBank->WritePair(Register::HL, 0x764A);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Content of the Operand 1 will be stored in the address held by HL.
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x32)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x41)), static_cast<uint16_t>(0x764A)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x7649, registerBank->ReadPair(Register::HL));
}

TEST(TestControlUnit, TestImplicitRegisterSource)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    registerBank->Write(Register::C, 0x55);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Register A will hold content pointed by [C + 0xFF00]
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xF2)));
    EXPECT_CALL((*mockPointer), Read(0xFF55, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xD0)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0xD0, registerBank->Read(Register::A));
}

TEST(TestControlUnit, TestImplicitRegisterDestination)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    registerBank->Write(Register::A, 0xA6);
    registerBank->Write(Register::C, 0xF4);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // [C + 0xFF00] will hold the content od register A
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xE2)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xA6)), static_cast<uint16_t>(0xFFF4)));
    
    controlUnit->RunCycle();
}

TEST(TestControlUnit, TestImplicitImmediateSource)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Register A will hold content pointed by [C + 0xFF00]
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xF0)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x60)));
    EXPECT_CALL((*mockPointer), Read(0xFF60, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x09)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x09, registerBank->Read(Register::A));
}

TEST(TestControlUnit, TestImplicitImmediateDestination)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    registerBank->Write(Register::A, 0xD2);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Register A will hold content pointed by [C + 0xFF00]
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xE0)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x06)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xD2)), static_cast<uint16_t>(0xFF06)));
    
    controlUnit->RunCycle();
}

TEST(TestControlUnit, TestImmediatePair)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Register A will hold content pointed by [C + 0xFF00]
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(ImmediateOpcode(Register::BC))));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x4A)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x98)));
    EXPECT_CALL((*mockPointer), Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(ImmediateOpcode(Register::DE))));
    EXPECT_CALL((*mockPointer), Read(0x0004, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xBB)));
    EXPECT_CALL((*mockPointer), Read(0x0005, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x1A)));
    EXPECT_CALL((*mockPointer), Read(0x0006, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(ImmediateOpcode(Register::HL))));
    EXPECT_CALL((*mockPointer), Read(0x0007, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x99)));
    EXPECT_CALL((*mockPointer), Read(0x0008, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x50)));
    EXPECT_CALL((*mockPointer), Read(0x0009, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(ImmediateOpcode(Register::SP))));
    EXPECT_CALL((*mockPointer), Read(0x000A, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x24)));
    EXPECT_CALL((*mockPointer), Read(0x000B, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xCE)));

    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x984A, registerBank->ReadPair(Register::BC));
    EXPECT_EQ(0x1ABB, registerBank->ReadPair(Register::DE));
    EXPECT_EQ(0x5099, registerBank->ReadPair(Register::HL));
    EXPECT_EQ(0xCE24, registerBank->ReadPair(Register::SP));
}

TEST(TestControlUnit, TestTransferToSP)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    registerBank->WritePair(Register::HL, 0x65A1);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Register A will hold content pointed by [C + 0xFF00]
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xF9)));
    
    controlUnit->RunCycle();
    EXPECT_EQ(0x65A1, registerBank->ReadPair(Register::SP));
}

TEST(TestControlUnit, TestAddRegister)
{
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    auto registerBank = make_shared<RegisterBank>();
    arithmeticLogicUnit->Initialize(registerBank);
    arithmeticLogicUnit->InitializeRegisters();
    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // ADD A, C
    registerBank->Write(Register::C, 0x45);
    registerBank->Write(Register::A, 0x82);

    auto rawBinary = 0x80 | RegisterBank::ToInstructionSource(Register::C);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Register A will hold content pointed by [C + 0xFF00]
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(rawBinary)));
    
    controlUnit->RunCycle();
    EXPECT_EQ(0xC7, registerBank->Read(Register::A));
}

TEST(TestControlUnit, TestAddImmediate)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // ADD A, #73
    registers->Write(Register::A, 0x24);

    auto rawBinary = 0xC6;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Register A will hold content pointed by [C + 0xFF00]
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(rawBinary)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x73)));
    
    controlUnit->RunCycle();
    EXPECT_EQ(0x97, registers->Read(Register::A));
}

TEST(TestControlUnit, TestAddRegisterIndirect)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // ADD A, (HL)
    // where HL = 0x75E1 => 0x40
    registers->Write(Register::A, 0x60);
    registers->WritePair(Register::HL, 0x75E1);

    auto rawBinary = 0x86;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Register A will hold content pointed by [C + 0xFF00]
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(rawBinary)));
    EXPECT_CALL((*mockPointer), Read(0x75E1, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x40)));
    
    controlUnit->RunCycle();
    EXPECT_EQ(0xA0, registers->Read(Register::A));
}

TEST(TestControlUnit, TestAdcRegister)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // ADC A, B ; CY = 1 : A = 0x00
    // ADC A, D ; CY = 0 : A = 0x10
    // ADC A, E ; CY = 0 : A = 0x1A 
    registers->Write(Register::A, 0xFF);
    registers->Write(Register::B, 0x01);
    registers->Write(Register::D, 0x0F);
    registers->Write(Register::E, 0x0A);

    auto instruction1 = 0x88 | RegisterBank::ToInstructionSource(Register::B);
    auto instruction2 = 0x88 | RegisterBank::ToInstructionSource(Register::D);
    auto instruction3 = 0x88 | RegisterBank::ToInstructionSource(Register::E);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Run three instructions to evaluate the effects of running adc with and without carry.
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction2)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction3)));
    
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x1A, registers->Read(Register::A));
}

TEST(TestControlUnit, TestAdcImmediate)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();


    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // ADC A, #0xFF ; CY = 1 : A = 0x00
    // ADC A, #0x0E ; CY = 0 : A = 0x0F
    // ADC A, #0x01 ; CY = 0 : A = 0x10 
    registers->Write(Register::A, 0x01);

    auto instruction1 = 0xCE;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Run three instructions to evaluate the effects of running adc with and without carry.
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0E)));
    EXPECT_CALL((*mockPointer), Read(0x0004, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0005, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x01)));
    
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x10, registers->Read(Register::A));
}

TEST(TestControlUnit, TestAdcRegisterIndirect)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // ADC A, (HL) ; HL = #45DA -> 0x4C A -> 0x04 CY = 0
    // ADC A, (HL) ; HL = #3BFF -> 0xB0 A -> 0x00 CY = 1
    // ADC A, (HL) ; HL = #814A -> 0x0F A -> 0x10 CY = 0
    registers->Write(Register::A, 0x04);
    registers->WritePair(Register::HL, 0x45DC);

    auto instruction1 = 0x8E;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Run three instructions to evaluate the effects of running adc with and without carry.
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x45DC, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x4C)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x3BFF, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xB0)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x814A, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0F)));
    
    controlUnit->RunCycle();
    registers->WritePair(Register::HL, 0x3BFF);
    controlUnit->RunCycle();
    registers->WritePair(Register::HL, 0x814A);
    controlUnit->RunCycle();

    EXPECT_EQ(0x10, registers->Read(Register::A));
}

TEST(TestControlUnit, TestSubRegister)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // SUB A, L
    registers->Write(Register::L, 0x56);
    registers->Write(Register::A, 0x16);

    auto rawBinary = 0x90 | RegisterBank::ToInstructionSource(Register::L);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(rawBinary)));
    
    controlUnit->RunCycle();
    EXPECT_EQ(0xC0, registers->Read(Register::A));
}

TEST(TestControlUnit, TestSubImmediate)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // SUB A, #FF
    registers->Write(Register::A, 0x01);

    auto rawBinary = 0xD6;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(rawBinary)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    
    controlUnit->RunCycle();
    EXPECT_EQ(0x02, registers->Read(Register::A));
}

TEST(TestControlUnit, TestSubRegisterIndirect)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // SUB A, (HL)
    // HL = 0x65EE => 0xAA
    registers->Write(Register::A, 0xAA);
    registers->WritePair(Register::HL, 0x65EE);

    auto rawBinary = 0x96;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(rawBinary)));
    EXPECT_CALL((*mockPointer), Read(0x65EE, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xAA)));
    
    controlUnit->RunCycle();
    EXPECT_EQ(0x00, registers->Read(Register::A));
}

TEST(TestControlUnit, TestSbcRegister)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // SBC A, C ; CY is 1
    registers->Write(Register::A, 0x00);
    registers->Write(Register::C, 0x01);
    registers->Write(Register::D, 0xFE);

    auto instruction1 = 0x98 | RegisterBank::ToInstructionSource(Register::C);
    auto instruction2 = 0x98 | RegisterBank::ToInstructionSource(Register::D);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction2)));
    
    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers->Read(Register::A));
}

TEST(TestControlUnit, TestSbcImmediate)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    registers->Write(Register::A, 0x10);

    auto instruction1 = 0xDE;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x08)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x08)));

    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers->Read(Register::A));
}

TEST(TestControlUnit, TestSbcRegisterIndirect)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    registers->Write(Register::A, 0x09);
    registers->WritePair(Register::HL, 0x67DD);

    auto instruction1 = 0x9E;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x67DD, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x09)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x87D1, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x01)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x7EEE, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x01)));

    controlUnit->RunCycle();
    registers->WritePair(Register::HL, 0x87D1);
    controlUnit->RunCycle();
    registers->WritePair(Register::HL, 0x7EEE);
    controlUnit->RunCycle();

    EXPECT_EQ(0xFD, registers->Read(Register::A));
}

TEST(TestControlUnit, TestAndRegister)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // AND A, C
    registers->Write(Register::A, 0xF1);
    registers->Write(Register::C, 0x8F);

    auto instruction1 = 0xA0 | RegisterBank::ToInstructionSource(Register::C);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x81, registers->Read(Register::A));
}

TEST(TestControlUnit, TestAndImmediate)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // AND A, #0A
    registers->Write(Register::A, 0xAA);

    auto instruction1 = 0xE6;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0A)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x0A, registers->Read(Register::A));
}

TEST(TestControlUnit, TestAndRegisterIndirect)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // AND A, (HL)
    registers->Write(Register::A, 0xFF);
    registers->WritePair(Register::HL, 0x8A5D);

    auto instruction1 = 0xA6;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x8A5D, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers->Read(Register::A));
}

TEST(TestControlUnit, TestOrRegister)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // OR A, C
    registers->Write(Register::A, 0xF0);
    registers->Write(Register::C, 0x0F);

    auto instruction1 = 0xB0 | RegisterBank::ToInstructionSource(Register::C);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0xFF, registers->Read(Register::A));
}

TEST(TestControlUnit, TestOrImmediate)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // OR A, #0x55
    registers->Write(Register::A, 0xAA);

    auto instruction1 = 0xF6;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x55)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0xFF, registers->Read(Register::A));
}

TEST(TestControlUnit, TestOrRegisterIndirect)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // OR A, (HL))
    registers->Write(Register::A, 0x80);
    registers->WritePair(Register::HL, 0x1111);

    auto instruction1 = 0xB6;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x1111, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0F)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x8F, registers->Read(Register::A));
}

TEST(TestControlUnit, TestXorRegister)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // XOR A, D
    registers->Write(Register::A, 0x45);
    registers->Write(Register::D, 0x45);

    auto instruction1 = 0xA8 | RegisterBank::ToInstructionSource(Register::D);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x00, registers->Read(Register::A));
}

TEST(TestControlUnit, TestXorImmediate)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // XOR A, #79
    registers->Write(Register::A, 0xFF);

    auto instruction1 = 0xEE;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x79)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x86, registers->Read(Register::A));
}

TEST(TestControlUnit, TestXorRegisterIndirect)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // XOR A, (HL)
    registers->Write(Register::A, 0x81);
    registers->WritePair(Register::HL, 0x0914);

    auto instruction1 = 0xAE;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0914, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0xFF, registers->Read(Register::A));
}

TEST(TestControlUnit, TestCpRegister)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // CP A, D
    registers->Write(Register::A, 0xA1);
    registers->Write(Register::D, 0xA0);

    auto instruction1 = 0xB8 | RegisterBank::ToInstructionSource(Register::D);

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0xA1, registers->Read(Register::A));
    EXPECT_EQ(0x00, registers->ReadFlag(Flag::Z)); // A is different then D
}

TEST(TestControlUnit, TestCpImmediate)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // CP A, #01
    registers->Write(Register::A, 0x10);

    auto instruction1 = 0xFE;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x01)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x11)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x10, registers->Read(Register::A));
    EXPECT_EQ(0x01, registers->ReadFlag(Flag::H)); // A i biggern than 0x01
    EXPECT_EQ(0x00, registers->ReadFlag(Flag::CY));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x10, registers->Read(Register::A));
    EXPECT_EQ(0x01, registers->ReadFlag(Flag::H)); // A i smaller than 0x011
    EXPECT_EQ(0x01, registers->ReadFlag(Flag::CY));
}

TEST(TestControlUnit, TestCpRegisterIndirect)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // CP A, (HL))
    registers->Write(Register::A, 0x05);
    registers->WritePair(Register::HL, 0x50D3);

    auto instruction1 = 0xBE;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x50D3, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x05, registers->Read(Register::A));
    EXPECT_EQ(0x00, registers->ReadFlag(Flag::Z)); 
    EXPECT_EQ(0x01, registers->ReadFlag(Flag::H)); // A is smaller then D
    EXPECT_EQ(0x01, registers->ReadFlag(Flag::CY));
}

TEST(TestControlUnit, TestIncRegister)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // INC H
    registers->Write(Register::H, 0x08);

    auto instruction1 = 0x04 | (RegisterBank::ToInstructionSource(Register::H)) << 0x03;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x09, registers->Read(Register::H));
}

TEST(TestControlUnit, TestIncRegisterIndirect)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // INC (HL)
    registers->WritePair(Register::HL, 0x6589);

    auto instruction1 = 0x34;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x6589, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x56)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x57)), static_cast<uint16_t>(0x6589)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(registers->ReadPair(Register::HL), 0x6589);
    EXPECT_EQ(registers->ReadPair(Register::PC), 0x0001);
    
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x6589, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x57)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x58)), static_cast<uint16_t>(0x6589)));

    controlUnit->RunCycle();

    EXPECT_EQ(registers->ReadPair(Register::HL), 0x6589);
    EXPECT_EQ(registers->ReadPair(Register::PC), 0x0002);
}

TEST(TestControlUnit, TestDecRegister)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // INC H
    registers->Write(Register::H, 0x4E);

    auto instruction1 = 0x05 | (RegisterBank::ToInstructionSource(Register::H)) << 0x03;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x4D, registers->Read(Register::H));
}

TEST(TestControlUnit, TestDecRegisterIndirect)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // INC (HL)
    registers->WritePair(Register::HL, 0x66A1);

    auto instruction1 = 0x35;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x66A1, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x70)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x6F)), static_cast<uint16_t>(0x66A1)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(registers->ReadPair(Register::HL), 0x66A1);
    EXPECT_EQ(registers->ReadPair(Register::PC), 0x0001);
    
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x66A1, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x00)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xFF)), static_cast<uint16_t>(0x66A1)));

    controlUnit->RunCycle();

    EXPECT_EQ(registers->ReadPair(Register::HL), 0x66A1);
    EXPECT_EQ(registers->ReadPair(Register::PC), 0x0002);

}

TEST(TestControlUnit, TestPush)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // set stackPoiner
    auto spValue = static_cast<uint16_t>(0x8000);
    registers->WritePair(Register::SP, spValue);

    auto instruction0 = (0x03 << 0x06) | (RegisterBank::ToInstructionRegisterPushPair(Register::BC)) << 0x04 | 0x05;
    auto instruction1 = (0x03 << 0x06) | (RegisterBank::ToInstructionRegisterPushPair(Register::DE)) << 0x04 | 0x05;
    auto instruction2 = (0x03 << 0x06) | (RegisterBank::ToInstructionRegisterPushPair(Register::HL)) << 0x04 | 0x05;
    auto instruction3 = (0x03 << 0x06) | (RegisterBank::ToInstructionRegisterPushPair(Register::AF)) << 0x04 | 0x05;
    registers->WritePair(Register::BC, 0x65DD);
    registers->WritePair(Register::DE, 0xEE11);
    registers->WritePair(Register::HL, 0x0814);
    registers->WritePair(Register::AF, 0xAAB1);

    auto pcValue = 0x0000;

    for (auto i = 0; i < 1000; i++)
    {
        // First trigger to controlUnit. 
        auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
        EXPECT_CALL((*mockPointer), Read(static_cast<uint16_t>(pcValue++), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction0)));
        EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x65)), static_cast<uint16_t>(spValue)));
        EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xDD)), static_cast<uint16_t>(spValue - 1)));

        controlUnit->RunCycle();
        spValue -= 2;
        EXPECT_EQ(spValue, registers->ReadPair(Register::SP));

        EXPECT_CALL((*mockPointer), Read(static_cast<uint16_t>(pcValue++), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
        EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xEE)), static_cast<uint16_t>(spValue)));
        EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x11)), static_cast<uint16_t>(spValue - 1)));
        
        controlUnit->RunCycle();
        spValue -= 2;
        EXPECT_EQ(spValue, registers->ReadPair(Register::SP));

        EXPECT_CALL((*mockPointer), Read(static_cast<uint16_t>(pcValue++), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction2)));
        EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x08)), static_cast<uint16_t>(spValue)));
        EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x14)), static_cast<uint16_t>(spValue - 1)));
        
        controlUnit->RunCycle();
        spValue -= 2;
        EXPECT_EQ(spValue, registers->ReadPair(Register::SP));

        EXPECT_CALL((*mockPointer), Read(static_cast<uint16_t>(pcValue++), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction3)));
        EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xAA)), static_cast<uint16_t>(spValue)));
        EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xB1)), static_cast<uint16_t>(spValue - 1)));
        
        controlUnit->RunCycle();
        spValue -= 2;
        EXPECT_EQ(spValue, registers->ReadPair(Register::SP));
    }
}


TEST(TestControlUnit, TestPop)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // set stackPoiner
    auto spValue = static_cast<uint16_t>(0x8000);
    registers->WritePair(Register::SP, spValue);

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
        auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
        EXPECT_CALL((*mockPointer), Read(static_cast<uint16_t>(pcValue++), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction0)));
        EXPECT_CALL((*mockPointer), Read(static_cast<uint16_t>(spValue), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>((operandValue0 >> 0x08) & 0xFF)));
        EXPECT_CALL((*mockPointer), Read(static_cast<uint16_t>(spValue + 1), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>((operandValue0 & 0xFF))));

        controlUnit->RunCycle();
        spValue += 2;
        EXPECT_EQ(spValue, registers->ReadPair(Register::SP));
        EXPECT_EQ(operandValue0, registers->ReadPair(Register::BC));

        EXPECT_CALL((*mockPointer), Read(static_cast<uint16_t>(pcValue++), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
        EXPECT_CALL((*mockPointer), Read(static_cast<uint16_t>(spValue), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>((operandValue1 >> 0x08) & 0xFF)));
        EXPECT_CALL((*mockPointer), Read(static_cast<uint16_t>(spValue + 1), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>((operandValue1 & 0xFF))));

        controlUnit->RunCycle();
        spValue += 2;
        EXPECT_EQ(spValue, registers->ReadPair(Register::SP));
        EXPECT_EQ(operandValue1, registers->ReadPair(Register::DE));

        EXPECT_CALL((*mockPointer), Read(static_cast<uint16_t>(pcValue++), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction2)));
        EXPECT_CALL((*mockPointer), Read(static_cast<uint16_t>(spValue), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>((operandValue2 >> 0x08) & 0xFF)));
        EXPECT_CALL((*mockPointer), Read(static_cast<uint16_t>(spValue + 1), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>((operandValue2 & 0xFF))));

        controlUnit->RunCycle();
        spValue += 2;
        EXPECT_EQ(spValue, registers->ReadPair(Register::SP));
        EXPECT_EQ(operandValue2, registers->ReadPair(Register::HL));


        EXPECT_CALL((*mockPointer), Read(static_cast<uint16_t>(pcValue++), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction3)));
        EXPECT_CALL((*mockPointer), Read(static_cast<uint16_t>(spValue), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>((operandValue3 >> 0x08) & 0xFF)));
        EXPECT_CALL((*mockPointer), Read(static_cast<uint16_t>(spValue + 1), MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>((operandValue3 & 0xFF))));

        controlUnit->RunCycle();
        spValue += 2;
        EXPECT_EQ(spValue, registers->ReadPair(Register::SP));
        EXPECT_EQ(operandValue3, registers->ReadPair(Register::AF));
    }
}

TEST(TestControlUnit, TestLdhlRegister)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // INC H
    registers->WritePair(Register::SP, 0x31DA);

    auto instruction1 = 0xF8;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x06)));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x31E0, registers->ReadPair(Register::HL));
}

TEST(TestControlUnit, TestSPTransferToMemory)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // INC H
    registers->WritePair(Register::SP, 0x87A1);

    auto instruction1 = 0x08;

    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xA1)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x67)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xA1)), static_cast<uint16_t>(0x67A1)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x87)), static_cast<uint16_t>(0x67A2)));
    
    controlUnit->RunCycle();
}

TEST(TestControlUnit, TestSPImmediate)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    registers->WritePair(Register::SP, 0xFFFE);

    auto instruction1 = 0xE8;

    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x01)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    EXPECT_CALL((*mockPointer), Read(0x0004, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0005, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x02)));
    

    controlUnit->RunCycle();

    EXPECT_EQ(0xFFFF, registers->ReadPair(Register::SP));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0xFFFE, registers->ReadPair(Register::SP));
    
    controlUnit->RunCycle();

    EXPECT_EQ(0x0000, registers->ReadPair(Register::SP));
}

TEST(TestControlUnit, TestIncRegisterPair)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // INC BC
    registers->WritePair(Register::BC, 0x80FF);

    auto instruction1 = 0x03 | (RegisterBank::ToInstructionRegisterPair(Register::BC)) << 0x04;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0004, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0005, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x8105, registers->ReadPair(Register::BC));
}


TEST(TestControlUnit, TestDecRegisterPair)
{
    shared_ptr<RegisterBank> registers = make_shared<RegisterBank>();
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    shared_ptr<ArithmeticLogicUnitInterface> arithmeticLogicUnit = make_shared<ArithmeticLogicDecorator>();
    arithmeticLogicUnit->Initialize(registers);
    arithmeticLogicUnit->InitializeRegisters();

    auto controlUnit = make_shared<ControlUnitDecorator>();
         controlUnit->Initialize(memoryController, arithmeticLogicUnit);

    // INC BC
    registers->WritePair(Register::DE, 0x0010);

    auto instruction1 = 0x0B | (RegisterBank::ToInstructionRegisterPair(Register::DE)) << 0x04;

    // First trigger to controlUnit. 
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0003, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0004, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0005, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0006, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    EXPECT_CALL((*mockPointer), Read(0x0007, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(instruction1)));
    
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();
    controlUnit->RunCycle();

    EXPECT_EQ(0x0008, registers->ReadPair(Register::DE));
}