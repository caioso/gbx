#pragma once

#include <exception>
#include <string>

namespace gbx
{

class GBXException : std::exception
{
public:
    GBXException(const std::string message);
    virtual ~GBXException() = default;
    virtual const char* what() const noexcept;

protected:
    std::string _message;
};

class MemoryAccessException : public GBXException
{
public:
    MemoryAccessException(const std::string message);
    ~MemoryAccessException() = default;
};

class MemoryControllerException : public GBXException
{
public:
    MemoryControllerException(const std::string message);
    ~MemoryControllerException() = default;
};


}