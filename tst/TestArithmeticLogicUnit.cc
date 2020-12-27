#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cmath>
#include <thread>
#include <variant>

#include <memory>

#include "../src/ArithmeticLogicUnit.h"
#include "../src/Channel.h"
#include "../src/MemoryController.h"
#include "../src/RegisterBank.h"

using namespace std;
using namespace gbx;

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

TEST(TestArithmeticLogicUnit, Construction) 
{
    auto alu = make_shared<ALUWrapperForTests>();
    EXPECT_EQ(ChannelType::InOut, alu->ALUControlUnitChannel->Type());
    EXPECT_EQ(ALUState::Idle, alu->GetState());
}

TEST(TestArithmeticLogicUnit, FetchPCMessage)
{
    auto testPassed = false;
    auto alu = make_shared<ALUWrapperForTests>();
    auto controlUnitChannel = make_shared<Channel<ALUMessage>>(ChannelType::InOut);
    auto MemoryControllerALUChannel = make_shared<Channel<MemoryMessage>>(ChannelType::InOut);

    controlUnitChannel->OnReceived([&testPassed](ALUMessage message) -> void 
    {
        if (message == ALUMessage::ReadyToDecode)
            testPassed = true;
        return;
    });

    MemoryControllerALUChannel->OnReceived( [&MemoryControllerALUChannel](MemoryMessage message) -> void 
    {
        if (message.Request == MemoryRequestType::Read && get<uint16_t>(message.Data) == 0x0000)
        {
            MemoryControllerALUChannel->Send({MemoryRequestType::Result, static_cast<uint8_t>(0xAA), MemoryAccessType::Byte});
        }
        return;
    });

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUMemoryControllerChannel->Bind(MemoryControllerALUChannel);
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
    auto alu = make_shared<ALUWrapperForTests>();
    auto controlUnitChannel = make_shared<Channel<ALUMessage>>(ChannelType::InOut);
    auto MemoryControllerALUChannel = make_shared<Channel<MemoryMessage>>(ChannelType::InOut);

    controlUnitChannel->OnReceived([&testPassed, &controlUnitChannel](ALUMessage message) -> void 
    {
        if (message == ALUMessage::ReadyToDecode)
            controlUnitChannel->Send(ALUMessage::Decode);
        else if (message == ALUMessage::ReadyToExecute)
            testPassed = true;
    });

    MemoryControllerALUChannel->OnReceived( [&MemoryControllerALUChannel](MemoryMessage message) -> void 
    {
        if (message.Request == MemoryRequestType::Read && get<uint16_t>(message.Data) == 0x0000)
            MemoryControllerALUChannel->Send({MemoryRequestType::Result, static_cast<uint8_t>(0x47), MemoryAccessType::Byte});
    });

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUMemoryControllerChannel->Bind(MemoryControllerALUChannel);
    
    // First trigger to ALU. Subsequent changes are handled in the CU-ALU channel
    controlUnitChannel->Send(ALUMessage::Fetch);

    EXPECT_EQ(ALUState::Decoding, alu->GetState());
    EXPECT_TRUE(testPassed);    
}

TEST(TestArithmeticLogicUnit, TestExecuting)
{
    auto testPassed = false;
    auto alu = make_shared<ALUWrapperForTests>();
    auto controlUnitChannel = make_shared<Channel<ALUMessage>>(ChannelType::InOut);
    auto MemoryControllerALUChannel = make_shared<Channel<MemoryMessage>>(ChannelType::InOut);

    controlUnitChannel->OnReceived([&testPassed, &controlUnitChannel](ALUMessage message) -> void 
    {
        if (message == ALUMessage::ReadyToDecode)
            controlUnitChannel->Send(ALUMessage::Decode);
        else if (message == ALUMessage::ReadyToExecute)
            controlUnitChannel->Send(ALUMessage::Execute);
        else if (message == ALUMessage::ReadyToFetch) // LD B, A does not perform write back.
            testPassed = true;
    });

    MemoryControllerALUChannel->OnReceived( [&MemoryControllerALUChannel](MemoryMessage message) -> void 
    {
        if (message.Request == MemoryRequestType::Read && get<uint16_t>(message.Data) == 0x0000)
            MemoryControllerALUChannel->Send({MemoryRequestType::Result, static_cast<uint8_t>(0x47), MemoryAccessType::Byte});
    });

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUMemoryControllerChannel->Bind(MemoryControllerALUChannel);
    
    // First trigger to ALU. Subsequent changes are handled in the CU-ALU channel
    controlUnitChannel->Send(ALUMessage::Fetch);

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(alu->GetRegisterBank()->Read(Register::A), alu->GetRegisterBank()->Read(Register::B));
    EXPECT_EQ(ALUState::Executing, alu->GetState());
    EXPECT_TRUE(testPassed);    
}


TEST(TestArithmeticLogicUnit, TestExecutingUnknownInstructions)
{
    auto testPassed = false;
    auto alu = make_shared<ALUWrapperForTests>();
    auto controlUnitChannel = make_shared<Channel<ALUMessage>>(ChannelType::InOut);
    auto MemoryControllerALUChannel = make_shared<Channel<MemoryMessage>>(ChannelType::InOut);

    controlUnitChannel->OnReceived([&controlUnitChannel](ALUMessage message) -> void 
    {
        if (message == ALUMessage::ReadyToDecode)
            controlUnitChannel->Send(ALUMessage::Decode);
        else if (message == ALUMessage::ReadyToExecute)
            controlUnitChannel->Send(ALUMessage::Execute);
    });

    MemoryControllerALUChannel->OnReceived( [&MemoryControllerALUChannel](MemoryMessage message) -> void 
    {
        if (message.Request == MemoryRequestType::Read && get<uint16_t>(message.Data) == 0x0000)
            MemoryControllerALUChannel->Send({MemoryRequestType::Result, static_cast<uint8_t>(0xFF), MemoryAccessType::Byte});
    });

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUMemoryControllerChannel->Bind(MemoryControllerALUChannel);
    
    try
    {
        controlUnitChannel->Send(ALUMessage::Fetch);
    }
    catch (const ArithmeticLogicUnitException& e)
    {
        testPassed = true;
    }

    EXPECT_TRUE(testPassed);    
}


TEST(TestArithmeticLogicUnit, TestAcquireSingleOperand)
{
    auto testPassed = false;
    auto alu = make_shared<ALUWrapperForTests>();
    auto controlUnitChannel = make_shared<Channel<ALUMessage>>(ChannelType::InOut);
    auto MemoryControllerALUChannel = make_shared<Channel<MemoryMessage>>(ChannelType::InOut);

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

    // The returned operation bytes will be 0x26 0xAA which translates to LD H, 0xAA (load H immediate with 0xAA)
    MemoryControllerALUChannel->OnReceived( [&MemoryControllerALUChannel](MemoryMessage message) -> void 
    {
        if (message.Request == MemoryRequestType::Read && get<uint16_t>(message.Data) == 0x0000)
            MemoryControllerALUChannel->Send({MemoryRequestType::Result, static_cast<uint8_t>(0x26), MemoryAccessType::Byte});
        else if (message.Request == MemoryRequestType::Read && get<uint16_t>(message.Data) == 0x0001)
            MemoryControllerALUChannel->Send({MemoryRequestType::Result, static_cast<uint8_t>(0xAA), MemoryAccessType::Byte});
    });

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUMemoryControllerChannel->Bind(MemoryControllerALUChannel);
    
    // First trigger to ALU. Subsequent changes are handled in the CU-ALU channel
    controlUnitChannel->Send(ALUMessage::Fetch);

    // Check whether the instruction has been properlye executed (A == B)
    EXPECT_EQ(0xAA, alu->GetRegisterBank()->Read(Register::H));
    EXPECT_EQ(ALUState::Executing, alu->GetState());
    EXPECT_TRUE(testPassed);    
}


