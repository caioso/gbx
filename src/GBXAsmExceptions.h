#pragma once

#include <exception>
#include <string>

namespace gbxasm
{

class GBXAsmException : std::exception
{
public:
    GBXAsmException(const std::string);
    virtual ~GBXAsmException() = default;
    virtual const char* what() const noexcept;

protected:
    std::string _message;
};

class LexerException : public GBXAsmException
{
public:
    LexerException(const std::string);
    ~LexerException() = default;
};

class ParserException : public GBXAsmException
{
public:
    ParserException(const std::string);
    ~ParserException() = default;
};
}