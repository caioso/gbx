#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "../src/Channel.h"

using namespace std;
using namespace gbx;

template<typename T>
class ChannelWrapperForTests : public Channel<T>
{
public:
    ChannelWrapperForTests()
    {}

    shared_ptr<Channel<T>> Binding()
    {
        return this->_binding;
    }
};

TEST(TestChannel, Binding)
{
    auto sourceChannel = make_shared<ChannelWrapperForTests<int>>();
    auto sinkChannel = make_shared<ChannelWrapperForTests<int>>();
    
    sourceChannel->Bind(sinkChannel);
    sinkChannel->Bind(sourceChannel);

    EXPECT_EQ(sourceChannel, sinkChannel->Binding());
    EXPECT_EQ(sinkChannel, sourceChannel->Binding());
}

TEST(TestChannel, SendIntegerMessage)
{
    auto testPassed = false;
    auto sourceChannel = make_shared<ChannelWrapperForTests<int>>();
    auto sinkChannel = make_shared<ChannelWrapperForTests<int>>();
    
    sourceChannel->Bind(sinkChannel);
    sinkChannel->Bind(sourceChannel);

    sinkChannel->OnReceived([&testPassed](int) -> void { testPassed = true; });
    sourceChannel->Send(0xFFAA);

    EXPECT_TRUE(testPassed);
}

TEST(TestChannel, SendStringMesage)
{
    auto testPassed = false;
    auto sourceChannel = make_shared<ChannelWrapperForTests<string>>();
    auto sinkChannel = make_shared<ChannelWrapperForTests<string>>();
    
    sourceChannel->Bind(sinkChannel);
    sinkChannel->Bind(sourceChannel);

    sinkChannel->OnReceived([&testPassed](string) -> void { testPassed = true; });
    sourceChannel->Send("Hello World");

    EXPECT_TRUE(testPassed);
}

TEST(TestChannel, MessageCallbackDoesWork)
{
    class TestClass
    {
    public:
        TestClass()
        {
            _receivedValue = 0;
            sinkChannel = make_shared<ChannelWrapperForTests<int>>();
            sinkChannel->OnReceived([this](int value) -> void { this->DoWork(value); });
        }

        void DoWork(int value)
        {
            _receivedValue = value;
        }

        int ReceivedValue()
        {
            return _receivedValue;
        }

        shared_ptr<ChannelWrapperForTests<int>> sinkChannel;

    private:
        int _receivedValue;
    };

    TestClass testObject;
    auto sourceChannel = make_shared<ChannelWrapperForTests<int>>();
    
    sourceChannel->Bind(testObject.sinkChannel);
    testObject.sinkChannel->Bind(sourceChannel);

    sourceChannel->Send(0xABCD);

    EXPECT_EQ(0xABCD, testObject.ReceivedValue());
}

TEST(TestChannel, SendMessageInDuplexChannel)
{
    auto testPassed = false;
    string receivedString = "";
    auto leftChannel = make_shared<ChannelWrapperForTests<string>>();
    auto rightChannel = make_shared<ChannelWrapperForTests<string>>();
    
    leftChannel->Bind(rightChannel);
    rightChannel->Bind(leftChannel);

    leftChannel->OnReceived([&leftChannel, &receivedString](string value) -> void 
    { 
        receivedString = value;
        leftChannel->Send("Test String"); 
    });
    rightChannel->OnReceived([&testPassed](string) -> void { testPassed = true; });
    rightChannel->Send("Hello World");

    EXPECT_TRUE(testPassed);
    EXPECT_EQ("Hello World", receivedString);
}