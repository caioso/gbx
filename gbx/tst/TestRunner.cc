#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <chrono>
#include <iostream>
#include <limits>
#include <memory>
#include <thread> 
#include <variant> 

#include "../src/interfaces/ServerTransport.h"
#include "../src/runtime/CancellationToken.h"
#include "../src/runtime/Runner.h"
#include "interfaces/Runtime.h"
#include "interfaces/RegisterBankInterface.h"

using namespace std;
using namespace std::chrono_literals;
using namespace gbx;
using namespace gbx::interfaces;
using namespace gbx::runtime;
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

class ServerTransportMock : public gbx::interfaces::ServerTransport
{
public:
    virtual ~ServerTransportMock() = default;
    MOCK_METHOD(void, WaitForClient, ());
    MOCK_METHOD(void, SendMessage, (std::shared_ptr<DebugMessage>));
    MOCK_METHOD(void, Subscribe, (std::weak_ptr<gbxcommons::Observer>));
    MOCK_METHOD(void, Unsubscribe, (std::weak_ptr<gbxcommons::Observer>));
};

TEST(TestRunner, Construction) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto runner = make_shared<Runner>(pointer);
    
    EXPECT_EQ(RunnerMode::Runtime, runner->Mode());
}

TEST(TestRunner, ConstructionInDebugMode) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto runtimePointer = static_pointer_cast<Runtime>(runtime);
    auto transport = make_shared<ServerTransportMock>();
    auto transportPointer = static_pointer_cast<ServerTransport>(transport);
    auto runner = make_shared<Runner>(runtimePointer, transportPointer);

    EXPECT_EQ(RunnerMode::Debug, runner->Mode());
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
    runner->Run(100, token);

    EXPECT_EQ(RunnerMode::Runtime, runner->Mode());
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
    EXPECT_EQ(RunnerMode::Runtime, runner->Mode());
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
    EXPECT_EQ(RunnerMode::Runtime, runner->Mode());
}

TEST(TestRunner, RunInDebugMode) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto transport = make_shared<ServerTransportMock>();
    auto transportPointer = static_pointer_cast<ServerTransport>(transport);
    auto runner = make_shared<Runner>(pointer, transportPointer);

    CancellationToken token;
    std::thread cancellationThread([&]()
    { 
        std::this_thread::sleep_for(100ms);
        runner->ClientJoined();
        std::this_thread::sleep_for(100ms);
        token.Cancel();
    });

    EXPECT_CALL((*transport), WaitForClient());   
    EXPECT_CALL((*transport), Subscribe(::_));
    EXPECT_CALL((*runtime), Run()).WillRepeatedly(Return());
    runner->Run(token);

    cancellationThread.join();
    EXPECT_TRUE(token.IsCancelled());
    EXPECT_EQ(RunnerMode::Debug, runner->Mode());
}

TEST(TestRunner, RunInDebugModeForAGivenNumberOfCycles) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto transport = make_shared<ServerTransportMock>();
    auto transportPointer = static_pointer_cast<ServerTransport>(transport);
    auto runner = make_shared<Runner>(pointer, transportPointer);

    CancellationToken token;
    std::thread cancellationThread([&]()
    { 
        std::this_thread::sleep_for(100ms);
        runner->ClientJoined();
        std::this_thread::sleep_for(100ms);
        token.Cancel();
    });

    EXPECT_CALL((*transport), WaitForClient());
    EXPECT_CALL((*transport), Subscribe(::_));
    EXPECT_CALL((*runtime), Run()).WillRepeatedly(Return());
    runner->Run(numeric_limits<size_t>::max(), token);
    
    cancellationThread.join();
    EXPECT_TRUE(token.IsCancelled());
    EXPECT_EQ(RunnerMode::Debug, runner->Mode());
}