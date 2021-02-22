#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include "../src/interfaces/DebugRequestProducer.h"
#include "../src/interfaces/Runtime.h"
#include "../src/requests/RegisterDataRequest.h"
#include "../src/requests/RegisterDataResponse.h"
#include "../src/requests/RegisterDataRequestHandler.h"
#include "interfaces/Runtime.h"
#include "interfaces/RegisterBankInterface.h"

using namespace std;
using namespace gbx;
using namespace gbx::interfaces;
using namespace gbx::requests;
using namespace gbxcore;
using namespace gbxcore::interfaces;

using namespace ::testing;

class DummyRequest : public DebugRequest
{
public:
    DummyRequest()
        : DebugRequest(RequestType::UnknownType)
    {}

    virtual ~DummyRequest() = default;
};  

class RequestProducerMock : public DebugRequestProducer
{
public:
    virtual ~RequestProducerMock() = default;
    MOCK_METHOD(void, ConsumeResponse, (std::shared_ptr<gbx::interfaces::DebugResponse>));
};

class RuntimeMock : public Runtime
{
public:
    virtual ~RuntimeMock() = default;
    MOCK_METHOD(void, Run, ());
    MOCK_METHOD(uint8_t, ReadRegister, (Register));
};

TEST(TestRegisterDataHandler, Construction) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto runtimePointer = static_pointer_cast<Runtime>(runtime);
    auto registerDataRequestsHandler = make_shared<RegisterDataRequestHandler>();
}

TEST(TestRegisterDataHandler, ProcessIncorrectRequestType) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto runtimePointer = static_pointer_cast<Runtime>(runtime);
    auto registerDataRequestsHandler = make_shared<RegisterDataRequestHandler>();
    auto request = make_shared<DummyRequest>();
    auto requestPointer = static_pointer_cast<DebugRequest>(request);

    ASSERT_EXCEPTION( { auto response = registerDataRequestsHandler->Process(requestPointer, runtimePointer);} , 
                      RequestHandlerException, 
                      "Incorrect request type received by 'RegisterDataRequestHandler'");
}

TEST(TestRegisterDataHandler, ReadSingleRegisterTest) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto runtimePointer = static_pointer_cast<Runtime>(runtime);
    auto registerDataRequestsHandler = make_shared<RegisterDataRequestHandler>();
    auto request = make_shared<RegisterDataRequest>(Register::F, RegisterDataOperation::Read);
    auto requestPointer = static_pointer_cast<DebugRequest>(request);

    EXPECT_CALL((*runtime), ReadRegister(Register::F)).WillOnce(Return(static_cast<uint8_t>(0xAA)));    
    auto response = registerDataRequestsHandler->Process(requestPointer, runtimePointer);

    auto responsePointer = dynamic_pointer_cast<RegisterDataResponse>(response);
    EXPECT_NE(nullptr, responsePointer);
    EXPECT_EQ(0xAA, get<uint8_t>(responsePointer->Value()));
    EXPECT_EQ(Register::F, responsePointer->Register());
}

// IMPLEMENT READING PAIRS!
