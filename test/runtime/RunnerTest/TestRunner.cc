#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "RuntimeTestMocks.h"

#include <chrono>
#include <iostream>
#include <limits>
#include <memory>
#include <thread> 
#include <variant> 

#include "ServerTransport.h"
#include "CancellationToken.h"
#include "Runner.h"
#include "Runtime.h"
#include "RegisterBankInterface.h"

using namespace std;
using namespace std::chrono_literals;
using namespace gbxruntime::runner;
using namespace gbxdb;
using namespace gbxdb::interfaces;
using namespace gbxruntime::runner;
using namespace gbxcore::interfaces;

using ::testing::Return;
using ::testing::_;

TEST(RuntimeTests_Runner, Construction) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto runner = make_shared<Runner>(pointer);
    
    EXPECT_EQ(RunnerMode::Runtime, runner->Mode());
}

TEST(RuntimeTests_Runner, ConstructionInDebugMode) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto runtimePointer = static_pointer_cast<Runtime>(runtime);
    auto transport = make_unique<ServerTransportMock>();
    auto runner = make_shared<Runner>(runtimePointer, std::move(transport));

    EXPECT_EQ(RunnerMode::Debug, runner->Mode());
}

TEST(RuntimeTests_Runner, RunForANumberOfCycles) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto runner = make_shared<Runner>(pointer);

    EXPECT_CALL((*runtime), Run()).Times(100);
    CancellationToken token;
    runner->Run(100, token);
    
    EXPECT_CALL((*runtime), Run()).Times(100);
    runner->Run(100, token);

    EXPECT_EQ(RunnerMode::Runtime, runner->Mode());
}

TEST(RuntimeTests_Runner, CancellationAWithPredefinedNumberOfCycles) 
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
    EXPECT_EQ(RunnerMode::Runtime, runner->Mode());
}

TEST(RuntimeTests_Runner, RunIndefinitelyWithCancellationToken) 
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
    EXPECT_EQ(RunnerMode::Runtime, runner->Mode());
}

TEST(RuntimeTests_Runner, RunInDebugMode) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto transport = make_unique<ServerTransportMock>();
    auto transportPointer = transport.get();
    auto runner = make_shared<Runner>(pointer, std::move(transport));

    CancellationToken token;
    std::thread cancellationThread([&]()
    { 
        std::this_thread::sleep_for(100ms);
        runner->ClientJoined();
        std::this_thread::sleep_for(100ms);
        token.Cancel();
    });

    EXPECT_CALL((*transportPointer), WaitForClient());   
    EXPECT_CALL((*transportPointer), Subscribe(::_));
    EXPECT_CALL((*transportPointer), InitializeProtocol()).Times(1);
    EXPECT_CALL((*runtime), Run()).WillRepeatedly(Return());
    runner->Run(token);

    cancellationThread.join();
    EXPECT_TRUE(token.IsCancelled());
    EXPECT_EQ(RunnerMode::Debug, runner->Mode());
}

TEST(RuntimeTests_Runner, RunInDebugModeForAGivenNumberOfCycles) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto transport = make_unique<ServerTransportMock>();
    auto transportPointer = transport.get();
    auto runner = make_shared<Runner>(pointer, std::move(transport));

    CancellationToken token;
    std::thread cancellationThread([&]()
    { 
        std::this_thread::sleep_for(100ms);
        runner->ClientJoined();
        std::this_thread::sleep_for(100ms);
        token.Cancel();
    });

    EXPECT_CALL((*transportPointer), WaitForClient());
    EXPECT_CALL((*transportPointer), Subscribe(::_));
    EXPECT_CALL((*transportPointer), InitializeProtocol()).Times(1);
    EXPECT_CALL((*runtime), Run()).WillRepeatedly(Return());
    runner->Run(numeric_limits<size_t>::max(), token);
    
    cancellationThread.join();
    EXPECT_TRUE(token.IsCancelled());
    EXPECT_EQ(RunnerMode::Debug, runner->Mode());
}