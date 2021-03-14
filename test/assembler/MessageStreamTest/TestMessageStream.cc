#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include <memory>
#include <string>
#include <vector>

#include "MessageStream.h"
#include "StreamSink.h"
#include "OutputStream.h"

using namespace gbxasm;
using namespace gbxasm::interfaces;
using namespace gbxasm::streams;
using namespace std;

class SinkMock : public interfaces::StreamSink
{
public:
    virtual ~SinkMock() = default;
    MOCK_METHOD(void, Write, (std::string));
};

TEST(TestRemoveCommentsPass, OutputStreamConstruction)
{
    auto sinkMock = make_shared<SinkMock>();
    auto sinkPointer = static_pointer_cast<StreamSink>(sinkMock);
    auto outputStream = make_shared<streams::OutputStream>(nullopt, sinkPointer);
}

TEST(TestRemoveCommentsPass, OutputMessage)
{
    auto sinkMock = make_shared<SinkMock>();
    auto sinkPointer = static_pointer_cast<StreamSink>(sinkMock);
    auto outputStream = make_shared<streams::OutputStream>(nullopt, sinkPointer);

    EXPECT_CALL((*sinkMock), Write("Test Message"));
    outputStream->Write("Test Message", nullopt, nullopt, nullopt);
}

TEST(TestRemoveCommentsPass, OutputWithPrefixMessage)
{
    auto sinkMock = make_shared<SinkMock>();
    auto sinkPointer = static_pointer_cast<StreamSink>(sinkMock);
    auto outputStream = make_shared<streams::OutputStream>("WARNING", sinkPointer);

    EXPECT_CALL((*sinkMock), Write("WARNING: Test Message"));
    outputStream->Write("Test Message", nullopt, nullopt, nullopt);
}

TEST(TestRemoveCommentsPass, OutputWithPrefixMessageAndLocation)
{
    auto sinkMock = make_shared<SinkMock>();
    auto sinkPointer = static_pointer_cast<StreamSink>(sinkMock);
    auto outputStream = make_shared<streams::OutputStream>("warning", sinkPointer);

    EXPECT_CALL((*sinkMock), Write("MyFile.z80:44:20: warning: Test Message"));
    outputStream->Write("Test Message", "MyFile.z80", 44, 20);
}

TEST(TestRemoveCommentsPass, OutputWithIncompleteInformation)
{
    auto sinkMock = make_shared<SinkMock>();
    auto sinkPointer = static_pointer_cast<StreamSink>(sinkMock);
    auto outputStream = make_shared<streams::OutputStream>("warning", sinkPointer);

    EXPECT_CALL((*sinkMock), Write("warning: Test Message"));
    outputStream->Write("Test Message", "MyFile.z80", nullopt, nullopt);

    EXPECT_CALL((*sinkMock), Write("warning: Test Message"));
    outputStream->Write("Test Message", nullopt, 44, nullopt);
    
    EXPECT_CALL((*sinkMock), Write("warning: Test Message"));
    outputStream->Write("Test Message", nullopt, nullopt, 20);

    EXPECT_CALL((*sinkMock), Write("warning: Test Message"));
    outputStream->Write("Test Message", "MyFile.z80", 44, nullopt);
    
    EXPECT_CALL((*sinkMock), Write("warning: Test Message"));
    outputStream->Write("Test Message", "MyFile.z80", nullopt, 20);
    
    EXPECT_CALL((*sinkMock), Write("warning: Test Message"));
    outputStream->Write("Test Message", nullopt, 44, 20);
}