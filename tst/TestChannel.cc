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
    ChannelWrapperForTests(ChannelType type)
    : Channel<T>(type)
    {}

    shared_ptr<Channel<T>> Binding()
    {
        return this->_binding;
    }
};

TEST(TestChannel, Construction) 
{
    auto sourceChannel = make_shared<Channel<int>>(ChannelType::In);
    auto sinkChannel = make_shared<Channel<int>>(ChannelType::Out);
    auto duplexChannel = make_shared<Channel<int>>(ChannelType::InOut);

    EXPECT_EQ(ChannelType::In, sourceChannel->Type());
    EXPECT_EQ(ChannelType::Out, sinkChannel->Type());
    EXPECT_EQ(ChannelType::InOut, duplexChannel->Type());
}

TEST(TestChannel, Binding)
{
    auto sourceChannel = make_shared<ChannelWrapperForTests<int>>(ChannelType::In);
    auto sinkChannel = make_shared<ChannelWrapperForTests<int>>(ChannelType::Out);
    
    sourceChannel->Bind(sinkChannel);
    EXPECT_EQ(sourceChannel, sinkChannel->Binding());
    EXPECT_EQ(sinkChannel, sourceChannel->Binding());
}

TEST(TestChannel, SendIntegerMessage)
{
    auto testPassed = false;
    auto sourceChannel = make_shared<ChannelWrapperForTests<int>>(ChannelType::Out);
    auto sinkChannel = make_shared<ChannelWrapperForTests<int>>(ChannelType::In);
    
    sourceChannel->Bind(sinkChannel);
    sinkChannel->OnReceived([&testPassed](int) -> void { testPassed = true; });
    sourceChannel->Send(0xFFAA);

    EXPECT_TRUE(testPassed);
}

TEST(TestChannel, SendStringMesage)
{
    auto testPassed = false;
    auto sourceChannel = make_shared<ChannelWrapperForTests<string>>(ChannelType::Out);
    auto sinkChannel = make_shared<ChannelWrapperForTests<string>>(ChannelType::In);
    
    sourceChannel->Bind(sinkChannel);
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
            sinkChannel = make_shared<ChannelWrapperForTests<int>>(ChannelType::In);
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
    auto sourceChannel = make_shared<ChannelWrapperForTests<int>>(ChannelType::Out);
    sourceChannel->Bind(testObject.sinkChannel);
    sourceChannel->Send(0xABCD);

    EXPECT_EQ(0xABCD, testObject.ReceivedValue());
}

TEST(TestChannel, SendMessageInDuplexChannel)
{
    auto testPassed = false;
    string receivedString = "";
    auto leftChannel = make_shared<ChannelWrapperForTests<string>>(ChannelType::InOut);
    auto rightChannel = make_shared<ChannelWrapperForTests<string>>(ChannelType::InOut);
    
    leftChannel->Bind(rightChannel);
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

TEST(TestChannel, SendMessageInOutputChannel)
{
    auto testPassed = false;
    auto sourceChannel = make_shared<ChannelWrapperForTests<int>>(ChannelType::Out);
    auto sinkChannel = make_shared<ChannelWrapperForTests<int>>(ChannelType::In);
    
    sourceChannel->Bind(sinkChannel);

    try
    {
        sinkChannel->Send(1);
    }
    catch (const ChannelException& e)
    {
        testPassed = true;
    }

    EXPECT_TRUE(testPassed);
}

TEST(TestChannel, RegisterCallbackInInputChannel)
{
    auto testPassed = false;
    auto sourceChannel = make_shared<ChannelWrapperForTests<int>>(ChannelType::Out);
    auto sinkChannel = make_shared<ChannelWrapperForTests<int>>(ChannelType::In);
    
    sourceChannel->Bind(sinkChannel);

    try
    {
        sourceChannel->OnReceived([](int)->void{});
    }
    catch (const ChannelException& e)
    {
        testPassed = true;
    }

    EXPECT_TRUE(testPassed);
}

TEST(TestChannel, SendMessageInNotBoundedChannel)
{
    auto testPassed = false;
    auto sourceChannel = make_shared<ChannelWrapperForTests<int>>(ChannelType::In);
    
    try
    {
        sourceChannel->Send(1);
    }
    catch (const ChannelException& e)
    {
        testPassed = true;
    }

    EXPECT_TRUE(testPassed);
}

TEST(TestChannel, CalNullCallback)
{
    auto testPassed = false;
    auto sourceChannel = make_shared<ChannelWrapperForTests<int>>(ChannelType::Out);
    auto sinkChannel = make_shared<ChannelWrapperForTests<int>>(ChannelType::In);
    
    sourceChannel->Bind(sinkChannel);
    sinkChannel->OnReceived(nullptr);

    try
    {
        sourceChannel->Send(1);
    }
    catch (const ChannelException& e)
    {
        testPassed = true;
    }

    EXPECT_TRUE(testPassed);
}