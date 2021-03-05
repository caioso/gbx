#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <memory>
#include <vector>
#include <string>

#include "../src/interfaces/ServerProtocol.h"
#include "../src/requests/client-connected/ClientConnectedRequest.h"
#include "../src/requests/debug-server-status/DebugServerStatusRequest.h"
#include "../src/RequestTranslator.h"
#include "TestUtils.h"

using namespace gbx;
using namespace gbx::interfaces;
using namespace gbx::requests;
using namespace std;

using ::testing::Return;
using ::testing::_;

class RawEventSourceMock
{
public:
    RawEventSourceMock() = default;
    ~RawEventSourceMock() = default;

    void AddEventListener(std::shared_ptr<gbxcommons::Observer<interfaces::RawDebugMessageEventArgs>> observer)
    {
        _observers.push_back(observer);
    }

    void NotifyObserver(std::shared_ptr<interfaces::RawDebugMessageEventArgs> message)
    {
        for (auto observer : _observers)
            observer->Notify(message);
    }

private:
    vector<std::shared_ptr<gbxcommons::Observer<interfaces::RawDebugMessageEventArgs>>> _observers;
};

class RequestEventObserver : public gbxcommons::Observer<DebugMessageEventArgs>
{
public:
    RequestEventObserver() = default;
    ~RequestEventObserver() = default;

    MOCK_METHOD(void, Notify, (std::shared_ptr<DebugMessageEventArgs>));
};

TEST(TestRequestTranslator, Construction)
{
    auto requestTranslator = make_shared<RequestTranslator>();
}

TEST(TestRequestTranslator, DecodeStatusClientConnectedMessage)
{
    auto rawEventSource = make_shared<RawEventSourceMock>();
    auto requestTranslator = make_shared<RequestTranslator>();
    auto requestObserver = make_shared<RequestEventObserver>();

    rawEventSource->AddEventListener(requestTranslator);
    requestTranslator->AddEventListener(requestObserver);

    EXPECT_CALL((*requestObserver), Notify(::_)).WillOnce(testing::Invoke([&](std::shared_ptr<gbx::DebugMessageEventArgs> arguement)
    {
        auto message = dynamic_pointer_cast<gbx::requests::ClientConnectedRequest>(arguement->Message());
        EXPECT_NE(nullptr, arguement);
        EXPECT_EQ(MessageType::ClientConnectedRequest, message->Type());
    }));

    rawEventSource->NotifyObserver(ClientConnectedRequest::MakeRawRequest());
}

TEST(TestRequestTranslator, DecodeStatusStatusMessage)
{
    auto rawEventSource = make_shared<RawEventSourceMock>();
    auto requestTranslator = make_shared<RequestTranslator>();
    auto requestObserver = make_shared<RequestEventObserver>();

    rawEventSource->AddEventListener(requestTranslator);
    requestTranslator->AddEventListener(requestObserver);

    EXPECT_CALL((*requestObserver), Notify(::_)).WillOnce(testing::Invoke([&](std::shared_ptr<gbx::DebugMessageEventArgs> arguement)
    {
        auto message = dynamic_pointer_cast<gbx::requests::DebugServerStatusRequest>(arguement->Message());
        EXPECT_NE(nullptr, arguement);
        EXPECT_EQ(MessageType::StatusRequest, message->Type());
    }));

    rawEventSource->NotifyObserver(DebugServerStatusRequest::MakeRawRequest());
}