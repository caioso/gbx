#pragma once

#include <exception>
#include <string>

namespace gbx
{

class GBXEmulatorExceptions : std::exception
{
public:
    explicit GBXEmulatorExceptions(const std::string&);
    virtual ~GBXEmulatorExceptions() = default;
    virtual const char* what() const noexcept;

protected:
    std::string _message;
};

class MessageHandlerException : public GBXEmulatorExceptions
{
public:
    explicit MessageHandlerException(const std::string&);
    ~MessageHandlerException() = default;
};

class ServerProtocolException : public GBXEmulatorExceptions
{
public:
    explicit ServerProtocolException(const std::string&);
    ~ServerProtocolException() = default;
};

}
