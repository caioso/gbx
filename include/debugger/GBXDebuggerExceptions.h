#pragma once

#include <exception>
#include <string>

namespace gbxdb
{

class GBXDebuggerExceptions : std::exception
{
public:
    explicit GBXDebuggerExceptions(const std::string&);
    virtual ~GBXDebuggerExceptions() = default;
    virtual const char* what() const noexcept;

protected:
    std::string _message;
};

class MessageHandlerException : public GBXDebuggerExceptions
{
public:
    MessageHandlerException(const std::string&);
    ~MessageHandlerException() = default;
};

class ServerProtocolException : public GBXDebuggerExceptions
{
public:
    ServerProtocolException(const std::string&);
    ~ServerProtocolException() = default;
};

}
