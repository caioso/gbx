#pragma once

#include <exception>
#include <string>

namespace gbx
{

class GBXException : std::exception
{
public:
    explicit GBXException(const std::string&);
    virtual ~GBXException() = default;
    virtual const char* what() const noexcept;

protected:
    std::string _message;
};

class RequestHandlerException : public GBXException
{
public:
    explicit RequestHandlerException(const std::string&);
    ~RequestHandlerException() = default;
};

class DebugServerException : public GBXException
{
public:
    explicit DebugServerException(const std::string&);
    ~DebugServerException() = default;
};

class ServerProtocolException : public GBXException
{
public:
    explicit ServerProtocolException(const std::string&);
    ~ServerProtocolException() = default;
};

}
