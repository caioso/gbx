#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <random>

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
    MOCK_METHOD((std::variant<uint8_t, uint16_t>), ReadRegister, (Register));
    MOCK_METHOD(void, WriteRegister, (Register, (std::variant<uint8_t, uint16_t>)));
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
    auto targetRegister = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    auto runtime = make_shared<RuntimeMock>();
    auto runtimePointer = static_pointer_cast<Runtime>(runtime);
    auto registerDataRequestsHandler = make_shared<RegisterDataRequestHandler>();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0x00, 0xFF};

    for (auto target : targetRegister)
    {
        auto value = distribution(engine);
        auto request = make_shared<RegisterDataRequest>(target, RegisterDataOperation::Read, nullopt);
        auto requestPointer = static_pointer_cast<DebugRequest>(request);
        
        EXPECT_CALL((*runtime), ReadRegister(target)).WillOnce(Return(static_cast<uint8_t>(value)));    
        auto response = registerDataRequestsHandler->Process(requestPointer, runtimePointer);
        auto responsePointer = dynamic_pointer_cast<RegisterDataResponse>(response);
        EXPECT_NE(nullptr, responsePointer);
        EXPECT_EQ(value, get<uint8_t>(responsePointer->Value()));
        EXPECT_EQ(target, responsePointer->Register());
    }
}

TEST(TestRegisterDataHandler, ReadPairRegisterTest) 
{
    auto targetRegister = {Register::HL, Register::DE, Register::BC, Register::AF};
    auto runtime = make_shared<RuntimeMock>();
    auto runtimePointer = static_pointer_cast<Runtime>(runtime);
    auto registerDataRequestsHandler = make_shared<RegisterDataRequestHandler>();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint16_t> distribution{0x0000, 0xFFFF};

    for (auto target : targetRegister)
    {
        auto value = distribution(engine);
        auto request = make_shared<RegisterDataRequest>(target, RegisterDataOperation::Read, nullopt);
        auto requestPointer = static_pointer_cast<DebugRequest>(request);
        
        EXPECT_CALL((*runtime), ReadRegister(target)).WillOnce(Return(static_cast<uint16_t>(value)));    
        auto response = registerDataRequestsHandler->Process(requestPointer, runtimePointer);
        auto responsePointer = dynamic_pointer_cast<RegisterDataResponse>(response);
        EXPECT_NE(nullptr, responsePointer);
        EXPECT_EQ(value, get<uint16_t>(responsePointer->Value()));
        EXPECT_EQ(target, responsePointer->Register());
    }
}

TEST(TestRegisterDataHandler, WriteSingleRegisterTest) 
{
    auto targetRegister = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    auto runtime = make_shared<RuntimeMock>();
    auto runtimePointer = static_pointer_cast<Runtime>(runtime);
    auto registerDataRequestsHandler = make_shared<RegisterDataRequestHandler>();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0x00, 0xFF};

    for (auto target : targetRegister)
    {
        auto valueToWrite = distribution(engine);
        auto writeRequest = make_shared<RegisterDataRequest>(target, RegisterDataOperation::Write, make_optional(valueToWrite));
        auto writeRequestPointer = static_pointer_cast<DebugRequest>(writeRequest);
        
        auto expectationVariant = std::variant<uint8_t, uint16_t>(valueToWrite);
        EXPECT_CALL((*runtime), WriteRegister(target, expectationVariant));
        auto response = registerDataRequestsHandler->Process(writeRequestPointer, runtimePointer);
        auto responsePointer = dynamic_pointer_cast<RegisterDataResponse>(response);

        EXPECT_NE(nullptr, responsePointer);
        EXPECT_EQ(valueToWrite, get<uint8_t>(responsePointer->Value()));
        EXPECT_EQ(target, responsePointer->Register());
    }
}