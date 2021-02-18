#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <thread> 

#include "../src/CancellationToken.h"
#include "../src/Runner.h"
#include "interfaces/Runtime.h"

using namespace std;
using namespace std::chrono_literals;
using namespace gbx;
using namespace gbx::interfaces;
using ::testing::Return;

class RuntimeMock : public Runtime
{
public:
    virtual ~RuntimeMock() = default;
    MOCK_METHOD(void, Run, ());
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
    runner->Run(100);
}

TEST(TestRunner, RunIndefinitelyWithCancellationToken) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto runner = make_shared<Runner>(pointer);

    CancellationToken token;
    std::thread cancellationThread([&]()
    { 
        std::this_thread::sleep_for(1000ms);
        token.Cancel();
    });

    EXPECT_CALL((*runtime), Run()).WillRepeatedly(Return());
    runner->Run(token);

    cancellationThread.join();
    EXPECT_TRUE(token.IsCancelled());
}
