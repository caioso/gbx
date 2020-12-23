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
    RegisterBank& GetRegisterBank()
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
    auto alu = make_shared<ALUWrapperForTests>();
    auto controlUnitChannel = make_shared<Channel<ALUMessage>>(ChannelType::InOut);
    auto MemoryControllerALUChannel = make_shared<Channel<MemoryMessage>>(ChannelType::InOut);

    controlUnitChannel->OnReceived( [](ALUMessage) -> void {return;} );
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
    controlUnitChannel->Send(ALUMessage::FetchPC);

    auto instructionRegister = alu->GetRegisterBank().Read(Register::IR);
    auto programCounter = alu->GetRegisterBank().ReadPair(Register::PC);

    EXPECT_EQ(0xAA, instructionRegister);
    EXPECT_EQ(0x0001, programCounter);
}

TEST(TestArithmeticLogicUnit, TestDecoding)
{
    auto testPassed = false;
    auto alu = make_shared<ALUWrapperForTests>();
    auto controlUnitChannel = make_shared<Channel<ALUMessage>>(ChannelType::InOut);

    controlUnitChannel->OnReceived( [&testPassed](ALUMessage message) -> void 
    {
        // For simple instructions
        if (message == ALUMessage::Complete)
            testPassed = true;
    });
    
    controlUnitChannel->Bind(alu->ALUControlUnitChannel);

    // Idle -> *FetchingPC* -> Decoding
    alu->SetStateForTest(ALUState::FetchingPC);
    controlUnitChannel->Send(ALUMessage::Decode);

    EXPECT_TRUE(testPassed);
}