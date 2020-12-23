#include <gtest/gtest.h>

#include <memory>

#include "../src/ArithmeticLogicUnit.h"
#include "../src/ControlUnit.h"

using namespace std;
using namespace gbx;

class ControlUnitWrapperForTests : public ControlUnit
{
public:
    ControlUnitState CtrlUnitState() { return _state; }
    FetchSubState CtrlFetchSubstate() { return _fetchSubstate; }
};

TEST(TestControlUnit, Construction) 
{
    auto controlUnit = make_shared<ControlUnitWrapperForTests>();

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());
    EXPECT_EQ(FetchSubState::FetchT1, controlUnit->CtrlFetchSubstate());
}

TEST(TestControlUnit, UpdateFetchState) 
{
    auto controlUnit = make_shared<ControlUnitWrapperForTests>();
    auto dummyALUChannel = make_shared<Channel<ALUMessage>>(ChannelType::In);
    dummyALUChannel->OnReceived([](ALUMessage){ return; });

    controlUnit->ControlUnitALUChannel->Bind(dummyALUChannel);
    
    controlUnit->Update();

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());
    EXPECT_EQ(FetchSubState::FetchT2, controlUnit->CtrlFetchSubstate());

    controlUnit->Update();

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());
    EXPECT_EQ(FetchSubState::FetchT3, controlUnit->CtrlFetchSubstate());

    controlUnit->Update();

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());
    EXPECT_EQ(FetchSubState::FetchT4, controlUnit->CtrlFetchSubstate());

    controlUnit->Update();

    EXPECT_EQ(ControlUnitState::Fetch, controlUnit->CtrlUnitState());
    EXPECT_EQ(FetchSubState::FetchT1, controlUnit->CtrlFetchSubstate());
}

TEST(TestControlUnit, RequestPCFetch)
{
    auto testPassed = false;
    auto controlUnit = make_shared<ControlUnitWrapperForTests>();
    auto dummyAluChannel = make_shared<Channel<ALUMessage>>(ChannelType::In);

    controlUnit->ControlUnitALUChannel->Bind(dummyAluChannel);
    dummyAluChannel->OnReceived([&testPassed](ALUMessage message) 
    { 
        if (message == ALUMessage::FetchPC)
            testPassed = true;
    });

    controlUnit->Update();
    EXPECT_TRUE(testPassed);
}

TEST(TestControlUnit, RequestDecoding)
{
    auto testPassed = false;
    auto updateCounter = 0;
    auto controlUnit = make_shared<ControlUnitWrapperForTests>();
    auto dummyAluChannel = make_shared<Channel<ALUMessage>>(ChannelType::In);

    controlUnit->ControlUnitALUChannel->Bind(dummyAluChannel);
    dummyAluChannel->OnReceived([&updateCounter, &testPassed](ALUMessage message) 
    { 
        if (updateCounter == 0)
            EXPECT_TRUE((updateCounter++) == 0 && message == ALUMessage::FetchPC);
        else if (updateCounter == 1)
        {
            EXPECT_TRUE((updateCounter++) == 1 && message == ALUMessage::Decode);
            testPassed = true;
        }
    });

    controlUnit->Update();
    controlUnit->Update();
    controlUnit->Update();
    controlUnit->Update();
    EXPECT_TRUE(testPassed);
}