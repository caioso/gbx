#pragma once

#include <exception>
#include <string>

namespace gbxcommons
{

class GBXCommonsException : std::exception
{
public:
    explicit GBXCommonsException(const std::string&);
    virtual ~GBXCommonsException() = default;
    virtual const char* what() const noexcept;

protected:
    std::string _message;
};

class ArgumentsParserException : public GBXCommonsException
{
public:
    explicit ArgumentsParserException(const std::string&);
    ~ArgumentsParserException() = default;
};

}
