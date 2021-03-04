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

    void NotifyObserver(array<uint8_t, interfaces::MaxRawRequestSize> message)
    {
        auto eventArgs = make_shared<interfaces::RawDebugMessageEventArgs>(message);
        for (auto observer : _observers)
            observer->Notify(eventArgs);
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

TEST(TestRequestTranslator, TranslateRawRequest)
{
    array<uint8_t, interfaces::MaxRawRequestSize> request = 
    {{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }};

    auto rawEventSource = make_shared<RawEventSourceMock>();
    auto requestTranslator = make_shared<RequestTranslator>();

    rawEventSource->AddEventListener(requestTranslator);
    rawEventSource->NotifyObserver(request);
}

TEST(TestRequestTranslator, NotifyListener)
{
    array<uint8_t, interfaces::MaxRawRequestSize> request = 
    {{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }};

    auto rawEventSource = make_shared<RawEventSourceMock>();
    auto requestTranslator = make_shared<RequestTranslator>();
    auto requestObserver = make_shared<RequestEventObserver>();

    rawEventSource->AddEventListener(requestTranslator);
    requestTranslator->AddEventListener(requestObserver);

    EXPECT_CALL((*requestObserver), Notify(::_));
    rawEventSource->NotifyObserver(request);
}

TEST(TestRequestTranslator, DecodeStatusClientConnectedMessage)
{
    array<uint8_t, interfaces::MaxRawRequestSize> request = 
    {{
        0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }};

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

    rawEventSource->NotifyObserver(request);
}

TEST(TestRequestTranslator, DecodeStatusStatusMessage)
{
    array<uint8_t, interfaces::MaxRawRequestSize> request = 
    {{
        0xFE, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }};

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

    rawEventSource->NotifyObserver(request);
}