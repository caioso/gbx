#include <gtest/gtest.h>

#include <memory>

#include "../src/ArithmeticLogicUnit.h"
#include "../src/ClockSource.h"
#include "../src/ControlUnit.h"

using namespace std;
using namespace gbx;

class ControlUnitWrapperForTests : public ControlUnit
{
public:
    ClockSource GetClockSource() { return *_clock;}
    ControlUnitState CtrlUnitState() { return _state; }
};

TEST(TestControlUnit, Construction) 
{
    auto controlUnit = make_shared<ControlUnitWrapperForTests>();

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());
}

TEST(TestControlUnit, Initialization) 
{
    auto controlUnit = make_shared<ControlUnitWrapperForTests>();
    controlUnit->Initialize();

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());

}
TEST(TestControlUnit, RequestFetch)
{
    auto testPassed = false;
    auto controlUnit = make_shared<ControlUnitWrapperForTests>();
    auto dummyAluChannel = make_shared<Channel<ALUMessage>>(ChannelType::InOut);

    controlUnit->ControlUnitALUChannel->Bind(dummyAluChannel);
    dummyAluChannel->OnReceived([&testPassed, &dummyAluChannel](ALUMessage message) 
    { 
        if (message == ALUMessage::Fetch)
        {
            dummyAluChannel->Send(ALUMessage::ReadyToDecode);
            testPassed = true;
        }
    });

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());
    controlUnit->RunInstructionCycle();
    EXPECT_EQ(ControlUnitState::Decode, controlUnit->CtrlUnitState());

    EXPECT_TRUE(testPassed);
}

TEST(TestControlUnit, RequestDecode)
{
    auto testPassed = false;
    auto controlUnit = make_shared<ControlUnitWrapperForTests>();
    auto dummyAluChannel = make_shared<Channel<ALUMessage>>(ChannelType::InOut);

    controlUnit->ControlUnitALUChannel->Bind(dummyAluChannel);
    dummyAluChannel->OnReceived([&testPassed, &dummyAluChannel](ALUMessage message) 
    { 
        if (message == ALUMessage::Fetch)
            dummyAluChannel->Send(ALUMessage::ReadyToDecode);
        else if (message == ALUMessage::Decode)
        {
            dummyAluChannel->Send(ALUMessage::ReadyToExecute);
            testPassed = true;
        }
    });

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());
    controlUnit->RunInstructionCycle();
    EXPECT_EQ(ControlUnitState::Decode, controlUnit->CtrlUnitState());
    controlUnit->RunInstructionCycle();
    EXPECT_EQ(ControlUnitState::Execute, controlUnit->CtrlUnitState());

    EXPECT_TRUE(testPassed);
}

TEST(TestControlUnit, RequestExecute)
{
    auto testPassed = false;
    auto controlUnit = make_shared<ControlUnitWrapperForTests>();
    auto dummyAluChannel = make_shared<Channel<ALUMessage>>(ChannelType::InOut);

    controlUnit->ControlUnitALUChannel->Bind(dummyAluChannel);
    dummyAluChannel->OnReceived([&testPassed, &dummyAluChannel](ALUMessage message) 
    { 
        if (message == ALUMessage::Fetch)
            dummyAluChannel->Send(ALUMessage::ReadyToDecode);
        if (message == ALUMessage::Decode)
            dummyAluChannel->Send(ALUMessage::ReadyToExecute);
        else if (message == ALUMessage::Execute)
        {
            dummyAluChannel->Send(ALUMessage::ReadyToWriteBack);
            testPassed = true;
        }
    });

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());
    controlUnit->RunInstructionCycle();
    EXPECT_EQ(ControlUnitState::Decode, controlUnit->CtrlUnitState());
    controlUnit->RunInstructionCycle();
    EXPECT_EQ(ControlUnitState::Execute, controlUnit->CtrlUnitState());
    controlUnit->RunInstructionCycle();
    EXPECT_EQ(ControlUnitState::WriteBack, controlUnit->CtrlUnitState());

    EXPECT_TRUE(testPassed);
}

TEST(TestControlUnit, RequestWriteBack)
{
    auto testPassed = false;
    auto controlUnit = make_shared<ControlUnitWrapperForTests>();
    auto dummyAluChannel = make_shared<Channel<ALUMessage>>(ChannelType::InOut);

    controlUnit->ControlUnitALUChannel->Bind(dummyAluChannel);
    dummyAluChannel->OnReceived([&testPassed, &dummyAluChannel](ALUMessage message) 
    { 
        if (message == ALUMessage::Fetch)
            dummyAluChannel->Send(ALUMessage::ReadyToDecode);
        if (message == ALUMessage::Decode)
            dummyAluChannel->Send(ALUMessage::ReadyToExecute);
        if (message == ALUMessage::Execute)
            dummyAluChannel->Send(ALUMessage::ReadyToWriteBack);
        else if (message == ALUMessage::WriteBack)
        {
            dummyAluChannel->Send(ALUMessage::ReadyToFetch);
            testPassed = true;
        }
    });

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());
    controlUnit->RunInstructionCycle();
    EXPECT_EQ(ControlUnitState::Decode, controlUnit->CtrlUnitState());
    controlUnit->RunInstructionCycle();
    EXPECT_EQ(ControlUnitState::Execute, controlUnit->CtrlUnitState());
    controlUnit->RunInstructionCycle();
    EXPECT_EQ(ControlUnitState::WriteBack, controlUnit->CtrlUnitState());
    controlUnit->RunInstructionCycle();
    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());

    EXPECT_TRUE(testPassed);
}

TEST(TestControlUnit, IntermediateAcquireRequest)
{
    auto messageCounter = 0;
    auto testPassed = false;
    auto controlUnit = make_shared<ControlUnitWrapperForTests>();
    auto dummyAluChannel = make_shared<Channel<ALUMessage>>(ChannelType::InOut);

    controlUnit->ControlUnitALUChannel->Bind(dummyAluChannel);
    dummyAluChannel->OnReceived([&testPassed, &dummyAluChannel, &messageCounter](ALUMessage message) 
    { 
        if (messageCounter == 0 && message == ALUMessage::Fetch)
            dummyAluChannel->Send(ALUMessage::ReadyToDecode);
        if (messageCounter == 1 && message == ALUMessage::Decode)
            dummyAluChannel->Send(ALUMessage::ReadyToAcquire); // Read 8 bits operand from memory
        if (messageCounter == 2 && message == ALUMessage::Acquire)
            dummyAluChannel->Send(ALUMessage::ReadyToAcquire); // Read extra 8 bits operando from memory
        else if (messageCounter == 3 && message == ALUMessage::Acquire)
            dummyAluChannel->Send(ALUMessage::ReadyToExecute);
        else if (messageCounter == 4 && message == ALUMessage::Execute)
            dummyAluChannel->Send(ALUMessage::ReadyToWriteBack);
        else if (messageCounter == 5 && message == ALUMessage::WriteBack)
        {
            dummyAluChannel->Send(ALUMessage::ReadyToFetch);
            testPassed = true;
        }

        messageCounter++;
    });

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());
    controlUnit->RunInstructionCycle();
    EXPECT_EQ(ControlUnitState::Decode, controlUnit->CtrlUnitState());
    controlUnit->RunInstructionCycle();
    EXPECT_EQ(ControlUnitState::Acquire, controlUnit->CtrlUnitState());
    controlUnit->RunInstructionCycle();
    EXPECT_EQ(ControlUnitState::Acquire, controlUnit->CtrlUnitState());
    controlUnit->RunInstructionCycle();
    EXPECT_EQ(ControlUnitState::Execute, controlUnit->CtrlUnitState());
    controlUnit->RunInstructionCycle();
    EXPECT_EQ(ControlUnitState::WriteBack, controlUnit->CtrlUnitState());
    controlUnit->RunInstructionCycle();
    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());

    EXPECT_TRUE(testPassed);
}

TEST(TestControlUnit, RunMultiplePlainCycles)
{
    auto fullCycles = 0;
    auto controlUnit = make_shared<ControlUnitWrapperForTests>();
    auto dummyAluChannel = make_shared<Channel<ALUMessage>>(ChannelType::InOut);

    controlUnit->ControlUnitALUChannel->Bind(dummyAluChannel);
    dummyAluChannel->OnReceived([&dummyAluChannel, &fullCycles](ALUMessage message) 
    { 
        if (message == ALUMessage::Fetch)
            dummyAluChannel->Send(ALUMessage::ReadyToDecode);
        if (message == ALUMessage::Decode)
            dummyAluChannel->Send(ALUMessage::ReadyToExecute);
        if (message == ALUMessage::Execute)
            dummyAluChannel->Send(ALUMessage::ReadyToWriteBack);
        else if (message == ALUMessage::WriteBack)
        {
            fullCycles++;
            dummyAluChannel->Send(ALUMessage::ReadyToFetch);
        }
    });

    for (auto i = 0; i < 100; i++)
        controlUnit->RunInstructionCycle();

    EXPECT_EQ(25, fullCycles);
}
