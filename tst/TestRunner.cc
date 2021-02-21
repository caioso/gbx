#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <chrono>
#include <iostream>
#include <limits>
#include <memory>
#include <thread> 

#include "../src/interfaces/DebugRequest.h"
#include "../src/interfaces/DebugRequestConsumer.h"
#include "../src/interfaces/DebugRequestProducer.h"
#include "../src/CancellationToken.h"
#include "../src/Runner.h"
#include "interfaces/Runtime.h"
#include "interfaces/RegisterBankInterface.h"

using namespace std;
using namespace std::chrono_literals;
using namespace gbx;
using namespace gbx::interfaces;
using ::testing::Return;
using ::testing::_;

class DummyRequest : public DebugRequest
{
public:
    DummyRequest()
        : DebugRequest(RequestType::UnknownType)
    {}

    virtual ~DummyRequest() = default;
};  

class RuntimeMock : public Runtime
{
public:
    virtual ~RuntimeMock() = default;
    MOCK_METHOD(void, Run, ());
    MOCK_METHOD(uint8_t, ReadRegister, (Register));
};

class RequestProducerMock : public DebugRequestProducer
{
public:
    virtual ~RequestProducerMock() = default;
    MOCK_METHOD(void, ConsumeResponse, (std::shared_ptr<interfaces::DebugResponse>));
};

TEST(TestRunner, Construction) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto producer = make_shared<RequestProducerMock>();
    auto producerPointer = static_pointer_cast<DebugRequestProducer>(producer);
    auto runner = make_shared<Runner>(pointer, producerPointer);
}

TEST(TestRunner, RunForANumberOfCycles) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto producer = make_shared<RequestProducerMock>();
    auto producerPointer = static_pointer_cast<DebugRequestProducer>(producer);
    auto runner = make_shared<Runner>(pointer, producerPointer);

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
    auto producer = make_shared<RequestProducerMock>();
    auto producerPointer = static_pointer_cast<DebugRequestProducer>(producer);
    auto runner = make_shared<Runner>(pointer, producerPointer);

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
    auto producer = make_shared<RequestProducerMock>();
    auto producerPointer = static_pointer_cast<DebugRequestProducer>(producer);
    auto runner = make_shared<Runner>(pointer, producerPointer);

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
    auto producer = make_shared<RequestProducerMock>();
    auto producerPointer = static_pointer_cast<DebugRequestProducer>(producer);
    auto runner = make_shared<Runner>(pointer, producerPointer);


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
    auto producer = make_shared<RequestProducerMock>();
    auto producerPointer = static_pointer_cast<DebugRequestProducer>(producer);
    auto runner = make_shared<Runner>(pointer, producerPointer);

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

TEST(TestRunner, ConsumeSingleRequestTest)
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto producer = make_shared<RequestProducerMock>();
    auto producerPointer = static_pointer_cast<DebugRequestProducer>(producer);
    auto runner = make_shared<Runner>(pointer, producerPointer);
    auto request = make_shared<DummyRequest>();
    auto requestPointer = static_pointer_cast<DebugRequest>(request);

    EXPECT_CALL((*runtime), Run());
    EXPECT_CALL((*producer), ConsumeResponse(::_));
    CancellationToken token;
    runner->ConsumeRequest(requestPointer);
    runner->RunWithDebugSupport(1, token);
}

TEST(TestRunner, ConsumeSingleRequestInDetachedModeTest)
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto producer = make_shared<RequestProducerMock>();
    auto producerPointer = static_pointer_cast<DebugRequestProducer>(producer);
    auto runner = make_shared<Runner>(pointer, producerPointer);
    auto request = make_shared<DummyRequest>();
    auto requestPointer = static_pointer_cast<DebugRequest>(request);

    EXPECT_CALL((*runtime), Run());
    EXPECT_CALL((*producer), ConsumeResponse(::_)).Times(0);
    CancellationToken token;
    runner->ConsumeRequest(requestPointer);
    runner->Run(1, token);
}

TEST(TestRunner, ConsumeMultipleRequestsTest)
{
    auto runtime = make_shared<RuntimeMock>();
    auto pointer = static_pointer_cast<Runtime>(runtime);
    auto producer = make_shared<RequestProducerMock>();
    auto producerPointer = static_pointer_cast<DebugRequestProducer>(producer);
    auto runner = make_shared<Runner>(pointer, producerPointer);
    auto request = make_shared<DummyRequest>();
    auto requestPointer = static_pointer_cast<DebugRequest>(request);

    EXPECT_CALL((*runtime), Run()).Times(1);
    EXPECT_CALL((*producer), ConsumeResponse(::_)).Times(3);
    CancellationToken token;
    runner->ConsumeRequest(requestPointer);
    runner->ConsumeRequest(requestPointer);
    runner->ConsumeRequest(requestPointer);
    runner->RunWithDebugSupport(1, token);
}

TEST(TestRunner, RequestReadRegister)
{
    auto runTime = make_shared<RuntimeMock>();
 
    EXPECT_CALL((*runTime), ReadRegister(Register::A)).WillOnce(Return(static_cast<uint8_t>(0xFE)));
    auto registerValue = runTime->ReadRegister(Register::A);

    EXPECT_EQ(0xFE, registerValue);
}