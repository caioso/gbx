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

class ProtocolException : public GBXDebuggerExceptions
{
public:
    ProtocolException(const std::string&);
    ~ProtocolException() = default;
};

class CommandLineInputException : public GBXDebuggerExceptions
{
public:
    CommandLineInputException(const std::string&);
    ~CommandLineInputException() = default;
};

class CommandLineOutputDriverException : public GBXDebuggerExceptions
{
public:
    CommandLineOutputDriverException(const std::string&);
    ~CommandLineOutputDriverException() = default;
};

}
