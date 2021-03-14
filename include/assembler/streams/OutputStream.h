#pragma once

#include <iostream>
#include <memory>
#include <sstream>

#include "MessageStream.h"
#include "StreamSink.h"

namespace gbxasm::streams
{

class OutputStream : public interfaces::MessageStream
{
public:
    OutputStream(std::optional<std::string>, std::shared_ptr<interfaces::StreamSink>);
    virtual ~OutputStream() = default;

    OutputStream(const OutputStream&) = default;
    OutputStream(OutputStream&&) = default;
    OutputStream& operator=(const OutputStream&) = default;
    OutputStream& operator=(OutputStream&&) = default;

    void Write(std::string, std::optional<std::string>, std::optional<size_t>, std::optional<size_t>) override;

private:
    std::shared_ptr<interfaces::StreamSink> _sink;
    std::string _prefix;
};

}