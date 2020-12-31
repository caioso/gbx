#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cmath>
#include <thread>
#include <variant>

#include <memory>

#include "../src/ArithmeticLogicUnit.h"
#include "../src/Channel.h"
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

    ALUState GetState()
    {
        return this->_state;
    }

    void SetStateForTest(ALUState state)
    {
        this->_state = state;
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

TEST(TestArithmeticLogicUnit, Construction) 
{
    auto alu = make_shared<ALUWrapperForTests>();
    EXPECT_EQ(ALUState::Idle, alu->GetState());
}

TEST(TestArithmeticLogicUnit, FetchPCMessage)
{
    auto testPassed = false;
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    auto controlUnitChannel = make_shared<Channel<ALUMessage>>();
    controlUnitChannel->OnReceived([&testPassed](ALUMessage message) -> void 
    {
        if (message == ALUMessage::ReadyToDecode)
            testPassed = true;
        return;
    });

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUControlUnitChannel->Bind(controlUnitChannel);

    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xAA)));
    controlUnitChannel->Send(ALUMessage::Fetch);

    auto instructionRegister = alu->GetRegisterBank()->Read(Register::IR);
    auto programCounter = alu->GetRegisterBank()->ReadPair(Register::PC);

    EXPECT_EQ(ALUState::FetchingPC, alu->GetState());
    EXPECT_EQ(0xAA, instructionRegister);
    EXPECT_EQ(0x0001, programCounter);
    EXPECT_TRUE(testPassed);
}

TEST(TestArithmeticLogicUnit, TestDecoding)
{
    auto testPassed = false;
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    auto controlUnitChannel = make_shared<Channel<ALUMessage>>();
    controlUnitChannel->OnReceived([&testPassed, &controlUnitChannel](ALUMessage message) -> void 
    {
        if (message == ALUMessage::ReadyToDecode)
            controlUnitChannel->Send(ALUMessage::Decode);
        else if (message == ALUMessage::ReadyToExecute)
            testPassed = true;
    });

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUControlUnitChannel->Bind(controlUnitChannel);

    // First trigger to ALU. Subsequent changes are handled in the CU-ALU channel
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x47)));
    controlUnitChannel->Send(ALUMessage::Fetch);

    EXPECT_EQ(ALUState::Decoding, alu->GetState());
    EXPECT_TRUE(testPassed);    
}

TEST(TestArithmeticLogicUnit, TestExecuting)
{
    auto testPassed = false;
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    auto controlUnitChannel = make_shared<Channel<ALUMessage>>();
    controlUnitChannel->OnReceived([&testPassed, &controlUnitChannel](ALUMessage message) -> void 
    {
        if (message == ALUMessage::ReadyToDecode)
            controlUnitChannel->Send(ALUMessage::Decode);
        else if (message == ALUMessage::ReadyToExecute)
            controlUnitChannel->Send(ALUMessage::Execute);
        else if (message == ALUMessage::ReadyToFetch) // LD B, A does not perform write back.
            testPassed = true;
    });

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUControlUnitChannel->Bind(controlUnitChannel);
    
    // First trigger to ALU. Subsequent changes are handled in the CU-ALU channel
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x47)));
    controlUnitChannel->Send(ALUMessage::Fetch);

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(alu->GetRegisterBank()->Read(Register::A), alu->GetRegisterBank()->Read(Register::B));
    EXPECT_EQ(ALUState::Executing, alu->GetState());
    EXPECT_TRUE(testPassed);    
}


TEST(TestArithmeticLogicUnit, TestExecutingUnknownInstructions)
{
    auto testPassed = false;
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);
    auto controlUnitChannel = make_shared<Channel<ALUMessage>>();

    controlUnitChannel->OnReceived([&controlUnitChannel](ALUMessage message) -> void 
    {
        if (message == ALUMessage::ReadyToDecode)
            controlUnitChannel->Send(ALUMessage::Decode);
        else if (message == ALUMessage::ReadyToExecute)
            controlUnitChannel->Send(ALUMessage::Execute);
    });

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUControlUnitChannel->Bind(controlUnitChannel);
    
    try
    {
        auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
        EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
        controlUnitChannel->Send(ALUMessage::Fetch);
    }
    catch (const ArithmeticLogicUnitException& e)
    {
        testPassed = true;
    }

    EXPECT_TRUE(testPassed);    
}

TEST(TestArithmeticLogicUnit, TestAcquireSingleImmediateOperand)
{
    auto testPassed = false;
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);
    auto controlUnitChannel = make_shared<Channel<ALUMessage>>();

    controlUnitChannel->OnReceived([&testPassed, &controlUnitChannel](ALUMessage message) -> void 
    {
        if (message == ALUMessage::ReadyToDecode)
            controlUnitChannel->Send(ALUMessage::Decode);
        else if(message == ALUMessage::ReadyToAcquire)
            controlUnitChannel->Send(ALUMessage::Acquire);
        else if (message == ALUMessage::ReadyToExecute)
            controlUnitChannel->Send(ALUMessage::Execute);
        else if (message == ALUMessage::ReadyToFetch) // LD B, A does not perform write back.
            testPassed = true;
    });

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUControlUnitChannel->Bind(controlUnitChannel);
    
    // First trigger to ALU. Subsequent changes are handled in the CU-ALU channel
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be 0x26 0xAA which translates to LD H, 0xAA (load H immediate with 0xAA)
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x26)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xAA)));
    controlUnitChannel->Send(ALUMessage::Fetch);

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0xAA, alu->GetRegisterBank()->Read(Register::H));
    EXPECT_EQ(ALUState::Executing, alu->GetState());
    EXPECT_TRUE(testPassed);    
}

TEST(TestArithmeticLogicUnit, TestAcquireSingleRegisterIndirectOperand)
{
    auto testPassed = false;
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);
    auto controlUnitChannel = make_shared<Channel<ALUMessage>>();

    // Initialize HL
    alu->GetRegisterBank()->WritePair(Register::HL, 0xAABB);

    controlUnitChannel->OnReceived([&testPassed, &controlUnitChannel](ALUMessage message) -> void 
    {
        if (message == ALUMessage::ReadyToDecode)
            controlUnitChannel->Send(ALUMessage::Decode);
        else if(message == ALUMessage::ReadyToAcquire)
            controlUnitChannel->Send(ALUMessage::Acquire);
        else if (message == ALUMessage::ReadyToExecute)
            controlUnitChannel->Send(ALUMessage::Execute);
        else if (message == ALUMessage::ReadyToFetch) // LD A, (HL) does not perform write back.
            testPassed = true;
    });

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUControlUnitChannel->Bind(controlUnitChannel);
    
    // First trigger to ALU. Subsequent changes are handled in the CU-ALU channel
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be OPCODE: 0x7E  (HL): 0xCC where HL holds address 0xAABB
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    EXPECT_CALL((*mockPointer), Read(0xAABB, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xCC)));
    controlUnitChannel->Send(ALUMessage::Fetch);

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0xCC, alu->GetRegisterBank()->Read(Register::A));
    EXPECT_EQ(ALUState::Executing, alu->GetState());
    EXPECT_TRUE(testPassed);    
}

TEST(TestArithmeticLogicUnit, TestAcquireSingleRegisterIndirectBCOperand)
{
    auto testPassed = false;
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);
    auto controlUnitChannel = make_shared<Channel<ALUMessage>>();

    // Initialize HL
    alu->GetRegisterBank()->WritePair(Register::BC, 0x1234);

    controlUnitChannel->OnReceived([&testPassed, &controlUnitChannel](ALUMessage message) -> void 
    {
        if (message == ALUMessage::ReadyToDecode)
            controlUnitChannel->Send(ALUMessage::Decode);
        else if(message == ALUMessage::ReadyToAcquire)
            controlUnitChannel->Send(ALUMessage::Acquire);
        else if (message == ALUMessage::ReadyToExecute)
            controlUnitChannel->Send(ALUMessage::Execute);
        else if (message == ALUMessage::ReadyToFetch) // LD A, (BC) does not perform write back.
            testPassed = true;
    });

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUControlUnitChannel->Bind(controlUnitChannel);

    
    // First trigger to ALU. Subsequent changes are handled in the CU-ALU channel
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be OPCODE: 0x0A  (BC): 0xDD where BC holds address 0x1234
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0A)));
    EXPECT_CALL((*mockPointer), Read(0x1234, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xDD)));
    controlUnitChannel->Send(ALUMessage::Fetch);

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0xDD, alu->GetRegisterBank()->Read(Register::A));
    EXPECT_EQ(ALUState::Executing, alu->GetState());
    EXPECT_TRUE(testPassed);    
}

TEST(TestArithmeticLogicUnit, TestAcquireSingleRegisterIndirectDEOperand)
{
    auto testPassed = false;
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);
    auto controlUnitChannel = make_shared<Channel<ALUMessage>>();

    // Initialize HL
    alu->GetRegisterBank()->WritePair(Register::DE, 0x6789);

    controlUnitChannel->OnReceived([&testPassed, &controlUnitChannel](ALUMessage message) -> void 
    {
        if (message == ALUMessage::ReadyToDecode)
            controlUnitChannel->Send(ALUMessage::Decode);
        else if(message == ALUMessage::ReadyToAcquire)
            controlUnitChannel->Send(ALUMessage::Acquire);
        else if (message == ALUMessage::ReadyToExecute)
            controlUnitChannel->Send(ALUMessage::Execute);
        else if (message == ALUMessage::ReadyToFetch) // LD A, (DE) does not perform write back.
            testPassed = true;
    });

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUControlUnitChannel->Bind(controlUnitChannel);
    
     // First trigger to ALU. Subsequent changes are handled in the CU-ALU channel
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be OPCODE: 0x0A  (BC): 0xDD where BC holds address 0x1234
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x1A)));
    EXPECT_CALL((*mockPointer), Read(0x6789, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x22)));
    controlUnitChannel->Send(ALUMessage::Fetch);

    EXPECT_EQ(0x22, alu->GetRegisterBank()->Read(Register::A));
    EXPECT_EQ(ALUState::Executing, alu->GetState());
    EXPECT_TRUE(testPassed);    
}

TEST(TestArithmeticLogicUnit, TestAcquireSingleRegisterIndirectDestinationHL)
{
    auto testPassed = false;
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    auto controlUnitChannel = make_shared<Channel<ALUMessage>>();

    // Initialize HL
    alu->GetRegisterBank()->WritePair(Register::HL, 0xAABB);
    alu->GetRegisterBank()->Write(Register::A, 0xCC);

    controlUnitChannel->OnReceived([&testPassed, &controlUnitChannel](ALUMessage message) -> void 
    {
        if (message == ALUMessage::ReadyToDecode)
            controlUnitChannel->Send(ALUMessage::Decode);
        else if (message == ALUMessage::ReadyToExecute)
            controlUnitChannel->Send(ALUMessage::Execute);
        else if (message == ALUMessage::ReadyToWriteBack)
            controlUnitChannel->Send(ALUMessage::WriteBack);
        else if (message == ALUMessage::ReadyToFetch)
            testPassed = true;
    });

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUControlUnitChannel->Bind(controlUnitChannel);
    
    // First trigger to ALU. Subsequent changes are handled in the CU-ALU channel
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // Write A (0xCC) to the location pointed by HL (0xAABB);
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x77)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xCC)), static_cast<uint16_t>(0xAABB)));
    controlUnitChannel->Send(ALUMessage::Fetch);

    EXPECT_EQ(ALUState::WritingBack, alu->GetState());
    EXPECT_TRUE(testPassed);    
}

TEST(TestArithmeticLogicUnit, TestAcquireSingleRegisterIndirectDestinationDE)
{
    auto testPassed = false;
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    auto controlUnitChannel = make_shared<Channel<ALUMessage>>();

    // Initialize HL
    alu->GetRegisterBank()->WritePair(Register::DE, 0x1234);
    alu->GetRegisterBank()->Write(Register::A, 0x99);

    controlUnitChannel->OnReceived([&testPassed, &controlUnitChannel](ALUMessage message) -> void 
    {
        if (message == ALUMessage::ReadyToDecode)
            controlUnitChannel->Send(ALUMessage::Decode);
        else if (message == ALUMessage::ReadyToExecute)
            controlUnitChannel->Send(ALUMessage::Execute);
        else if (message == ALUMessage::ReadyToWriteBack)
            controlUnitChannel->Send(ALUMessage::WriteBack);
        else if (message == ALUMessage::ReadyToFetch)
            testPassed = true;
    });

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUControlUnitChannel->Bind(controlUnitChannel);

    // First trigger to ALU. Subsequent changes are handled in the CU-ALU channel
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be OPCODE: 0x77  write A to (HL) (A value will be 0xEE, HL will point to 0xAABB)
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x12)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x99)), static_cast<uint16_t>(0x1234)));
    controlUnitChannel->Send(ALUMessage::Fetch);

    EXPECT_EQ(ALUState::WritingBack, alu->GetState());
    EXPECT_TRUE(testPassed);    
}

TEST(TestArithmeticLogicUnit, TestAcquireSingleRegisterIndirectDestinationBC)
{
    auto testPassed = false;
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    auto controlUnitChannel = make_shared<Channel<ALUMessage>>();

    // Initialize HL
    alu->GetRegisterBank()->WritePair(Register::BC, 0x9967);
    alu->GetRegisterBank()->Write(Register::A, 0x12);

    controlUnitChannel->OnReceived([&testPassed, &controlUnitChannel](ALUMessage message) -> void 
    {
        if (message == ALUMessage::ReadyToDecode)
            controlUnitChannel->Send(ALUMessage::Decode);
        else if (message == ALUMessage::ReadyToExecute)
            controlUnitChannel->Send(ALUMessage::Execute);
        else if (message == ALUMessage::ReadyToWriteBack)
            controlUnitChannel->Send(ALUMessage::WriteBack);
        else if (message == ALUMessage::ReadyToFetch)
            testPassed = true;
    });

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUControlUnitChannel->Bind(controlUnitChannel);
    
    // First trigger to ALU. Subsequent changes are handled in the CU-ALU channel
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // The returned operation bytes will be OPCODE: 0x77  write A to (HL) (A value will be 0xEE, HL will point to 0xAABB)
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x02)));
    EXPECT_CALL((*mockPointer), Write(std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0x12)), static_cast<uint16_t>(0x9967)));
    controlUnitChannel->Send(ALUMessage::Fetch);

    EXPECT_EQ(ALUState::WritingBack, alu->GetState());
    EXPECT_TRUE(testPassed);    
}

TEST(TestArithmeticLogicUnit, TestIndexedSourceOpcodeIX)
{
    auto testPassed = false;
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    auto controlUnitChannel = make_shared<Channel<ALUMessage>>();

    controlUnitChannel->OnReceived([&testPassed, &controlUnitChannel](ALUMessage message) -> void 
    {
        if(message == ALUMessage::ReadyToReadRealOpcode)
            controlUnitChannel->Send(ALUMessage::FetchOpcode);
        else if (message == ALUMessage::ReadyToDecode)
            controlUnitChannel->Send(ALUMessage::Decode);
        else if(message == ALUMessage::ReadyToAcquire)
            controlUnitChannel->Send(ALUMessage::Acquire);
        else if(message == ALUMessage::ReadyToAcquire)
            controlUnitChannel->Send(ALUMessage::Acquire);
        else if (message == ALUMessage::ReadyToExecute)
            controlUnitChannel->Send(ALUMessage::Execute);
        else if (message == ALUMessage::ReadyToFetch) // LD A, (IX + d) does not perform write back.
            testPassed = true;
    });

    alu->GetRegisterBank()->WritePair(Register::IX, 0x0100);

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUControlUnitChannel->Bind(controlUnitChannel);
    
    // First trigger to ALU. Subsequent changes are handled in the CU-ALU channel
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // IX will have value 0x0100 so the final address will be 0x01AA, which will hold value 0x11
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xDD)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x0A)));
    EXPECT_CALL((*mockPointer), Read(0x010A, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x11)));
    controlUnitChannel->Send(ALUMessage::Fetch);

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0x11, alu->GetRegisterBank()->Read(Register::A));
    EXPECT_EQ(ALUState::Executing, alu->GetState());
    EXPECT_TRUE(testPassed);    
}

TEST(TestArithmeticLogicUnit, TestIndexedSourceOpcodeIY)
{
    auto testPassed = false;
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    auto controlUnitChannel = make_shared<Channel<ALUMessage>>();

    controlUnitChannel->OnReceived([&testPassed, &controlUnitChannel](ALUMessage message) -> void 
    {
        if(message == ALUMessage::ReadyToReadRealOpcode)
            controlUnitChannel->Send(ALUMessage::FetchOpcode);
        else if (message == ALUMessage::ReadyToDecode)
            controlUnitChannel->Send(ALUMessage::Decode);
        else if(message == ALUMessage::ReadyToAcquire)
            controlUnitChannel->Send(ALUMessage::Acquire);
        else if(message == ALUMessage::ReadyToAcquire)
            controlUnitChannel->Send(ALUMessage::Acquire);
        else if (message == ALUMessage::ReadyToExecute)
            controlUnitChannel->Send(ALUMessage::Execute);
        else if (message == ALUMessage::ReadyToFetch) // LD A, (IX + d) does not perform write back.
            testPassed = true;
    });

    alu->GetRegisterBank()->WritePair(Register::IY, 0x0A00);

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUControlUnitChannel->Bind(controlUnitChannel);
    
    // First trigger to ALU. Subsequent changes are handled in the CU-ALU channel
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // IY will have value 0x0A00 so the final address will be 0x0A09, which will hold value 0xCA
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFD)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x09)));
    EXPECT_CALL((*mockPointer), Read(0x0A09, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xCA)));
    controlUnitChannel->Send(ALUMessage::Fetch);

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0xCA, alu->GetRegisterBank()->Read(Register::A));
    EXPECT_EQ(ALUState::Executing, alu->GetState());
    EXPECT_TRUE(testPassed);    
}

TEST(TestArithmeticLogicUnit, TestIndexedSourceOpcodeIXNegativeDisplacement)
{
    auto testPassed = false;
    shared_ptr<MemoryControllerInterface> memoryController = make_shared<MemoryControllerMock>();
    auto alu = make_shared<ALUWrapperForTests>();
         alu->Initialize(memoryController);

    auto controlUnitChannel = make_shared<Channel<ALUMessage>>();

    controlUnitChannel->OnReceived([&testPassed, &controlUnitChannel](ALUMessage message) -> void 
    {
        if(message == ALUMessage::ReadyToReadRealOpcode)
            controlUnitChannel->Send(ALUMessage::FetchOpcode);
        else if (message == ALUMessage::ReadyToDecode)
            controlUnitChannel->Send(ALUMessage::Decode);
        else if(message == ALUMessage::ReadyToAcquire)
            controlUnitChannel->Send(ALUMessage::Acquire);
        else if(message == ALUMessage::ReadyToAcquire)
            controlUnitChannel->Send(ALUMessage::Acquire);
        else if (message == ALUMessage::ReadyToExecute)
            controlUnitChannel->Send(ALUMessage::Execute);
        else if (message == ALUMessage::ReadyToFetch) // LD A, (IX + d) does not perform write back.
            testPassed = true;
    });

    alu->GetRegisterBank()->WritePair(Register::IY, 0x0200);

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUControlUnitChannel->Bind(controlUnitChannel);

    // First trigger to ALU. Subsequent changes are handled in the CU-ALU channel
    auto mockPointer = static_pointer_cast<MemoryControllerMock>(memoryController);
    // IY will have value 0x0200 so the final address will be 0x01FF, which will hold value 0x10
    EXPECT_CALL((*mockPointer), Read(0x0000, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFD)));
    EXPECT_CALL((*mockPointer), Read(0x0001, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x7E)));
    EXPECT_CALL((*mockPointer), Read(0x0002, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0xFF)));
    EXPECT_CALL((*mockPointer), Read(0x01FF, MemoryAccessType::Byte)).WillOnce(Return(static_cast<uint8_t>(0x10)));
    controlUnitChannel->Send(ALUMessage::Fetch);

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0x10, alu->GetRegisterBank()->Read(Register::A));
    EXPECT_EQ(ALUState::Executing, alu->GetState());
    EXPECT_TRUE(testPassed);    
}