#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cmath>
#include <thread>
#include <variant>

#include <memory>

#include "../src/ArithmeticLogicUnit.h"
#include "../src/Channel.h"
#include "../src/RegisterBank.h"
#include "../src/ROM.h"

using namespace std;
using namespace gbx;

class ALUWrapperForTests : public ArithmeticLogicUnit
{
public:
    RegisterBank& GetRegisterBank()
    {
        return this->_registers;
    }
};

TEST(TestArithmeticLogicUnit, Construction) 
{
    auto alu = make_shared<ArithmeticLogicUnit>();
    EXPECT_EQ(ChannelType::InOut, alu->ALUControlUnitChannel->Type());
}

TEST(TestArithmeticLogicUnit, FetchPCMessage)
{
    auto alu = make_shared<ALUWrapperForTests>();
    auto controlUnitChannel = make_shared<Channel<ALUMessage>>(ChannelType::InOut);
    auto ROMALUChannel = make_shared<Channel<ROMMessage>>(ChannelType::InOut);

    controlUnitChannel->OnReceived( [](ALUMessage) -> void {return;} );
    ROMALUChannel->OnReceived( [&ROMALUChannel](ROMMessage message) -> void 
    {
        if (message.Request == ROMRequestType::Read && get<uint16_t>(message.Data) == 0x0000)
        {
            ROMALUChannel->Send({ROMRequestType::Result, static_cast<uint8_t>(0xAA)});
        }
        return;
    });

    controlUnitChannel->Bind(alu->ALUControlUnitChannel);
    alu->ALUROMChannel->Bind(ROMALUChannel);
    controlUnitChannel->Send(ALUMessage::FetchPC);

    auto instructionRegister = alu->GetRegisterBank().Read(Register::IR);
    auto programCounter = alu->GetRegisterBank().ReadPair(Register::PC);

    EXPECT_EQ(0xAA, instructionRegister);
    EXPECT_EQ(0x0001, programCounter);
}