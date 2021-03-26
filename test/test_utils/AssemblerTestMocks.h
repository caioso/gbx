#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <memory>
#include <variant>

#include "MessageStream.h"
#include "StreamSink.h"

using ::testing::Return;
using ::testing::_;

class StreamMock : public gbxasm::interfaces::MessageStream
{
public:
    virtual ~StreamMock() = default;
    MOCK_METHOD(void, Write, (std::string, (std::optional<std::string>), (std::optional<size_t>), (std::optional<size_t>)));
};

class SinkMock : public gbxasm::interfaces::StreamSink
{
public:
    virtual ~SinkMock() = default;
    MOCK_METHOD(void, Write, (std::string));
};