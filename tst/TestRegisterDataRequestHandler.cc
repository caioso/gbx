#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../src/interfaces/DebugRequestProducer.h"
#include "../src/interfaces/Runtime.h"
#include "../src/requests/RegisterDataRequest.h"
#include "../src/requests/RegisterDataRequestHandler.h"
#include "interfaces/Runtime.h"

using namespace std;
using namespace gbx;
using namespace gbx::interfaces;
using namespace gbx::requests;

class RequestProducerMock : public DebugRequestProducer
{
public:
    virtual ~RequestProducerMock() = default;
    MOCK_METHOD(void, ConsumeResponse, (std::shared_ptr<interfaces::DebugResponse>));
};

class RuntimeMock : public Runtime
{
public:
    virtual ~RuntimeMock() = default;
    MOCK_METHOD(void, Run, ());
};

TEST(TestRegisterDataMessage, Construction) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto runtimePointer = static_pointer_cast<Runtime>(runtime);
    auto registerDataRequestsHandler = make_shared<RegisterDataRequestHandler>();
}

TEST(TestRegisterDataMessage, ProcessRequest) 
{
    auto runtime = make_shared<RuntimeMock>();
    auto runtimePointer = static_pointer_cast<Runtime>(runtime);
    auto registerDataRequestsHandler = make_shared<RegisterDataRequestHandler>();
    auto request = make_shared<RegisterDataRequest>(0, RegisterDataOperation::Read, RequestType::Register);
    auto requestPointer = static_pointer_cast<DebugRequest>(request);
    
    registerDataRequestsHandler->Process(requestPointer, runtimePointer);
}
