#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <chrono>
#include <iostream>
#include <limits>
#include <memory>
#include <thread> 
#include <variant> 

#include "../src/CancellationToken.h"
#include "../src/Runner.h"
#include "interfaces/Runtime.h"
#include "interfaces/RegisterBankInterface.h"

using namespace std;
using namespace std::chrono_literals;
using namespace gbx;
using namespace gbx::interfaces;
using namespace gbxcore::interfaces;

using ::testing::Return;
using ::testing::_;

class RuntimeMock : public Runtime
{
public:
    virtual ~RuntimeMock() = default;
    MOCK_METHOD(void, Run, ());
    MOCK_METHOD((std::variant<uint8_t, uint16_t>), ReadRegister, (Register));
    MOCK_METHOD(void, WriteRegister, (Register, (std::variant<uint8_t, uint16_t>)));
};

TEST(TestRunner, Construction) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto runner = make_shared<Runner>(pointer);
}

TEST(TestRunner, RunForANumberOfCycles) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto runner = make_shared<Runner>(pointer);

    EXPECT_CALL((*runtime), Run()).Times(100);
    CancellationToken token;
    runner->Run(100, token);
    
    EXPECT_CALL((*runtime), Run()).Times(100);
    runner->RunWithDebugSupport(100, token);
}

TEST(TestRunner, CancellationAWithPredefinedNumberOfCycles) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto runner = make_shared<Runner>(pointer);

    CancellationToken token;
    std::thread cancellationThread([&]()
    { 
        std::this_thread::sleep_for(500ms);
        token.Cancel();
    });

    EXPECT_CALL((*runtime), Run()).WillRepeatedly(Return());
    runner->Run(numeric_limits<size_t>::max(), token);
    
    cancellationThread.join();
    EXPECT_TRUE(token.IsCancelled());
}

TEST(TestRunner, RunIndefinitelyWithCancellationToken) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto runner = make_shared<Runner>(pointer);

    CancellationToken token;
    std::thread cancellationThread([&]()
    { 
        std::this_thread::sleep_for(500ms);
        token.Cancel();
    });

    EXPECT_CALL((*runtime), Run()).WillRepeatedly(Return());
    runner->Run(token);

    cancellationThread.join();
    EXPECT_TRUE(token.IsCancelled());
}

TEST(TestRunner, RunInDebugMode) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto runner = make_shared<Runner>(pointer);

    CancellationToken token;
    std::thread cancellationThread([&]()
    { 
        std::this_thread::sleep_for(500ms);
        token.Cancel();
    });

    EXPECT_CALL((*runtime), Run()).WillRepeatedly(Return());
    runner->RunWithDebugSupport(token);
    
    cancellationThread.join();
    EXPECT_TRUE(token.IsCancelled());
}

TEST(TestRunner, RunInDebugModeForAGivenNumberOfCycles) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto runner = make_shared<Runner>(pointer);

    CancellationToken token;
    std::thread cancellationThread([&]()
    { 
        std::this_thread::sleep_for(500ms);
        token.Cancel();
    });

    EXPECT_CALL((*runtime), Run()).WillRepeatedly(Return());
    runner->RunWithDebugSupport(numeric_limits<size_t>::max(), token);
    
    cancellationThread.join();
    EXPECT_TRUE(token.IsCancelled());
}

TEST(TestRunner, RequestReadRegister)
{
    auto runTime = make_shared<RuntimeMock>();
 
    EXPECT_CALL((*runTime), ReadRegister(Register::A)).WillOnce(Return(static_cast<uint8_t>(0xFE)));
    auto registerValue = runTime->ReadRegister(Register::A);

    EXPECT_CALL((*runTime), ReadRegister(Register::HL)).WillOnce(Return(static_cast<uint16_t>(0xFEEF)));
    auto registerPairValue = runTime->ReadRegister(Register::HL);

    EXPECT_EQ(0xFE, get<uint8_t>(registerValue));
    EXPECT_EQ(0xFEEF, get<uint16_t>(registerPairValue));
}

TEST(TestRunner, RequestWriteRegister)
{
    auto runTime = make_shared<RuntimeMock>();
 
    auto expectationVariant = std::variant<uint8_t, uint16_t>(static_cast<uint8_t>(0xFE));
    EXPECT_CALL((*runTime), WriteRegister(Register::A, expectationVariant));
    runTime->WriteRegister(Register::A, static_cast<uint8_t>(0xFE));

    auto expectationPairVariant = std::variant<uint8_t, uint16_t>(static_cast<uint16_t>(0xFEEF));
    EXPECT_CALL((*runTime), WriteRegister(Register::HL, expectationPairVariant));
    runTime->WriteRegister(Register::HL, static_cast<uint16_t>(0xFEEF));
}