#pragma once

#include <string>
#include <optional>

namespace gbxasm::interfaces
{

class MessageStream
{
public:
    virtual ~MessageStream() = default;
    virtual void Write(std::string, std::optional<std::string>, std::optional<size_t>, std::optional<size_t>) = 0;
};

}