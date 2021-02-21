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

}
